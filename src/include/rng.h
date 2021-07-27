#ifndef RNG_H
#define RNG_H

#include "Arduino.h"


class RNGen {
private:
	const unsigned short ANALOG_RESOLUTION = 12;
	const unsigned short ANALOG_PIN = A0; // This analog pin must be floating!

	unsigned long analogVal;
	unsigned long previousAnalogVal;
	unsigned long timeVal;
	unsigned long seedVal;

	const unsigned short base = 0x100;

	void initializeSeed();
public:
	void generateBytes(char*, unsigned short);
};


void RNGen::initializeSeed() { // Truly, this is shameful. Please do better than me.
	analogReadResolution(ANALOG_RESOLUTION);

	seedVal = micros();
	previousAnalogVal = 1;

	for(unsigned short j = 0; j < 10; j += 1) {
		timeVal = micros();

		for(unsigned short i = 0; i < 10; i += 1) {
			analogVal = analogRead(ANALOG_PIN) + 1;
			seedVal += (analogVal + (analogVal/previousAnalogVal));
			previousAnalogVal = analogVal;

			seedVal ^= analogRead(ANALOG_PIN);
			seedVal = (seedVal << 7) | (seedVal >> 25);
		}

		seedVal = (seedVal << 5) | (seedVal >> 27);

		timeVal = (micros() + 1) - timeVal;
		seedVal *= timeVal;
	}

	srand(seedVal);
}


void RNGen::generateBytes(char* out, unsigned short bytes) {
	initializeSeed();

	for(unsigned short i = 0; i < bytes; i += 1) {
		out[i] = rand() % base;
	}
}

#endif