#include "Arduino.h"
#include "HardwareSerial.h"

//#include "src/include/rng.h"


void setup() {

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	const unsigned short ANALOG_RESOLUTION = 12;
	const unsigned short ANALOG_PIN = A0; // This analog pin must be floating!

	unsigned long analogVal[1000];
	char miniAnalogVal[1000];
	unsigned long zeros = 0;
	unsigned long ones = 0;
	unsigned long twos = 0;
	unsigned long threes = 0;
	unsigned long fours = 0;
	unsigned long fives = 0;
	unsigned long sixes = 0;
	unsigned long sevens = 0;
	unsigned long eights = 0;
	unsigned long nines = 0;

	unsigned long previousAnalogVal;
	unsigned long timeVal;
	unsigned long seedVal;

	const unsigned short base = 0x100;


	analogReadResolution(ANALOG_RESOLUTION);

	for(unsigned short i = 0; i < 1000; i += 1) {
		analogVal[i] = analogRead(ANALOG_PIN);
	}

	for(unsigned short i = 0; i < 1000; i += 1) {
		miniAnalogVal[i] = analogVal[i] % 10;

		if(miniAnalogVal[i] == 0) {
			zeros += 1;
		} else if(miniAnalogVal[i] == 1) {
			ones += 1;
		} else if(miniAnalogVal[i] == 2) {
			twos += 1;
		} else if(miniAnalogVal[i] == 3) {
			threes += 1;
		} else if(miniAnalogVal[i] == 4) {
			fours += 1;
		} else if(miniAnalogVal[i] == 5) {
			fives += 1;
		} else if(miniAnalogVal[i] == 6) {
			sixes += 1;
		} else if(miniAnalogVal[i] == 7) {
			sevens += 1;
		} else if(miniAnalogVal[i] == 8) {
			eights += 1;
		} else {
			nines += 1;
		}
	}

	Serial.println(zeros);
	Serial.println(ones);
	Serial.println(twos);
	Serial.println(threes);
	Serial.println(fours);
	Serial.println(fives);
	Serial.println(sixes);
	Serial.println(sevens);
	Serial.println(eights);
	Serial.println(nines);
	Serial.println();

/*
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
*/
}


void loop() {

}


/*
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
	RNGen();
	~RNGen();

	void generateBytes(char*, unsigned short);
};


RNGen::RNGen() {

}


RNGen::~RNGen() {

}


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
*/