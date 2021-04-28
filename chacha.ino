#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/chacha.h"


static const unsigned short BAUD_RATE = 9600;
static ChaChaEncryption& cipher = *new ChaChaEncryption();


void printHex(char* data, const unsigned short length) { // Boy do I sure love Serial.print!
	for(unsigned short i = 0; i < length; i += 1) {
		if(data[i] > 0x0f) {
			Serial.print(data[i], HEX);
			Serial.print(" ");
		} else {
			Serial.print('0');
			Serial.print(data[i], HEX);
			Serial.print(" ");
		}
	}

	if(data[length] > 0x0f) {
		Serial.print(data[length], HEX);
		Serial.println('\n');
	} else {
		Serial.print('0');
		Serial.print(data[length], HEX);
		Serial.println('\n');
	}
}


bool setupEncryption() {
	// print some useful messages. [DONE]
	// generate random nonce and key suggestions. [DONE]
	// user inputs desired nonce and key. [DONE]
	// input is verifyied as valid.
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

	Serial.println("Please input the shared, 32-Byte private key in the form of the suggested key given below.");
	Serial.println("This must be the same for both users! \n");
	Serial.print("Suggested random key:");

	printHex(suggestedKey, (KEY_BYTES - 1));

	char userKey[MAX_USER_KEY_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userKey, MAX_USER_KEY_LENGTH);
			break;
		}

		delay(250);
	}

	if(inputLength != (MAX_USER_KEY_LENGTH - 1)) {
		// Log an error here.
		Serial.println("ERROR: inputLength != MAX_USER_KEY_LENGTH");
		return false;
	}
	userKey[inputLength] = '\0';

	for(unsigned short i = 0; i < FIXED_NONCE_BYTES; i += 1){
		suggestedFixedNonce[i] = rand() % MAX_BYTE;
	}

	Serial.println("Please input your unique, 4-Byte nonce in the form of the suggested nonce given below.");
	Serial.println("This must be the different for both users! \n");
	Serial.print("Suggested random nonce:");

	printHex(suggestedFixedNonce, (FIXED_NONCE_BYTES - 1));

	char userFixedNonce[MAX_USER_FIXED_NONCE_LENGTH];
	while(true) {
		if(Serial.available() > 0) {
			inputLength = Serial.readBytesUntil('\n', userFixedNonce, MAX_USER_FIXED_NONCE_LENGTH);
			break;
		}

		delay(250);
	}

	if(inputLength != (MAX_USER_FIXED_NONCE_LENGTH - 1)) {
		// Log an error here.
		Serial.println("ERROR: inputLength != MAX_USER_FIXED_NONCE_LENGTH");
		return false;
	}
	userFixedNonce[inputLength] = '\0';

	if(cipher.buildEncryption(userKey, userFixedNonce)) {
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