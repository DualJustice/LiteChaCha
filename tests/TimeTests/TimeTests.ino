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

	for(unsigned short i = 0; i < 2; i += 1) {
		cipher.incrementNonceCounter();
	}

	unsigned long initialBlock = 1;

	// Input message and the number of bytes in message:
	unsigned long long MESSAGE_BYTES = 375;
	char message[MESSAGE_BYTES] = {"Any submission to the IETF intended by the Contributor for publication as all or part of an IETF Internet-Draft or RFC and any statement made within the context of an IETF activity is considered an \"IETF Contribution\". Such statements include oral statements in IETF sessions, as well as written and electronic communications made at any time or place, which are addressed to"};

	unsigned long long MESSAGE_2_BYTES = 4096;
	char message2[MESSAGE_2_BYTES];
	for(unsigned short i = 0; i < MESSAGE_2_BYTES; i += 1) {
		message2[i] = {'z'};
	}

	unsigned long long messageCount = 0;

	unsigned long timeStamp = 0;
	unsigned long duration = 0;

	unsigned long totalEncryptionTime = 0;
	unsigned long totalDecryptionTime = 0;

	if(setupEncryption()) { // Preferably only run once per session.
		Serial.print("Message bytes: ");
		Serial.println(MESSAGE_BYTES);

		timeStamp = micros();
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().
		duration = micros() - timeStamp;
		Serial.print("buildEncryption time approx. = ");
		Serial.print(duration);
		Serial.println(" us");

		for(unsigned short i = 0; i < 5; i += 1) {
			Serial.print("Test: ");
			Serial.println(i);

			timeStamp = micros();
			messageCount = cipher.getNonceCounter();
			cipher.encryptMessage(message, MESSAGE_BYTES, initialBlock);
			duration = micros() - timeStamp;
			totalEncryptionTime += duration;
			Serial.print("encryptMessage time approx. = ");
			Serial.print(duration);
			Serial.println(" us");

			Serial.print("Encrypted ");
			printMessage(message, MESSAGE_BYTES);

			for(unsigned short i = 0; i < 50; i += 1) { // Done to simulate something of a worst-case scenario.
				cipher.incrementNonceCounter();
			}

			timeStamp = micros();
			cipher.decryptMessage(message, MESSAGE_BYTES, messageCount, initialBlock);
			duration = micros() - timeStamp;
			totalDecryptionTime += duration;
			Serial.print("decryptMessage time approx. = ");
			Serial.print(duration);
			Serial.println(" us");

			Serial.print("Decrypted ");
			printMessage(message, MESSAGE_BYTES);
		}

		Serial.print("totalEncryptionTime: ");
		Serial.println(totalEncryptionTime);
		Serial.print("totalDecryptionTime: ");
		Serial.println(totalDecryptionTime);
		Serial.println("averageEncryptionTime: ");
		Serial.println("averageDecryptionTime: ");
		Serial.print('\n');

		totalEncryptionTime = 0;
		totalDecryptionTime = 0;

		Serial.print("Message bytes: ");
		Serial.println(MESSAGE_2_BYTES);

		for(unsigned short i = 0; i < 5; i += 1) {
			Serial.print("Test: ");
			Serial.println(i);

			timeStamp = micros();
			messageCount = cipher.getNonceCounter();
			cipher.encryptMessage(message2, MESSAGE_2_BYTES);
			duration = micros() - timeStamp;
			totalEncryptionTime += duration;
			Serial.print("encryptMessage time approx. = ");
			Serial.print(duration);
			Serial.println(" us");

			for(unsigned short i = 0; i < 50; i += 1) { // Done to simulate something of a worst-case scenario.
				cipher.incrementNonceCounter();
			}

			timeStamp = micros();
			cipher.decryptMessage(message2, MESSAGE_2_BYTES, messageCount);
			duration = micros() - timeStamp;
			totalDecryptionTime += duration;
			Serial.print("decryptMessage time approx. = ");
			Serial.print(duration);
			Serial.println(" us");
		}

		Serial.print("totalEncryptionTime: ");
		Serial.println(totalEncryptionTime);
		Serial.print("totalDecryptionTime: ");
		Serial.println(totalDecryptionTime);
		Serial.println("averageEncryptionTime: ");
		Serial.println("averageDecryptionTime: ");
	}
}


void loop() {}