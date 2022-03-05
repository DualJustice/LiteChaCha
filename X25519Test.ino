#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/X25519.h"


/*
This file exists to help users validate the implementation of the X25519 algorithm used in LiteChaCha, 
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
	X25519KeyExchange ecdhe;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


// -------------------- Necessary Variables --------------------

	const size_t SCALAR_BYTES = 32;
	const size_t U_COORD_BYTES = 32;
	const size_t TERMINATOR_BYTE = 1;

	unsigned char scalar[SCALAR_BYTES];
	unsigned char uCoord[U_COORD_BYTES];

	unsigned long timestamp = 0;


// ==================== User Inputs ====================

	const char scalarBuffer[(SCALAR_BYTES*2) + TERMINATOR_BYTE] = "a546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4";
	const char uCoordBuffer[(U_COORD_BYTES*2) + TERMINATOR_BYTE] = "e6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c";


// -------------------- Test Suite --------------------

	Serial.println(F("-------------------- X25519 Test Begin --------------------"));
	Serial.println();

	stringToHex(scalar, scalarBuffer);
	stringToHex(uCoord, uCoordBuffer);

	Serial.flush();
	timestamp = micros();
		ecdhe.curve25519(scalar, uCoord);
	timestamp = micros() - timestamp;
	Serial.print(F("Test Duration: "));
	Serial.print(timestamp);
	Serial.println(F(" (us) ..."));
	Serial.println(F("Output Key (hex):"));
	printHex(uCoord);

	Serial.println(F("-------------------- X25519 Test End --------------------"));
}


void loop() {}