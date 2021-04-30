#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/chacha.h"


static const unsigned short BAUD_RATE = 9600;
static ChaChaEncryption& cipher = *new ChaChaEncryption();


void printHex(char* data, const unsigned short length) { // Boy do I sure love Serial.print!
	for(unsigned short i = 0; i < length; i += 1) {
		if(data[i] > 0x0f) {
			Serial.print(data[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(data[i], HEX);
		}
	}

	Serial.println('\n');
}


bool setupEncryption() {
	// print some useful messages. [DONE]
	// generate random nonce and key suggestions. [DONE]
	// user inputs desired nonce and key. [DONE]
	// input is verifyied as valid. [DONE]
	// calls if(cipher.buildEncryption(uint_32& or *?, uint_32& or *));

	char suggestedKey[KEY_BYTES];
	char suggestedFixedNonce[FIXED_NONCE_BYTES];
	unsigned short inputLength = 0;

	Serial.println("Setting up encryption");

	const unsigned long entropyTimeVal = micros();
	analogReadResolution(ANALOG_RESOLUTION);
	const unsigned long entropyPinVal = analogRead(ANALOG_PIN_NUM);
	const unsigned long entropyVal = entropyTimeVal ^ entropyPinVal;
	srand(entropyVal);													// This could stand to be improved.
	analogReadResolution(DEFAULT_ANALOG_RESOLUTION);

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		suggestedKey[i] = rand() % MAX_BYTE;
	}

	Serial.println("Please input the shared, 32-Byte, hex private key in the form of the suggested key given below.");
	Serial.println("This must be the same for both users! \n");
	Serial.print("Suggested random key:");

	printHex(suggestedKey, KEY_BYTES);

	char userKeyDec[MAX_USER_KEY_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userKeyDec, MAX_USER_KEY_LENGTH + 1);
			break;
		}

		delay(250);
	}

	if(inputLength != (MAX_USER_KEY_LENGTH)) {
		// Log an error here?
		Serial.println("ERROR: inputLength != MAX_USER_KEY_LENGTH");
		return false;
	}

	for(unsigned short i = 0; i < inputLength; i += 1) {
		if(!(48 <= userKeyDec[i] && userKeyDec[i] <= 57)) {
			if(!(65 <= userKeyDec[i] && userKeyDec[i] <= 70)) {
				if(!(97 <= userKeyDec[i] && userKeyDec[i] <= 102)) {
					// Log an error here?
					Serial.print("ERROR: invalid userKeyDec input: ");
					Serial.println(userKeyDec[i]);
					return false;
				}
			}
		}
	}

	char userKeyHex[KEY_BYTES];
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		if(48 <= userKeyDec[i*2] && userKeyDec[i*2] <= 57) {
			userKeyDec[i*2] -= 48;
		} else if(65 <= userKeyDec[i*2] && userKeyDec[i*2] <= 70) {
			userKeyDec[i*2] -= 55;
		} else {
			userKeyDec[i*2] -= 87;
		}
		userKeyHex[i] = userKeyDec[i*2];
		userKeyHex[i] <<= 4;

		if(48 <= userKeyDec[(i*2) + 1] && userKeyDec[(i*2) + 1] <= 57) {
			userKeyDec[(i*2) + 1] -= 48;
		} else if(65 <= userKeyDec[(i*2) + 1] && userKeyDec[(i*2) + 1] <= 70) {
			userKeyDec[(i*2) + 1] -= 55;
		} else {
			userKeyDec[(i*2) + 1] -= 87;
		}
		userKeyHex[i] |= userKeyDec[(i*2) + 1];
	}

	for(unsigned short i = 0; i < FIXED_NONCE_BYTES; i += 1){
		suggestedFixedNonce[i] = rand() % MAX_BYTE;
	}

	Serial.println("Please input your unique, 4-Byte, hex nonce in the form of the suggested nonce given below.");
	Serial.println("This must be different for both users! \n");
	Serial.print("Suggested random nonce:");

	printHex(suggestedFixedNonce, FIXED_NONCE_BYTES);

	char userFixedNonceDec[MAX_USER_FIXED_NONCE_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userFixedNonceDec, MAX_USER_FIXED_NONCE_LENGTH + 1);
			break;
		}

		delay(250);
	}

	if(inputLength != (MAX_USER_FIXED_NONCE_LENGTH)) {
		// Log an error here?
		Serial.println("ERROR: inputLength != MAX_USER_FIXED_NONCE_LENGTH");
		return false;
	}

	for(unsigned short i = 0; i < inputLength; i += 1) {
		if(!(48 <= userFixedNonceDec[i] && userFixedNonceDec[i] <= 57)) {
			if(!(65 <= userFixedNonceDec[i] && userFixedNonceDec[i] <= 70)) {
				if(!(97 <= userFixedNonceDec[i] && userFixedNonceDec[i] <= 102)) {
					// Log an error here?
					Serial.print("ERROR: invalid userFixedNonceDec input: ");
					Serial.println(userFixedNonceDec[i]);
					return false;
				}
			}
		}
	}

	char userFixedNonceHex[FIXED_NONCE_BYTES];
	for(unsigned short i = 0; i < FIXED_NONCE_BYTES; i += 1) {
		if(48 <= userFixedNonceDec[i*2] && userFixedNonceDec[i*2] <= 57) {
			userFixedNonceDec[i*2] -= 48;
		} else if(65 <= userFixedNonceDec[i*2] && userFixedNonceDec[i*2] <= 70) {
			userFixedNonceDec[i*2] -= 55;
		} else {
			userFixedNonceDec[i*2] -= 87;
		}
		userFixedNonceHex[i] = userFixedNonceDec[i*2];
		userFixedNonceHex[i] <<= 4;

		if(48 <= userFixedNonceDec[(i*2) + 1] && userFixedNonceDec[(i*2) + 1] <= 57) {
			userFixedNonceDec[(i*2) + 1] -= 48;
		} else if(65 <= userFixedNonceDec[(i*2) + 1] && userFixedNonceDec[(i*2) + 1] <= 70) {
			userFixedNonceDec[(i*2) + 1] -= 55;
		} else {
			userFixedNonceDec[(i*2) + 1] -= 87;
		}
		userFixedNonceHex[i] |= userFixedNonceDec[(i*2) + 1];
	}

	Serial.println("Please input your glEEpal's unique, 4-Byte, hex nonce in the form of the suggested nonce given above.");
	Serial.println("This cannot be the same as your nonce! \n");

	char peerFixedNonceDec[MAX_USER_FIXED_NONCE_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', peerFixedNonceDec, MAX_USER_FIXED_NONCE_LENGTH + 1);
			break;
		}

		delay(250);
	}

	if(inputLength != (MAX_USER_FIXED_NONCE_LENGTH)) {
		// Log an error here?
		Serial.println("ERROR: inputLength != MAX_PEER_FIXED_NONCE_LENGTH");
		return false;
	}

	for(unsigned short i = 0; i < inputLength; i += 1) {
		if(!(48 <= peerFixedNonceDec[i] && peerFixedNonceDec[i] <= 57)) {
			if(!(65 <= peerFixedNonceDec[i] && peerFixedNonceDec[i] <= 70)) {
				if(!(97 <= peerFixedNonceDec[i] && peerFixedNonceDec[i] <= 102)) {
					// Log an error here?
					Serial.print("ERROR: invalid peerFixedNonceDec input: ");
					Serial.println(peerFixedNonceDec[i]);
					return false;
				}
			}
		}
	}

	for(unsigned short i = 0; i < inputLength; i += 1) {
		if(peerFixedNonceDec[i] != userFixedNonceDec[i]) {
			break;
		}

		if(i == (inputLength - 1)) {
			// Log an error here?
			Serial.println("ERROR: user & peer nonces are equal");
			return false;
		}
	}

	char peerFixedNonceHex[FIXED_NONCE_BYTES];
	for(unsigned short i = 0; i < FIXED_NONCE_BYTES; i += 1) {
		if(48 <= peerFixedNonceDec[i*2] && peerFixedNonceDec[i*2] <= 57) {
			peerFixedNonceDec[i*2] -= 48;
		} else if(65 <= peerFixedNonceDec[i*2] && peerFixedNonceDec[i*2] <= 70) {
			peerFixedNonceDec[i*2] -= 55;
		} else {
			peerFixedNonceDec[i*2] -= 87;
		}
		peerFixedNonceHex[i] = peerFixedNonceDec[i*2];
		peerFixedNonceHex[i] <<= 4;

		if(48 <= peerFixedNonceDec[(i*2) + 1] && peerFixedNonceDec[(i*2) + 1] <= 57) {
			peerFixedNonceDec[(i*2) + 1] -= 48;
		} else if(65 <= peerFixedNonceDec[(i*2) + 1] && peerFixedNonceDec[(i*2) + 1] <= 70) {
			peerFixedNonceDec[(i*2) + 1] -= 55;
		} else {
			peerFixedNonceDec[(i*2) + 1] -= 87;
		}
		peerFixedNonceHex[i] |= peerFixedNonceDec[(i*2) + 1];
	}

	if(cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex)) {
		return true;
	}
}


void printEndState(uint32_t* endState) {
	Serial.print("endState: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(endState[i], HEX);
	}

	Serial.println('\n');
}


void printKeyStream(char* keyStream) {
	Serial.print("keyStream: ");
	for(unsigned short i = 0; i < 64; i += 1) {
		Serial.print(keyStream[i], HEX);
	}

	Serial.println('\n');
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	if(setupEncryption()) {
		Serial.println("Encryption successfully set up! \n");
		cipher.encryptMessage();
		printEndState(cipher.getEndState());
		printKeyStream(cipher.getKeyStream());
	}

	Serial.println("Done \n");
}

void loop() {}