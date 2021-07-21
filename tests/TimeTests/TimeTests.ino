#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/ciphersetup.h"
#include "src/include/X25519.h"
#include "src/include/AEAD.h"
//#include <stdint.h>
//#include "src/include/poly1305.h"
//#include "src/include/tempfuncs.h"
//#include "src/include/chacha.h"


/*
---------- Basic Order of Operations ----------

1. Generate two cryptographically random numbers (not pseudorandom). These will be your private session key and your fixed nonce.
	1.1. Exchange fixed nonces unencrypted.
	1.2. Ensure that your fixed nonce is different from that of the other user.
2. Create a public session key with your private session key using X25519.
3. Create a shared private session key with your private session key and the other users public session key using X25519.
	3.1. Ensure that your shared private session key & fixed nonce pair has not been used by you before. For better, though unnecessary security, simply ensuring that the shared private
			session key is new is more secure as it does not allow for private key swaps across two different connections, giving potential attackers less messages to work with using a
			single key.
	3.2. Confirm a secure connection by comparing shared private session key out-of-band. If they match, the session is secure. This could be done in the future using RSA or ECDSA.

---------- Communication ----------

4. Create a one-time Poly1305 key with the private session key, your nonce, and the block counter set to 0 using chacha.
5. Encrypt the message with the shared private session key, your nonce, and the block counter set to 1 using chacha.
6. Create a MAC of the encrypted message in AEAD format (the nonce counter will be additional data) with the shared private session key using poly1305.
	6.1. When a message is received, reproduce its MAC in the same way.
	6.2. If your MAC matches that of the one sent with the message it is safe to decrypt and parse.
*/


