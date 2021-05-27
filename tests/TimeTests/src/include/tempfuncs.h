#ifndef TEMPFUNCS_H
#define TEMPFUNCS_H

//#include "Arduino.h"
//#include "HardwareSerial.h"


static const unsigned short BAUD_RATE = 9600;

static const unsigned short KEY_BYTES = 32;
//static const unsigned short MAX_USER_KEY_BYTES = (KEY_BYTES*2);
static const unsigned short FIXED_NONCE_BYTES = 4;
//static const unsigned short MAX_USER_FIXED_NONCE_BYTES = (FIXED_NONCE_BYTES*2);

static char userKeyHex[KEY_BYTES] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01};
static char userFixedNonceHex[FIXED_NONCE_BYTES] = {0x00, 0x00, 0x00, 0x00};
static char peerFixedNonceHex[FIXED_NONCE_BYTES] = {0x00, 0x00, 0x00, 0x00}; // This would normally need to be a different value than userFixedNonceHex!

/*
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
*/

bool setupEncryption() {
/*	Serial.println("Setting up encryption\n");

	const unsigned short ANALOG_RESOLUTION = 32;
	const unsigned short ANALOG_PIN_NUM = A0;
	const unsigned short DEFAULT_ANALOG_RESOLUTION = 10;
	const unsigned short MAX_BYTE = 0x100;

	char suggestedKey[KEY_BYTES];
	char userKeyDec[MAX_USER_KEY_BYTES];
	char suggestedFixedNonce[FIXED_NONCE_BYTES];
	char userFixedNonceDec[MAX_USER_FIXED_NONCE_BYTES];
	char peerFixedNonceDec[MAX_USER_FIXED_NONCE_BYTES];
	unsigned short inputLength = 0;

// Initialize random number generation for suggested key and fixed nonce:
	const unsigned long entropyTimeVal = micros();
	analogReadResolution(ANALOG_RESOLUTION);
	const unsigned long entropyPinVal = analogRead(ANALOG_PIN_NUM);
	analogReadResolution(DEFAULT_ANALOG_RESOLUTION);
	const unsigned long entropyVal = entropyTimeVal ^ entropyPinVal;
	srand(entropyVal); // This could stand to be improved.

// Generate random suggested key:
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		suggestedKey[i] = rand() % MAX_BYTE;
	}

// Print instructions and generated random suggested key in HEX:
	Serial.println("Please input the shared, 32-Byte, hex private key in the form of the suggested key given below.");
	Serial.println("This must be the same for both users!\n");
	Serial.print("Suggested random key:");
	printHex(suggestedKey, KEY_BYTES);

// Read in user's key:
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userKeyDec, MAX_USER_KEY_BYTES + 1);
			break;
		}

		delay(250);
	}

// Check that user's key is correct length:
	if(inputLength != (MAX_USER_KEY_BYTES)) {
		// Log an error here?
		Serial.println("ERROR: inputLength != MAX_USER_KEY_BYTES");
		return false;
	}

// Check that user's key is correct format:
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

// Convert user's key to equivalent HEX values and concatenate into bytes:
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

// Generate random suggested fixed nonce:
	for(unsigned short i = 0; i < FIXED_NONCE_BYTES; i += 1){
		suggestedFixedNonce[i] = rand() % MAX_BYTE;
	}

// Print instructions and generated random suggested fixed nonce in HEX:
	Serial.println("Please input your unique, 4-Byte, hex nonce in the form of the suggested nonce given below.");
	Serial.println("This must be different for both users!\n");
	Serial.print("Suggested random nonce:");
	printHex(suggestedFixedNonce, FIXED_NONCE_BYTES);

// Read in user's fixed nonce:
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userFixedNonceDec, MAX_USER_FIXED_NONCE_BYTES + 1);
			break;
		}

		delay(250);
	}

// Check that user's fixed nonce is correct length:
	if(inputLength != (MAX_USER_FIXED_NONCE_BYTES)) {
		// Log an error here?
		Serial.println("ERROR: inputLength != MAX_USER_FIXED_NONCE_BYTES");
		return false;
	}

// Check that user's fixed nonce is correct format:
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

// Convert user's fixed nonce to equivalent HEX values and concatenate into bytes:
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

// Print instructions:
	Serial.println("Please input your glEEpal's unique, 4-Byte, hex nonce in the form of the suggested nonce given above.");
	Serial.println("This cannot be the same as your nonce!\n");

// Read in peer's fixed nonce:
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', peerFixedNonceDec, MAX_USER_FIXED_NONCE_BYTES + 1);
			break;
		}

		delay(250);
	}

// Check that peer's fixed nonce is correct length:
	if(inputLength != (MAX_USER_FIXED_NONCE_BYTES)) {
		// Log an error here?
		Serial.println("ERROR: inputLength != MAX_PEER_FIXED_NONCE_LENGTH");
		return false;
	}

// Check that peer's fixed nonce is correct format:
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

// Check that peer and user fixed nonces are different:
	for(unsigned short i = 0; i < inputLength; i += 1) {
		if(peerFixedNonceDec[i] != userFixedNonceDec[i]) {
			break;
		} else if(i == (inputLength - 1)) {
			// Log an error here?
			Serial.println("ERROR: user & peer nonces are equal");
			return false;
		}
	}

// Convert peer's fixed nonce to equivalent HEX values and concatenate into bytes:
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

	Serial.println("Encryption successfully set up!");
*/
	return true;
}


void printLastEndState(unsigned long* endState) {
	Serial.print("endState: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(endState[i], HEX);
	}

	Serial.println('\n');
}


void printLastKeyStream(char* keyStream) {
	Serial.print("keyStream: ");
	for(unsigned short i = 0; i < 64; i += 1) {
		Serial.print(keyStream[i], HEX);
	}

	Serial.println('\n');
}


void printMessage(char* message, unsigned int MESSAGE_BYTES) {
	Serial.print("message: ");
	for(unsigned short i = 0; i < MESSAGE_BYTES; i += 1) {
		Serial.print(message[i], HEX);
	}

	Serial.print('\n');
}

#endif