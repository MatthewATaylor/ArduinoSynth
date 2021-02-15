#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_MCP4725.h>

const double MAX_OUTPUT_VOLTAGE = 3.3;

const uint8_t NUM_KEYS = 32;
const double MIN_KEY_HERTZ = 87.30706; // F2
//const double MIN_KEY_HERTZ = 174.6141; // F3

// Values determined experimentally
const double VOLTS_PER_HERTZ = 0.003835;
const double VOLTAGE_BIAS = 0.02462;

const uint32_t MAX_ANALOG_OUTPUT = 4095;

const uint8_t SERIAL_INPUT_LENGTH = 64;

struct Key {
	double frequency;
	double voltage;
	uint32_t analogOutput;
	bool on;
};

Key keys[NUM_KEYS];
Adafruit_MCP4725 dac;

size_t getSerialInput(char *buffer, size_t length) {
	size_t numBytesRead = Serial.readBytesUntil('\n', buffer, length - 1);
	buffer[numBytesRead] = '\0';
	return numBytesRead;
}

int getArgumentInt(char *command, size_t length, size_t prefixLength) {
	char argument[length - prefixLength + 1];
	for (size_t i = prefixLength; i < length; ++i) {
		argument[i - prefixLength] = command[i];
	}
	argument[length - prefixLength] = '\0';
	return atoi(argument);
}

void readKeyboard() {
	char serialBuffer[SERIAL_INPUT_LENGTH];
	size_t numReadBytes = getSerialInput(serialBuffer, SERIAL_INPUT_LENGTH);
	if (numReadBytes == 0) {
		return;
	}
	Serial.println(serialBuffer);

	uint8_t noteOnPrefixLength = 3;
	uint8_t noteOffPrefixLength = 4;

	// Set keys on or off based on MIDI keyboard input
	if (numReadBytes > noteOnPrefixLength && serialBuffer[0] == 'O' && serialBuffer[1] == 'N') {
		int noteId = getArgumentInt(serialBuffer, numReadBytes, noteOnPrefixLength);
		if (noteId >= 0 && noteId < NUM_KEYS) {
			keys[noteId].on = true;
		}
	}
	else if (numReadBytes > noteOffPrefixLength &&
		serialBuffer[0] == 'O' && serialBuffer[1] == 'F' && serialBuffer[2] == 'F'
	) {
		int noteId = getArgumentInt(serialBuffer, numReadBytes, noteOffPrefixLength);
		if (noteId >= 0 && noteId < NUM_KEYS) {
			keys[noteId].on = false;
		}
	}
}

void playKey(uint8_t index) {
	dac.setVoltage(keys[index].analogOutput, false);
	Serial.print("Target voltage: ");
	Serial.print(String(keys[index].voltage, 4));
	Serial.print("    Target frequency: ");
	Serial.println(keys[index].frequency);
}

void playNotes() {
	// Play the lowest note that's on
	for (uint8_t i = 0; i < NUM_KEYS; ++i) {
		if (keys[i].on) {
			playKey(i);
			return;
		}
	}
	dac.setVoltage(0, false);
}

void setup() {
	Serial.begin(115200);
	dac.begin(0x60);

	analogWriteResolution(12);

	for (uint8_t i = 0; i < NUM_KEYS; ++i) {
		keys[i].frequency = MIN_KEY_HERTZ * pow(2.0, i / 12.0);
		keys[i].voltage = VOLTS_PER_HERTZ * keys[i].frequency + VOLTAGE_BIAS;
		keys[i].analogOutput = static_cast<uint32_t>(
			keys[i].voltage / MAX_OUTPUT_VOLTAGE * MAX_ANALOG_OUTPUT
		);
	}
}

void loop() {
	readKeyboard();
	playNotes();
}
