#include "DebouncedButton.h"

DebouncedButton::DebouncedButton(uint32_t pin) : PIN(pin) {}

bool DebouncedButton::wasClicked() {
	// if (millis() - lastClickTime_ms > 50) {
	// 	bool buttonIsPressed = digitalRead(PIN);
	// 	if (buttonIsPressed != buttonWasPressed) {
	// 		buttonWasPressed = buttonIsPressed;
	// 		return buttonIsPressed;
	// 	}
	// }
	return false;
}
