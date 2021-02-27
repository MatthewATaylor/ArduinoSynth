#include "Sequencer.h"

Sequencer::Sequencer(
	double analogMax, uint32_t appendPin, uint32_t restPin,
	uint32_t resetPin, uint32_t noteLengthPin, uint32_t ledPin
) : ANALOG_MAX(analogMax), APPEND_PIN(appendPin), REST_PIN(restPin), RESET_PIN(resetPin),
	NOTE_LENGTH_PIN(noteLengthPin), LED_PIN(ledPin), restButton(REST_PIN) {

	pinMode(appendPin, INPUT);
	pinMode(restPin, INPUT);
	pinMode(resetPin, INPUT);
	pinMode(ledPin, OUTPUT);
}

bool Sequencer::isAppendingSequence() {
	return digitalRead(APPEND_PIN) == HIGH;
}
bool Sequencer::isResettingSequence() {
	return digitalRead(RESET_PIN) == HIGH;
}
bool Sequencer::isConfiguring() {
	return isAppendingSequence() || isResettingSequence();
}
bool Sequencer::isPlaying() {
	return mainSequence.length > 0;
}

uint32_t Sequencer::getNoteLength_ms() {
	return analogRead(NOTE_LENGTH_PIN) / ANALOG_MAX * NOTE_LENGTH_MAX_MS;
}

void Sequencer::append(uint8_t keyIndex) {
	if (isAppendingSequence()) {
		mainSequence.append(keyIndex);
	}
	else if (isResettingSequence()) {
		tempSequence.append(keyIndex);
	}
}
void Sequencer::update(Key *keys, uint8_t numKeys) {
	bool resetting = isResettingSequence();

	if (!resetting && wasResettingSequence) {
		// Replace mainSequence with tempSequence
		mainSequence.move(&tempSequence);
		if (mainSequence.length == 0) {
			for (uint8_t i = 0; i < numKeys; ++i) {
				keys[i].on = false;
			}
			digitalWrite(LED_PIN, LOW);
		}
	}

	wasResettingSequence = resetting;

	if (restButton.wasClicked()) {
		append(REST_ID);
	}

	// Turn on notes in sequence
	if (mainSequence.length > 0) {
		uint32_t currentTime_ms = millis();
		if (currentTime_ms - prevNoteStartTime_ms > getNoteLength_ms() || noteIndex == -1) {
			++noteIndex;
			if (static_cast<size_t>(noteIndex) >= mainSequence.length) {
				noteIndex = 0;
			}
			prevNoteStartTime_ms = currentTime_ms;
			digitalWrite(LED_PIN, ledWasOn ? LOW : HIGH);
			ledWasOn = !ledWasOn;
		}
		for (uint8_t i = 0; i < numKeys; ++i) {
			if (i == mainSequence.sequence[noteIndex]) {
				keys[i].on = true;
			}
			else {
				keys[i].on = false;
			}
		}
	}
}
