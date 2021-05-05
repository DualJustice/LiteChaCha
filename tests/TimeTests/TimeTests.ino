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
	unsigned long long MESSAGE_BYTES = 4096;
	char message[MESSAGE_BYTES];
	for(unsigned short i = 0; i < MESSAGE_BYTES; i += 1) {
		message[i] = 'a';
	}

	unsigned long long messageCount = 0;

	unsigned long timeStamp = 0;
	unsigned long duration = 0;

	if(setupEncryption()) { // Preferably only run once per session.
		Serial.print("Message bytes: ");
		Serial.println(MESSAGE_BYTES);

		timeStamp = micros();
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().
		duration = micros() - timeStamp;
		Serial.print("buildEncryption time approx. = ");
		Serial.print(duration);
		Serial.println(" us");

		timeStamp = micros();
		messageCount = cipher.getNonceCounter();
		cipher.encryptMessage(message, MESSAGE_BYTES);
		duration = micros() - timeStamp;
		Serial.print("encryptMessage time approx. = ");
		Serial.print(duration);
		Serial.println(" us");

		for(unsigned short i = 0; i < 50; i += 1) { // Done to simulate something of a worst-case scenario.
			cipher.incrementNonceCounter();
		}

		timeStamp = micros();
		cipher.decryptMessage(message, MESSAGE_BYTES, messageCount);
		duration = micros() - timeStamp;
		Serial.print("decryptMessage time approx. = ");
		Serial.print(duration);
		Serial.println(" us");
	}
}


void loop() {}