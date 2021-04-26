#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/chacha.h"


static const unsigned short BAUD_RATE = 9600;
static ChaChaEncryption& cipher = *new ChaChaEncryption();


bool setupEncryption() {
	// print some useful messages.
	// generate random nonce and key suggestions.
	// user inputs desired nonce and key.
	// calls if(cipher.buildEncryption(uint_32& or *?, uint_32& or *));

	const unsigned short MAX_BYTE = 256;
	const unsigned short SUGGESTED_NONCE_LENGTH = 4; // Bytes.
	const unsigned short SUGGESTED_KEY_LENGTH = 32; // Bytes.
	unsigned char suggestedNonce[SUGGESTED_NONCE_LENGTH];
	unsigned char suggestedKey[SUGGESTED_KEY_LENGTH];

	srand(micros()); // This could really stand to be improved.

	Serial.println("Setting up encryption");
	for(unsigned short i = 0; i < SUGGESTED_NONCE_LENGTH; i += 1){
		suggestedNonce[i] = rand() % MAX_BYTE;
	}

	for(unsigned short i = 0; i < SUGGESTED_KEY_LENGTH; i += 1) {
		suggestedKey[i] = rand() % MAX_BYTE;
	}

	Serial.println("Please input the shared, 32-Byte private key in the form of the suggested key given below.");
	Serial.println("This must be the same for both users! \n");
	Serial.print("Suggested random key:");
	for(unsigned short i = 0; i < SUGGESTED_KEY_LENGTH - 1; i += 1) {
		Serial.print(suggestedKey[i], HEX);
		Serial.print(" ");
	}

	Serial.print(suggestedKey[SUGGESTED_KEY_LENGTH - 1], HEX);
	Serial.println("\n");
// ==============================================================================================
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userSSID, internet.getMaxSSIDLength());
			break;
		}

		delay(250);
	}
// ==============================================================================================
	Serial.println("Please input your unique, 4-Byte nonce in the form of the suggested nonce given below.");
	Serial.println("This must be the different for both users! \n");
	Serial.print("Suggested random nonce:");
	for(unsigned short i = 0; i < SUGGESTED_NONCE_LENGTH - 1; i += 1) {
		Serial.print(suggestedNonce[i], HEX);
		Serial.print(" ");
	}

	Serial.print(suggestedNonce[SUGGESTED_NONCE_LENGTH - 1], HEX);
	Serial.println("\n");
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