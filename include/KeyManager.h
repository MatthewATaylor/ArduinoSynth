#ifndef KEY_MANAGER_H
#define KEY_MANAGER_H

#include <Arduino.h>
#include <Adafruit_MCP4725.h>

#include "Key.h"

class KeyManager {
private:
	const uint8_t NUM_KEYS;
	const double MIN_KEY_FREQUENCY;
	const uint8_t GATE_PIN;
	const double ANALOG_MAX;

	Key **keys;

	Key **enabledKeys;
	uint8_t numEnabledKeys = 0;

	static double getPrimaryVoltageForFrequency(double frequency);
	static double getSecondaryVoltageForFrequency(double frequency);

public:
	KeyManager(
		uint8_t numKeys, double minKeyFrequency, uint8_t gatePin,
		double analogMax, double maxOutputVoltage
	);

	void enableKey(uint8_t index);
	void disableKey(uint8_t index);
	void disableAllKeys();
	void disableAllKeysExcept(uint8_t index);

	void playLastKey(
		Adafruit_MCP4725 *primaryDac, Adafruit_MCP4725 *secondaryDac,
		uint32_t detuneValue, bool debug
	);

	void setGate();

	uint8_t getNumKeys();
};

#endif
