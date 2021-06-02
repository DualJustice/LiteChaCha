#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/tempfuncs.h"
#include "src/include/X25519.h"
#include "src/include/chacha.h"


static ChaChaEncryption& cipher = *new ChaChaEncryption();
static X25519KeyManagement& ecdhe = *new X25519KeyManagement();


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

/*
//CHACHA TEST:

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
*/

// X25519 TEST:

	static char privateKey[32] = {0xa5, 0x46, 0xe3, 0x6b, 0xf0, 0x52, 0x7c, 0x9d, 0x3b, 0x16, 0x15, 0x4b, 0x82, 0x46, 0x5e, 0xdd, 0x62, 0x14, 0x4c, 0x0a, 0xc1, 0xfc, 0x5a, 0x18, 0x50, 0x6a, 0x22, 0x44, 0xba, 0x44, 0x9a, 0xc4};

	ecdhe.startBigNum();

	ecdhe.createPubKey(privateKey);
}


void loop() {}