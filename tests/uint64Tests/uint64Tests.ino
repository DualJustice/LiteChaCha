#include "Arduino.h"
#include "HardwareSerial.h"

#include <stdint.h>


void setup() {

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	uint64_t wordBuffer;
	char test = 0x7f; // 01111111

	Serial.println("-     TEST ONE:");
	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= (test << 24);

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();

	test = 0x80; // 10000000

	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= (test << 24);

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();

// --------------------------------------------------

	Serial.println("-     TEST TWO:");
	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= ((uint64_t)test << 24);

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();

	test = 0x80; // 10000000

	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= ((uint64_t)test << 24);

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();

// --------------------------------------------------

	Serial.println("-     TEST THREE:");
	uint32_t testU = 0x7f000000; // 01111111

	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= testU;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();

	testU = 0x80000000; // 10000000

	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= testU;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();

// --------------------------------------------------

	Serial.println("-     TEST FOUR:");
	wordBuffer = 0x0000000000000000;

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}

	wordBuffer |= (0x0000000000000080 << 24);

	Serial.print("wordBuffer: ");
	if(wordBuffer == 0) {
		Serial.println("0000000000000000");
	} else {
		Serial.println(wordBuffer, HEX);
	}
	Serial.println();
}


void loop() {}