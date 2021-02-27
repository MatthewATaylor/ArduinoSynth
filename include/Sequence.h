#ifndef SEQUENCE_H
#define SEQUENCE_H

#include <Arduino.h>

class Sequence {
private:
	static const uint8_t REST_ID = 255;

public:
	uint8_t *sequence = nullptr;
	size_t capacity = 0;
	size_t length = 0;

	void expand();
	void append(uint8_t keyIndex);
	void reset();
	void move(Sequence *otherSequence);
	void print();
};

#endif