void setup() {
	CipherSuiteSetup init;
	X25519KeyManagement ecdhe;
	AEADConstruct authencrypt;
//	Poly1305MAC mac;
//	ChaChaEncryption cipher;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	unsigned long timeStamp = 0;
	unsigned long duration = 0;


// SETUP TEST: ------------------------------------------------------------------------------------

	init.generateRandomKey();

// AEAD TEST: -------------------------------------------------------------------------------------



/*
// X25519 TEST: -----------------------------------------------------------------------------------

//	static char privateKey[32] = {0xa5, 0x46, 0xe3, 0x6b, 0xf0, 0x52, 0x7c, 0x9d, 0x3b, 0x16, 0x15, 0x4b, 0x82, 0x46, 0x5e, 0xdd, 0x62, 0x14, 0x4c, 0x0a, 0xc1, 0xfc, 0x5a, 0x18, 0x50, 0x6a, 0x22, 0x44, 0xba, 0x44, 0x9a, 0xc4};
//	static char initXCoord[32] = {0xe6, 0xdb, 0x68, 0x67, 0x58, 0x30, 0x30, 0xdb, 0x35, 0x94, 0xc1, 0xa4, 0x24, 0xb1, 0x5f, 0x7c, 0x72, 0x66, 0x24, 0xec, 0x26, 0xb3, 0x35, 0x3b, 0x10, 0xa9, 0x03, 0xa6, 0xd0, 0xab, 0x1c, 0x4c};

//	static char privateKey[32] = {0x4b, 0x66, 0xe9, 0xd4, 0xd1, 0xb4, 0x67, 0x3c, 0x5a, 0xd2, 0x26, 0x91, 0x95, 0x7d, 0x6a, 0xf5, 0xc1, 0x1b, 0x64, 0x21, 0xe0, 0xea, 0x01, 0xd4, 0x2c, 0xa4, 0x16, 0x9e, 0x79, 0x18, 0xba, 0x0d};
//	static char initXCoord[32] = {0xe5, 0x21, 0x0f, 0x12, 0x78, 0x68, 0x11, 0xd3, 0xf4, 0xb7, 0x95, 0x9d, 0x05, 0x38, 0xae, 0x2c, 0x31, 0xdb, 0xe7, 0x10, 0x6f, 0xc0, 0x3c, 0x3e, 0xfc, 0x4c, 0xd5, 0x49, 0xc7, 0x15, 0xa4, 0x93};

//	static char privateKey[32] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
//	static char initXCoord[32] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//	static char privateKey[32] = {0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a};
//	static char initXCoord[32] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//	static char privateKey[32] = {0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b, 0x79, 0xe1, 0x7f, 0x8b, 0x83, 0x80, 0x0e, 0xe6, 0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18, 0xb6, 0xfd, 0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb};
//	static char initXCoord[32] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//	static char privateKey[32] = {0x5d, 0xab, 0x08, 0x7e, 0x62, 0x4a, 0x8a, 0x4b, 0x79, 0xe1, 0x7f, 0x8b, 0x83, 0x80, 0x0e, 0xe6, 0x6f, 0x3b, 0xb1, 0x29, 0x26, 0x18, 0xb6, 0xfd, 0x1c, 0x2f, 0x8b, 0x27, 0xff, 0x88, 0xe0, 0xeb};
//	static char initXCoord[32] = {0x85, 0x20, 0xf0, 0x09, 0x89, 0x30, 0xa7, 0x54, 0x74, 0x8b, 0x7d, 0xdc, 0xb4, 0x3e, 0xf7, 0x5a, 0x0d, 0xbf, 0x3a, 0x0d, 0x26, 0x38, 0x1a, 0xf4, 0xeb, 0xa4, 0xa9, 0x8e, 0xaa, 0x9b, 0x4e, 0x6a};

//	static char privateKey[32] = {0x77, 0x07, 0x6d, 0x0a, 0x73, 0x18, 0xa5, 0x7d, 0x3c, 0x16, 0xc1, 0x72, 0x51, 0xb2, 0x66, 0x45, 0xdf, 0x4c, 0x2f, 0x87, 0xeb, 0xc0, 0x99, 0x2a, 0xb1, 0x77, 0xfb, 0xa5, 0x1d, 0xb9, 0x2c, 0x2a};
//	static char initXCoord[32] = {0xde, 0x9e, 0xdb, 0x7d, 0x7b, 0x7d, 0xc1, 0xb4, 0xd3, 0x5b, 0x61, 0xc2, 0xec, 0xe4, 0x35, 0x37, 0x3f, 0x83, 0x43, 0xc8, 0x5b, 0x78, 0x67, 0x4d, 0xad, 0xfc, 0x7e, 0x14, 0x6f, 0x88, 0x2b, 0x4f};

//	static char privateKey[32] = {0x75, 0x1f, 0xb4, 0x30, 0x86, 0x55, 0xb4, 0x76, 0xb6, 0x78, 0x9b, 0x73, 0x25, 0xf9, 0xea, 0x8c, 0xdd, 0xd1, 0x6a, 0x58, 0x53, 0x3f, 0xf6, 0xd9, 0xe6, 0x00, 0x09, 0x46, 0x4a, 0x5f, 0x9d, 0x94};
//	static char initXCoord[32] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//	static char privateKey[32] = {0x0a, 0x54, 0x64, 0x52, 0x53, 0x29, 0x0d, 0x60, 0xdd, 0xad, 0xd0, 0xe0, 0x30, 0xba, 0xcd, 0x9e, 0x55, 0x01, 0xef, 0xdc, 0x22, 0x07, 0x55, 0xa1, 0xe9, 0x78, 0xf1, 0xb8, 0x39, 0xa0, 0x56, 0x88};
//	static char initXCoord[32] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

//	static char privateKey[32] = {0x0a, 0x54, 0x64, 0x52, 0x53, 0x29, 0x0d, 0x60, 0xdd, 0xad, 0xd0, 0xe0, 0x30, 0xba, 0xcd, 0x9e, 0x55, 0x01, 0xef, 0xdc, 0x22, 0x07, 0x55, 0xa1, 0xe9, 0x78, 0xf1, 0xb8, 0x39, 0xa0, 0x56, 0x88};
//	static char initXCoord[32] = {0x48, 0xd5, 0xdd, 0xd4, 0x06, 0x12, 0x57, 0xba, 0x16, 0x6f, 0xa3, 0xf9, 0xbb, 0xdb, 0x74, 0xf1, 0xa4, 0xe8, 0x1c, 0x08, 0x93, 0x84, 0xfa, 0x77, 0xf7, 0x90, 0x70, 0x9f, 0x0d, 0xfb, 0xc7, 0x66};

	static char privateKey[32] = {0x75, 0x1f, 0xb4, 0x30, 0x86, 0x55, 0xb4, 0x76, 0xb6, 0x78, 0x9b, 0x73, 0x25, 0xf9, 0xea, 0x8c, 0xdd, 0xd1, 0x6a, 0x58, 0x53, 0x3f, 0xf6, 0xd9, 0xe6, 0x00, 0x09, 0x46, 0x4a, 0x5f, 0x9d, 0x94};
	static char initXCoord[32] = {0x0b, 0xe7, 0xc1, 0xf5, 0xaa, 0xd8, 0x7d, 0x7e, 0x44, 0x86, 0x62, 0x67, 0x32, 0x98, 0xa4, 0x43, 0x47, 0x8b, 0x85, 0x97, 0x45, 0x17, 0x9e, 0xaf, 0x56, 0x4c, 0x79, 0xc0, 0xef, 0x6e, 0xee, 0x25};

	duration = 0;
	timeStamp = micros();
	ecdhe.curve25519(privateKey, initXCoord);
	duration = micros() - timeStamp;

	Serial.print("publicKey: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		if(initXCoord[i] > 0x0f) {
			Serial.print(initXCoord[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(initXCoord[i], HEX);
		}
	}
	Serial.println('\n');

	Serial.print("micros: ");
	Serial.print(duration);
	Serial.println('\n');
*/
/*
// POLY1305 TEST: ---------------------------------------------------------------------------------
	char tag[16];
	bool auth;

	uint32_t key[8] = {0x7bac2b25, 0x2db447af, 0x09b67a55, 0xa4e95584, 0x0ae1d673, 0x1075d9eb, 0x2a937578, 0x3ed553ff};
	unsigned long long messageLength = 160;
	char message[messageLength] = {0x50, 0x51, 0x52, 0x53, 0xc0, 0xc1, 0xc2, 0xc3, 0xc4, 0xc5, 0xc6, 0xc7, 0x00, 0x00, 0x00, 0x00, 0xd3, 0x1a, 0x8d, 0x34, 0x64, 0x8e, 0x60, 0xdb, 0x7b, 0x86, 0xaf, 0xbc, 0x53, 0xef, 0x7e, 0xc2, 0xa4, 0xad, 0xed, 0x51, 0x29, 0x6e, 0x08, 0xfe, 0xa9, 0xe2, 0xb5, 0xa7, 0x36, 0xee, 0x62, 0xd6, 0x3d, 0xbe, 0xa4, 0x5e, 0x8c, 0xa9, 0x67, 0x12, 0x82, 0xfa, 0xfb, 0x69, 0xda, 0x92, 0x72, 0x8b, 0x1a, 0x71, 0xde, 0x0a, 0x9e, 0x06, 0x0b, 0x29, 0x05, 0xd6, 0xa5, 0xb6, 0x7e, 0xcd, 0x3b, 0x36, 0x92, 0xdd, 0xbd, 0x7f, 0x2d, 0x77, 0x8b, 0x8c, 0x98, 0x03, 0xae, 0xe3, 0x28, 0x09, 0x1b, 0x58, 0xfa, 0xb3, 0x24, 0xe4, 0xfa, 0xd6, 0x75, 0x94, 0x55, 0x85, 0x80, 0x8b, 0x48, 0x31, 0xd7, 0xbc, 0x3f, 0xf4, 0xde, 0xf0, 0x8e, 0x4b, 0x7a, 0x9d, 0xe5, 0x76, 0xd2, 0x65, 0x86, 0xce, 0xc6, 0x4b, 0x61, 0x16, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x72, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	duration = 0;

	for(unsigned short i = 0; i < 5; i += 1) {
		timeStamp = micros();
		mac.createTag(tag, key, message, messageLength);
		duration += (micros() - timeStamp);
	}

	Serial.print("tag: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		if(tag[i] > 0x0f) {
			Serial.print(tag[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(tag[i], HEX);
		}
	}
	Serial.println('\n');

	Serial.print("5-run micros: ");
	Serial.print(duration);
	Serial.println('\n');

//						 0x1a, 0xe1, 0x0b, 0x59, 0x4f, 0x09, 0xe2, 0x6a, 0x7e, 0x90, 0x2e, 0xcb, 0xd0, 0x60, 0x06, 0x91
	char tagFalse[16] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	duration = 0;

	for(unsigned short i = 0; i < 5; i += 1) {
		timeStamp = micros();
		auth = mac.authenticateTag(tagFalse, key, message, messageLength);
		duration += (micros() - timeStamp);
	}

	Serial.print("auth? : ");
	Serial.print(auth);
	Serial.println('\n');

	Serial.print("5-run micros: ");
	Serial.print(duration);
	Serial.println('\n');
*/
/*
// CHACHA TEST (12 ms max per message, 256 bytes max expected message length): --------------------

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
}


void loop() {}