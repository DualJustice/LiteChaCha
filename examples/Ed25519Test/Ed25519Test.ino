#include "Arduino.h"
#include "HardwareSerial.h"

#include "Ed25519.h"


/*
This file exists to help users validate the implementation of the Ed25519 algorithm used in LiteChaCha, 
as well as to test its runtime.

Relevant test vectors can be found in TestVectors.txt.
Note that there are two distinct sources for the relevant test vectors: RFC8032 & CONTRIVED TEST VECTORS, 
and FB RESEARCH VERIFICATION TEST VECTORS. Some variables will be unnecessary depending on the vector set.

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
	Ed25519SignatureAlgorithm hancock;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


// -------------------- Necessary Variables --------------------

	const size_t KEY_BYTES = 32;
	const size_t TERMINATOR_BYTE = 1;
	const size_t SIGNATURE_BYTES = 64;

	unsigned char secretKey[KEY_BYTES];
	unsigned char publicKey[KEY_BYTES];
	unsigned char signature[SIGNATURE_BYTES];

	const bool createPublicKey = false;
	bool valid = false;

	unsigned long timestamp = 0;


// ==================== User Inputs ====================

//	FBResearchTestVectors will be false if you are doing your own tests or are testing vectors from RFC8032 & CONTRIVED TEST VECTORS. Otherwise, it will be true.
	const bool FBResearchTestVectors = false;

//	There is no need to input secretKeyBuffer if you are testing the FB RESEARCH VERIFICATION TEST VECTORS.
	const char secretKeyBuffer[(KEY_BYTES*2) + TERMINATOR_BYTE] = "9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60";

	const size_t messageBytes = 0;
	const char messageBuffer[(messageBytes*2) + TERMINATOR_BYTE] = "";

//	There is no need to input publicKeyBuffer if you are doing your own tests or are testing the RFC8032 & CONTRIVED TEST VECTORS.
	const char publicKeyBuffer[(KEY_BYTES*2) + TERMINATOR_BYTE] = "";

	const char signatureBuffer[(SIGNATURE_BYTES*2) + TERMINATOR_BYTE] = "e5564300c360ac729086e2cc806e828a84877f1eb8e5d974d873e065224901555fb8821590a33bacc61e39701cf9b46bd25bf5f0595bbe24655141438e7a100b";


// -------------------- Test Suite --------------------

	Serial.println(F("-------------------- Ed25519 Test Begin --------------------"));
	Serial.println();

	stringToHex(secretKey, secretKeyBuffer);
	stringToHex(publicKey, publicKeyBuffer);

	unsigned char message[messageBytes];
	stringToHex(message, messageBuffer, messageBytes);

	if(!FBResearchTestVectors) {
		Serial.flush();
		timestamp = micros();
			hancock.initialize(publicKey, secretKey);
		timestamp = micros() - timestamp;
		Serial.print(F("Public Key Derivation Duration: "));
		Serial.print(timestamp);
		Serial.println(F(" (us) ..."));
		Serial.println(F("Public Key (hex):"));
		printHex(publicKey);

		Serial.flush();
		timestamp = micros();
			hancock.sign(signature, publicKey, secretKey, message, createPublicKey, messageBytes);
		timestamp = micros() - timestamp;
		Serial.print(F("Sign Duration: "));
		Serial.print(timestamp);
		Serial.println(F(" (us) ..."));
		Serial.println(F("Generated Signature (hex):"));
		printHex(signature, SIGNATURE_BYTES);

		stringToHex(signature, signatureBuffer, SIGNATURE_BYTES);

		Serial.flush();
		timestamp = micros();
			valid = hancock.verify(publicKey, message, signature, messageBytes);
		timestamp = micros() - timestamp;
		Serial.print(F("Verify Duration: "));
		Serial.print(timestamp);
		Serial.println(F(" (us) ..."));
		if(valid) {
			Serial.println(F("Input Signature Is Valid."));
		} else {
			Serial.println(F("Input Signature is Invalid."));
		}
		Serial.println();
	} else {
		stringToHex(signature, signatureBuffer, SIGNATURE_BYTES);

		Serial.flush();
		timestamp = micros();
			valid = hancock.verify(publicKey, message, signature, messageBytes);
		timestamp = micros() - timestamp;
		Serial.print(F("Verify Duration: "));
		Serial.print(timestamp);
		Serial.println(F(" (us) ..."));
		if(valid) {
			Serial.println(F("Input Signature Is Valid."));
		} else {
			Serial.println(F("Input Signature is Invalid."));
		}
		Serial.println();
	}

	Serial.println(F("-------------------- Ed25519 Test End --------------------"));
}


void loop() {}