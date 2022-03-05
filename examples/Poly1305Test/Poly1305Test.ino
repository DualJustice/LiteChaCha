#include "Arduino.h"
#include "HardwareSerial.h"

#include "poly1305.h"

#include <stdint.h>


/*
This file exists to help users validate the implementation of the Poly1305 algorithm used in LiteChaCha, 
as well as to test its runtime.

Relevant test vectors can be found in TestVectors.txt.

In order to use this file, simply input the desired variables into the User Inputs section below and run it.
*/


void stringToHex(unsigned char* hexOut, const char* s, const size_t length = 32) {
	char t[length*2];
	for(unsigned short i = 0; i < (length*2); i += 1) {
		t[i] = s[i];
	}

	for(unsigned short i = 0; i < length; i += 1) {
		if(48 <= t[i*2] && t[i*2] <= 57) {
			t[i*2] -= 48;
		} else if(65 <= t[i*2] && t[i*2] <= 70) {
			t[i*2] -= 55;
		} else {
			t[i*2] -= 87;
		}
		hexOut[i] = t[i*2];
		hexOut[i] <<= 4;

		if(48 <= t[(i*2) + 1] && t[(i*2) + 1] <= 57) {
			t[(i*2) + 1] -= 48;
		} else if(65 <= t[(i*2) + 1] && t[(i*2) + 1] <= 70) {
			t[(i*2) + 1] -= 55;
		} else {
			t[(i*2) + 1] -= 87;
		}
		hexOut[i] |= t[(i*2) + 1];
	}
}


void printHex(const unsigned char* h, const size_t length = 32) {
	for(unsigned short i = 0; i < length; i += 1) {
		if(h[i] > 0x0f) {
			Serial.print(h[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(h[i], HEX);
		}
	}
	Serial.println('\n');
}


void setup() {
	Poly1305MAC mac;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


// -------------------- Necessary Variables --------------------

	const size_t KEY_LENGTH = 8;
	const size_t TAG_BYTES = 16;
	const size_t TERMINATOR_BYTE = 1;

	unsigned char tag[TAG_BYTES];

	bool authentic = false;

	unsigned long timestamp = 0;


// ==================== User Inputs ====================

	const uint32_t key[KEY_LENGTH] = {0x85d6be78, 0x57556d33, 0x7f4452fe, 0x42d506a8, 0x0103808a, 0xfb0db2fd, 0x4abff6af, 0x4149f51b};

	size_t messageLength = 34;
	const char messageBuffer[messageLength + TERMINATOR_BYTE] = {"Cryptographic Forum Research Group"};

	const char tagBuffer[(TAG_BYTES*2) + TERMINATOR_BYTE] = "a8061dc1305136c6c22b8baf0c0127a9";


// -------------------- Test Suite --------------------

	Serial.println(F("-------------------- Poly1305 Test Begin --------------------"));
	Serial.println();

	unsigned char message[messageLength];
	for(unsigned short i = 0; i < messageLength; i += 1) {
		message[i] = messageBuffer[i];
	}

	Serial.flush();
	timestamp = micros();
		mac.createTag(tag, key, message, messageLength);
	timestamp = micros() - timestamp;
	Serial.print(F("Tag Duration: "));
	Serial.print(timestamp);
	Serial.println(F(" (us) ..."));
	Serial.println(F("Generated Tag (hex):"));
	printHex(tag, TAG_BYTES);

	stringToHex(tag, tagBuffer, TAG_BYTES);

	Serial.flush();
	timestamp = micros();
		authentic = mac.authenticateTag(tag, key, message, messageLength);
	timestamp = micros() - timestamp;
	Serial.print(F("Authentication Duration: "));
	Serial.print(timestamp);
	Serial.println(F(" (us) ..."));
	if(authentic) {
		Serial.println(F("Message Is Authentic."));
	} else {
		Serial.println(F("Message Is Inauthentic."));
	}
	Serial.println();

	Serial.println(F("-------------------- Poly1305 Test End --------------------"));
}


void loop() {}