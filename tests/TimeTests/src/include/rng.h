#ifndef RNG_H
#define RNG_H

#include "Arduino.h"
#include "HardwareSerial.h" // DELETE ME!


class RNGen {
private:
	const unsigned short ANALOG_RESOLUTION = 12;
	const unsigned short ANALOG_PIN = A0; // This pin must be floating!

	unsigned long analogVal;
	unsigned long timeVal;
	unsigned long seedVal = 0;

	const unsigned short base = 0x100;


public:
	RNGen();
	~RNGen();

	void initializeSeed();
	void generateBytes(char*, unsigned short);
};


RNGen::RNGen() {

}


RNGen::~RNGen() {

}


void RNGen::initializeSeed() { // Truly, this is shameful. Please do better than me.
	analogReadResolution(ANALOG_RESOLUTION);

	for(unsigned short i = 0; i < 100; i += 1) {
		analogVal = analogRead(ANALOG_PIN);
		timeVal = micros();
		seedVal ^= (analogVal + timeVal);
		seedVal = (seedVal << 8) | (seedVal >> 24);
	}

	srand(seedVal);
}


void RNGen::generateBytes(char* out, unsigned short bytes) {
	for(unsigned short i = 0; i < 10; i += 1) {
		initializeSeed();
		Serial.println(seedVal);
	}


}

#endif