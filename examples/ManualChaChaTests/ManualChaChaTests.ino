/*
This file exists to help users validate the implementation of the ChaCha20 streamcipher used in LiteChaCha, 
as well as to test its runtime.

Relevant test vectors can be found in TestVectors.txt.

In order to use this file, simply input the desired variables into the User Inputs section below and run it.
*/


#include "chacha.h"


void stringToHex(unsigned char* hexOut, const char* s, const unsigned short length = 32) {
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


void printHex(const unsigned char* h, const unsigned short length) {
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
	ChaChaEncryption cipher;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


// -------------------- Necessary Variables --------------------

	const size_t KEY_BYTES = 32;
	const size_t FIXED_NONCE_BYTES = 4;
	const size_t TERMINATOR_BYTE = 1;
	const size_t EMPTY_MESSAGE_BYTES = 64;

	unsigned char key[KEY_BYTES];
	const unsigned char fixedNonce[FIXED_NONCE_BYTES] = {0x00, 0x00, 0x00, 0x00};

	const unsigned long long dummyMessageLength = 1;
	unsigned char dummyMessage[dummyMessageLength] = "";

	unsigned long timestamp = 0;


// -------------------- User Inputs --------------------

	const char keyBuffer[(KEY_BYTES*2) + TERMINATOR_BYTE] = "0000000000000000000000000000000000000000000000000000000000000000";
	unsigned long long nonce = 0;
	const unsigned long initialBlockCounter = 0;

	size_t plaintextBytes = 0;
	const char plaintextBuffer[(plaintextBytes*2) + TERMINATOR_BYTE] = "";


// -------------------- Test Suite --------------------

	Serial.println(F("-------------------- ChaCha Test Begin --------------------"));
	Serial.println();

	stringToHex(key, keyBuffer);

	unsigned char* message;
	if(!plaintextBytes) {
		plaintextBytes = EMPTY_MESSAGE_BYTES;
		message = new unsigned char[plaintextBytes] {0};
	} else {
		message = new unsigned char[plaintextBytes];
		stringToHex(message, plaintextBuffer, plaintextBytes);
	}

	Serial.flush();
	timestamp = micros();
		cipher.buildEncryption(key, fixedNonce, fixedNonce);
	timestamp = micros() - timestamp;
	Serial.print(F("Build Duration: "));
	Serial.print(timestamp);
	Serial.println(F(" (us) ..."));
	Serial.println();

	for(unsigned long long i = 0; i < nonce; i += 1) {
		cipher.encryptMessage(dummyMessage, dummyMessageLength);
	}

	Serial.flush();
	timestamp = micros();
		nonce = cipher.getNonceCounter();
		cipher.encryptMessage(message, plaintextBytes, initialBlockCounter);
	timestamp = micros() - timestamp;
	Serial.print(F("Encryption Duration: "));
	Serial.print(timestamp);
	Serial.println(F(" (us) ..."));
	Serial.println(F("Ciphertext (hex):"));
	printHex(message, plaintextBytes);

	Serial.flush();
	timestamp = micros();
		cipher.decryptMessage(message, plaintextBytes, nonce, initialBlockCounter);
	timestamp = micros() - timestamp;
	Serial.print(F("Decryption Duration: "));
	Serial.print(timestamp);
	Serial.println(F(" (us) ..."));
	Serial.println(F("Decrypted Message (hex):"));
	printHex(message, plaintextBytes);

	delete[] message;

	Serial.println(F("-------------------- ChaCha Test End --------------------"));
}


void loop() {}