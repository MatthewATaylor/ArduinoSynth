#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_MCP4725.h>

#include "Key.h"
#include "SerialReader.h"

const uint8_t NUM_KEYS = 32;
const double MIN_KEY_HERTZ = 87.30706; // F2

const double MAX_OUTPUT_VOLTAGE = 3.3;
const uint32_t ANALOG_MAX = 4095;
const uint32_t DETUNE_PIN = 0;
const uint32_t OSCILLOSCOPE_PIN = 1;

Adafruit_MCP4725 primaryDac;
Adafruit_MCP4725 secondaryDac;
Key keys[NUM_KEYS];
SerialReader serialReader;

int getArgumentInt(char *command, size_t length, size_t prefixLength) {
	char argument[length - prefixLength + 1];
	for (size_t i = prefixLength; i < length; ++i) {
		argument[i - prefixLength] = command[i];
	}
	argument[length - prefixLength] = '\0';
	return atoi(argument);
}

void readKeyboard() {
	char *serialBuffer = serialReader.getBuffer('\n');
	if (!serialBuffer) {
		// Data not ready
		return;
	}
	uint8_t bufferLength = serialReader.getBufferLength();
	// Serial.println("ECHO | " + String(serialBuffer));

	uint8_t noteOnPrefixLength = 3;
	uint8_t noteOffPrefixLength = 4;

	// Set keys on or off based on MIDI keyboard input
	if (bufferLength > noteOnPrefixLength && serialBuffer[0] == 'O' && serialBuffer[1] == 'N') {
		int noteId = getArgumentInt(serialBuffer, bufferLength, noteOnPrefixLength);
		if (noteId >= 0 && noteId < NUM_KEYS) {
			keys[noteId].on = true;
		}
	}
	else if (bufferLength > noteOffPrefixLength &&
		serialBuffer[0] == 'O' && serialBuffer[1] == 'F' && serialBuffer[2] == 'F'
	) {
		int noteId = getArgumentInt(serialBuffer, bufferLength, noteOffPrefixLength);
		if (noteId >= 0 && noteId < NUM_KEYS) {
			keys[noteId].on = false;
		}
	}
}

void playKey(uint8_t index) {
	primaryDac.setVoltage(keys[index].primaryAnalogOutput, false);

	int detuneValue = analogRead(DETUNE_PIN);
	double detuneExponent = (detuneValue - ANALOG_MAX / 2.0) *
		14.0 / static_cast<double>(ANALOG_MAX);
	double detuneMultiplier = pow(2.0, detuneExponent / 12.0);
	secondaryDac.setVoltage(keys[index].secondaryAnalogOutput * detuneMultiplier, false);

	// Serial.print("INFO | DAC 1 target voltage: ");
	// Serial.print(String(keys[index].primaryVoltage, 4));
	// Serial.print(" | DAC 2 target voltage: ");
	// Serial.print(String(keys[index].secondaryVoltage, 4));
	// Serial.print(" | Target frequency: ");
	// Serial.println(String(keys[index].frequency, 2));
}

void playNotes() {
	// Play the lowest note that's on
	for (uint8_t i = 0; i < NUM_KEYS; ++i) {
		if (keys[i].on) {
			playKey(i);
			return;
		}
	}
	primaryDac.setVoltage(0, false);
	secondaryDac.setVoltage(0, false);
}

//Models determined experimentally
double getPrimaryVoltageForFrequency(double frequency) {
	return 0.00000029 * frequency * frequency + 0.00260996 * frequency + 0.03378609;
}
double getSecondaryVoltageForFrequency(double frequency) {
	return 0.00000022 * frequency * frequency + 0.00257616 * frequency + 0.04023571;
}

void setup() {
	Serial.begin(115200);
	//Serial.setTimeout(4);

	primaryDac.begin(0x60);
	secondaryDac.begin(0x61);

	analogReadResolution(12);
	analogWriteResolution(12);

	for (uint8_t i = 0; i < NUM_KEYS; ++i) {
		keys[i].frequency = MIN_KEY_HERTZ * pow(2.0, i / 12.0);

		keys[i].primaryVoltage = getPrimaryVoltageForFrequency(keys[i].frequency);
		keys[i].primaryAnalogOutput = static_cast<uint32_t>(
			keys[i].primaryVoltage / MAX_OUTPUT_VOLTAGE * ANALOG_MAX
		);

		keys[i].secondaryVoltage = getSecondaryVoltageForFrequency(keys[i].frequency);
		keys[i].secondaryAnalogOutput = static_cast<uint32_t>(
			keys[i].secondaryVoltage / MAX_OUTPUT_VOLTAGE * ANALOG_MAX
		);
	}
}

void loop() {
	readKeyboard();
	playNotes();

	double testPointVoltage = analogRead(OSCILLOSCOPE_PIN) /
		static_cast<double>(ANALOG_MAX) * MAX_OUTPUT_VOLTAGE;

	Serial.println("OSC | " + String(micros()) + " | " + String(testPointVoltage, 4));
}
