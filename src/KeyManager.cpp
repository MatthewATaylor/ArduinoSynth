#include "KeyManager.h"

double KeyManager::getPrimaryVoltageForFrequency(double frequency) {
	return 0.00000029 * frequency * frequency + 0.00260996 * frequency + 0.03378609;
}
double KeyManager::getSecondaryVoltageForFrequency(double frequency) {
	return 0.00000022 * frequency * frequency + 0.00257616 * frequency + 0.04023571;
}

KeyManager::KeyManager(
	uint8_t numKeys, double minKeyFrequency, uint8_t gatePin,
	double analogMax, double maxOutputVoltage
) : NUM_KEYS(numKeys), MIN_KEY_FREQUENCY(minKeyFrequency), GATE_PIN(gatePin), ANALOG_MAX(analogMax) {

	keys = new Key*[numKeys];
	for (uint8_t i = 0; i < numKeys; ++i) {
		double frequency = minKeyFrequency * pow(2.0, i / 12.0);

		double primaryVoltage = getPrimaryVoltageForFrequency(frequency);
		uint32_t primaryAnalogOutput = static_cast<uint32_t>(
			primaryVoltage / maxOutputVoltage * analogMax
		);

		double secondaryVoltage = getSecondaryVoltageForFrequency(frequency);
		uint32_t secondaryAnalogOutput = static_cast<uint32_t>(
			secondaryVoltage / maxOutputVoltage * analogMax
		);

		keys[i] = new Key(
			frequency,
			primaryVoltage, primaryAnalogOutput,
			secondaryVoltage, secondaryAnalogOutput
		);
	}

	enabledKeys = new Key*[numKeys];
	for (uint8_t i = 0; i < numKeys; ++i) {
		enabledKeys[i] = nullptr;
	}

	pinMode(gatePin, OUTPUT);
}

void KeyManager::enableKey(uint8_t index) {
	if (keys[index]->enableQueueIndex != -1) {
		// Key already enabled
		return;
	}

	enabledKeys[numEnabledKeys] = keys[index];
	keys[index]->enableQueueIndex = numEnabledKeys;
	++numEnabledKeys;
}
void KeyManager::disableKey(uint8_t index) {
	if (keys[index]->enableQueueIndex == -1 || numEnabledKeys == 0) {
		// Cannot disable key
		return;
	}

	// Remove key from queue
	enabledKeys[keys[index]->enableQueueIndex] = nullptr;

	// Shift keys in queue
	for (uint8_t i = static_cast<uint8_t>(keys[index]->enableQueueIndex) + 1; i < numEnabledKeys; ++i) {
		--(enabledKeys[i]->enableQueueIndex);
		enabledKeys[i - 1] = enabledKeys[i];
	}
	enabledKeys[numEnabledKeys - 1] = nullptr;

	keys[index]->enableQueueIndex = -1;
	--numEnabledKeys;
}
void KeyManager::disableAllKeys() {
	for (uint8_t i = 0; i < numEnabledKeys; ++i) {
		enabledKeys[i]->enableQueueIndex = -1;
		enabledKeys[i] = nullptr;
	}
	numEnabledKeys = 0;
}
void KeyManager::disableAllKeysExcept(uint8_t index) {
	disableAllKeys();
	if (index < NUM_KEYS) {
		enableKey(index);
	}
}

void KeyManager::playLastKey(
	Adafruit_MCP4725 *primaryDac, Adafruit_MCP4725 *secondaryDac,
	uint32_t detuneValue, bool debug
) {
	if (numEnabledKeys > 0) {
		enabledKeys[numEnabledKeys - 1]->play(primaryDac, secondaryDac, detuneValue, ANALOG_MAX, debug);
	}
	else {
		primaryDac->setVoltage(0, false);
		secondaryDac->setVoltage(0, false);
	}
}

void KeyManager::setGate() {
	int gateValue;
	if (numEnabledKeys > 0) {
		gateValue = HIGH;
	}
	else {
		gateValue = LOW;
	}
	digitalWrite(GATE_PIN, gateValue);
}

uint8_t KeyManager::getNumKeys() {
	return NUM_KEYS;
}
