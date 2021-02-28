#include "DebouncedButton.h"

DebouncedButton::DebouncedButton(uint32_t pin) : PIN(pin) {
	pinMode(pin, INPUT);
}

bool DebouncedButton::wasClicked() {
	bool buttonIsPressed = digitalRead(PIN) == HIGH;
	uint32_t currentTime_ms = millis();
	if (currentTime_ms - lastClickTime_ms > 50 && buttonIsPressed != buttonWasPressed) {
		buttonWasPressed = buttonIsPressed;
		lastClickTime_ms = currentTime_ms;
		return buttonIsPressed;
	}
	return false;
}
