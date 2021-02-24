#ifndef KEY_H
#define KEY_H

#include <Arduino.h>

struct Key {
	double frequency;

	double primaryVoltage;
	uint32_t primaryAnalogOutput;

	double secondaryVoltage;
	uint32_t secondaryAnalogOutput;

	bool on;
};

#endif
