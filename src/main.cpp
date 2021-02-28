#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_MCP4725.h>

#include "KeyManager.h"
#include "SerialReader.h"
#include "Sequencer.h"

const double MAX_OUTPUT_VOLTAGE = 3.3;
const double ANALOG_MAX = 4095.0;
const uint32_t DETUNE_PIN = 0;
const uint32_t OSCILLOSCOPE_PIN = 1;

Adafruit_MCP4725 primaryDac;
Adafruit_MCP4725 secondaryDac;
KeyManager keyManager(32, 87.30706 /* F2 */, 2, ANALOG_MAX, MAX_OUTPUT_VOLTAGE);
SerialReader serialReader;
Sequencer sequencer(ANALOG_MAX, 22, 23, 24, 2, 26, &keyManager);

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

	uint8_t noteOnPrefixLength = 3;  // "ON "
	uint8_t noteOffPrefixLength = 4;  // "OFF "

	// Set keys on or off based on MIDI keyboard input
	if (bufferLength > noteOnPrefixLength && serialBuffer[0] == 'O' && serialBuffer[1] == 'N') {
		int noteId = getArgumentInt(serialBuffer, bufferLength, noteOnPrefixLength);
		if (noteId >= 0 && noteId < keyManager.getNumKeys()) {
			if (sequencer.isConfiguring()) {
				sequencer.append(noteId);
			}
			else if (!sequencer.isPlaying()) {
				// Only play note when not sequencing or playing
				keyManager.enableKey(noteId);
			}
		}
	}
	else if (bufferLength > noteOffPrefixLength &&
		serialBuffer[0] == 'O' && serialBuffer[1] == 'F' && serialBuffer[2] == 'F'
	) {
		int noteId = getArgumentInt(serialBuffer, bufferLength, noteOffPrefixLength);
		if (noteId >= 0 && noteId < keyManager.getNumKeys() && !sequencer.isPlaying()) {
			keyManager.disableKey(noteId);
		}
	}
}

void setup() {
	Serial.begin(115200);

	primaryDac.begin(0x60);
	secondaryDac.begin(0x61);

	analogReadResolution(12);
	analogWriteResolution(12);
}

void loop() {
	readKeyboard();
	sequencer.update();
	keyManager.playLastKey(&primaryDac, &secondaryDac, analogRead(DETUNE_PIN), false);
	keyManager.setGate();

	double testPointVoltage = analogRead(OSCILLOSCOPE_PIN) / ANALOG_MAX * MAX_OUTPUT_VOLTAGE;
	Serial.println("OSC | " + String(micros()) + " | " + String(testPointVoltage, 4));
}
