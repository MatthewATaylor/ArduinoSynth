#ifndef KEY_H
#define KEY_H

#include <Arduino.h>
#include <Adafruit_MCP4725.h>

class Key {
private:
	const double FREQUENCY;

	const double PRIMARY_VOLTAGE;
	const uint32_t PRIMARY_ANALOG_OUTPUT;

	const double SECONDARY_VOLTAGE;
	const uint32_t SECONDARY_ANALOG_OUTPUT;

public:
	int16_t enableQueueIndex = -1;

	Key(
		double frequency,
		double primaryVoltage, uint32_t primaryAnalogOutput,
		double secondaryVoltage, uint32_t secondaryAnalogOutput
	);

	void play(
		Adafruit_MCP4725 *primaryDac, Adafruit_MCP4725 *secondaryDac,
		uint32_t detuneValue, double analogMax, bool debug
	);
};

#endif
