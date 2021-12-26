#ifndef RNG_H
#define RNG_H

#include "Arduino.h"


class RNGen {
private:
	const unsigned short ANALOG_RESOLUTION = 12;
	const unsigned short ANALOG_PIN = A0; // This analog pin must be floating!

	char bit;
	unsigned long seedVal;

	const unsigned short base = 0x100;

	void initializeSeed();
public:
	void generateBytes(char*, unsigned short);
};


inline void RNGen::initializeSeed() { // This should be handled as a bare minimum implementation of RNG. I am sure you can do better than me.
	analogReadResolution(ANALOG_RESOLUTION);

	seedVal = 0;

	for(unsigned short i = 0; i < 32; i += 1) {
		bit = analogRead(ANALOG_PIN) & 0x00000001;

		seedVal = (seedVal << 1) | bit;
	}

	srand(seedVal);
}


inline void RNGen::generateBytes(char* out, unsigned short bytes) {
	initializeSeed();

	for(unsigned short i = 0; i < bytes; i += 1) {
		out[i] = rand() % base;
	}
}

#endif