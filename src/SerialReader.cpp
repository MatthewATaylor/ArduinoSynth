#include "SerialReader.h"

char *SerialReader::getBuffer(char delimiter) {
	while (Serial.available()) {
		char nextChar = static_cast<char>(Serial.read());

		if (nextChar == delimiter || bufferLength >= BUFFER_CAPACITY - 1) {
			// End of message
			buffer[bufferLength] = '\0';
			prevBufferLength = bufferLength;
			bufferLength = 0;  // Reset for further reading
			return buffer;
		}

		buffer[bufferLength] = nextChar;
		++bufferLength;
	}
	return nullptr;
}

uint8_t SerialReader::getBufferLength() {
	return prevBufferLength;
}
