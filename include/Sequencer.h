#ifndef SEQUENCER_H
#define SEQUENCER_H

#include <Arduino.h>

#include "DebouncedButton.h"
#include "Key.h"
#include "Sequence.h"

class Sequencer {
private:
	static const uint8_t REST_ID = 255;
	static const uint16_t NOTE_LENGTH_MAX_MS = 1000;

	const double ANALOG_MAX;
	const uint32_t APPEND_PIN;
	const uint32_t REST_PIN;
	const uint32_t RESET_PIN;
	const uint32_t NOTE_LENGTH_PIN;
	const uint32_t LED_PIN;

	bool wasResettingSequence = false;

	Sequence tempSequence;
	Sequence mainSequence;

	DebouncedButton restButton;

	uint32_t prevNoteStartTime_ms = 0;
	int noteIndex = -1;
	bool ledWasOn = false;

public:
	Sequencer(
		double analogMax, uint32_t appendPin, uint32_t restPin,
		uint32_t resetPin, uint32_t noteLengthPin, uint32_t ledPin
	);

	bool isAppendingSequence();
	bool isResettingSequence();
	bool isConfiguring();
	bool isPlaying();

	uint32_t getNoteLength_ms();

	void append(uint8_t keyIndex);
	void update(Key *keys, uint8_t numKeys);
};

#endif
