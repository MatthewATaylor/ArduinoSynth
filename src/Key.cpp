#include "Key.h"

Key::Key(
	double frequency,
	double primaryVoltage, uint32_t primaryAnalogOutput,
	double secondaryVoltage, uint32_t secondaryAnalogOutput
) : FREQUENCY(frequency),
	PRIMARY_VOLTAGE(primaryVoltage), PRIMARY_ANALOG_OUTPUT(primaryAnalogOutput),
	SECONDARY_VOLTAGE(secondaryVoltage), SECONDARY_ANALOG_OUTPUT(secondaryAnalogOutput) {}

void Key::play(
	Adafruit_MCP4725 *primaryDac, Adafruit_MCP4725 *secondaryDac,
	uint32_t detuneValue, double analogMax, bool debug
) {
	primaryDac->setVoltage(PRIMARY_ANALOG_OUTPUT, false);

	double detuneExponent = (detuneValue - analogMax / 2.0) * 14.0 / analogMax;
	double detuneMultiplier = pow(2.0, detuneExponent / 12.0);
	secondaryDac->setVoltage(SECONDARY_ANALOG_OUTPUT * detuneMultiplier, false);

	if (debug) {
		Serial.print("INFO | DAC 1 target voltage: ");
		Serial.print(String(PRIMARY_VOLTAGE, 4));
		Serial.print(" | DAC 2 target voltage: ");
		Serial.print(String(SECONDARY_VOLTAGE, 4));
		Serial.print(" | Target frequency: ");
		Serial.println(String(FREQUENCY, 2));
	}
}
