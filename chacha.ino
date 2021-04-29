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
	srand(entropyVal);
	analogReadResolution(DEFAULT_ANALOG_RESOLUTION);

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		suggestedKey[i] = rand() % MAX_BYTE;
	}

	Serial.println("Please input the shared, 32-Byte, hex private key in the form of the suggested key given below.");
	Serial.println("This must be the same for both users! \n");
	Serial.print("Suggested random key:");

	printHex(suggestedKey, KEY_BYTES);

	char userKey[MAX_USER_KEY_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userKey, MAX_USER_KEY_LENGTH + 1);
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
		if(!(48 <= userKey[i] && userKey[i] <= 57)) {
			if(!(65 <= userKey[i] && userKey[i] <= 70)) {
				if(!(97 <= userKey[i] && userKey[i] <= 102)) {
					// Log an error here?
					Serial.print("ERROR: invalid userKey input: ");
					Serial.println(userKey[i]);
					return false;
				}
			}
		}
	}

	for(unsigned short i = 0; i < FIXED_NONCE_BYTES; i += 1){
		suggestedFixedNonce[i] = rand() % MAX_BYTE;
	}

	Serial.println("Please input your unique, 4-Byte, hex nonce in the form of the suggested nonce given below.");
	Serial.println("This must be different for both users! \n");
	Serial.print("Suggested random nonce:");

	printHex(suggestedFixedNonce, FIXED_NONCE_BYTES);

	char userFixedNonce[MAX_USER_FIXED_NONCE_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userFixedNonce, MAX_USER_FIXED_NONCE_LENGTH + 1);
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
		if(!(48 <= userFixedNonce[i] && userFixedNonce[i] <= 57)) {
			if(!(65 <= userFixedNonce[i] && userFixedNonce[i] <= 70)) {
				if(!(97 <= userFixedNonce[i] && userFixedNonce[i] <= 102)) {
					// Log an error here?
					Serial.print("ERROR: invalid userFixedNonce input: ");
					Serial.println(userFixedNonce[i]);
					return false;
				}
			}
		}
	}

	Serial.println("Please input your glEEpal's unique, 4-Byte, hex nonce in the form of the suggested nonce given above.");
	Serial.println("This cannot be the same as your nonce! \n");

	char peerFixedNonce[MAX_USER_FIXED_NONCE_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', peerFixedNonce, MAX_USER_FIXED_NONCE_LENGTH + 1);
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
		if(!(48 <= peerFixedNonce[i] && peerFixedNonce[i] <= 57)) {
			if(!(65 <= peerFixedNonce[i] && peerFixedNonce[i] <= 70)) {
				if(!(97 <= peerFixedNonce[i] && peerFixedNonce[i] <= 102)) {
					// Log an error here?
					Serial.print("ERROR: invalid userFixedNonce input: ");
					Serial.println(peerFixedNonce[i]);
					return false;
				}
			}
		}
	}

	for(unsigned short i = 0; i < inputLength; i += 1) {
		if(peerFixedNonce[i] != userFixedNonce[i]) {
			break;
		}

		if(i == (inputLength - 1)) {
			// Log an error here?
			Serial.println("ERROR: user & peer nonces are equal");
			return false;
		}
	}

	if(cipher.buildEncryption(userKey, userFixedNonce, peerFixedNonce)) {
		return true;
	}
}


void printEndState(uint32_t* endState) {
	Serial.print("endState: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(endState[i], HEX);
	}

	Serial.println();
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	if(setupEncryption()) {
		Serial.println("Encryption successfully set up!");
		cipher.encryptMessage();
		printEndState(cipher.getEndState());
	}
}

void loop() {}