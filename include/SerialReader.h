#ifndef SERIAL_READER_H
#define SERIAL_READER_H

#include <Arduino.h>

class SerialReader {
private:
	static const uint8_t BUFFER_CAPACITY = 64;
	char buffer[BUFFER_CAPACITY];
	uint8_t prevBufferLength = 0;
	uint8_t bufferLength = 0;

public:
	// Return serial buffer ending with delimiter or nullptr if read is incomplete
	char *getBuffer(char delimiter);

	uint8_t getBufferLength();
};

#endif
