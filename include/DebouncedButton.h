#ifndef DEBOUNCED_BUTTON_H
#define DEBOUNCED_BUTTON

#include <Arduino.h>

class DebouncedButton {
private:
	const uint32_t PIN;
	uint32_t lastClickTime_ms = 0;
	bool buttonWasPressed = false;

public:
	DebouncedButton(uint32_t pin);
	bool wasClicked();
};

#endif
