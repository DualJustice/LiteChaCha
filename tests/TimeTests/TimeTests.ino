#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


static ChaChaEncryption& cipher = *new ChaChaEncryption();


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	// Input message and the number of bytes in message:
	unsigned long long MESSAGE_BYTES = 256;
	char message[MESSAGE_BYTES] = {"42"};
	unsigned long startBlock = 0;

	Serial.print("Message bytes: ");
	Serial.print(MESSAGE_BYTES);
	Serial.println('\n');

	for(unsigned short i = 0; i < MESSAGE_BYTES; i += 1) {
		message[i] = 'v';
	}

	unsigned long timeStamp = 0;
	unsigned long duration = 0;

	if(setupEncryption()) { // Preferably only run once per session.
		timeStamp = millis();
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().
		duration = millis() - timeStamp;
		Serial.print("buildEncryption time approx. = ");
		Serial.print(duration);
		Serial.println(" ms\n");

		timeStamp = millis();
		cipher.encryptMessage(message, MESSAGE_BYTES);
		duration = millis() - timeStamp;
		Serial.print("encryptMessage time approx. = ");
		Serial.print(duration);
		Serial.println(" ms\n");

		//printEncryptedMessage(message, MESSAGE_BYTES);
	}
}


void loop() {}