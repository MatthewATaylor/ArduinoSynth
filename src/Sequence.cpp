#include "Sequence.h"

void Sequence::expand() {
	capacity += 8;
	uint8_t *newSequence = new uint8_t[capacity];
	memcpy(newSequence, sequence, length * sizeof(uint8_t));
	delete[] sequence;
	sequence = newSequence;
}

void Sequence::append(uint8_t keyIndex) {
	if (length == capacity) {
		expand();
	}
	sequence[length] = keyIndex;
	++length;
}

void Sequence::reset() {
	delete[] sequence;
	capacity = 0;
	length = 0;
}

void Sequence::move(Sequence *otherSequence) {
	reset();

	sequence = otherSequence->sequence;
	capacity = otherSequence->capacity;
	length = otherSequence->length;

	otherSequence->sequence = nullptr;
	otherSequence->reset();
}

void Sequence::print() {
	String sequenceStr = "";
	for (size_t i = 0; i < length; ++i) {
		sequenceStr += String(sequence[i]) + " ";
	}
	Serial.println("INFO | " + sequenceStr);
}
