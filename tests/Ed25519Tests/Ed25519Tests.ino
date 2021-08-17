#include "Arduino.h"
#include "HardwareSerial.h"

//#include "src/include/keyinfrastructure.h"
//#include "src/include/authenticatedencrypt.h"
#include "src/include/Ed25519.h"
//#include "src/include/SHA512.h"


void stringToHex(char* out, char* s) {
	for(unsigned short i = 0; i < 32; i += 1) {
		if(48 <= s[i*2] && s[i*2] <= 57) {
			s[i*2] -= 48;
		} else if(65 <= s[i*2] && s[i*2] <= 70) {
			s[i*2] -= 55;
		} else {
			s[i*2] -= 87;
		}
		out[i] = s[i*2];
		out[i] <<= 4;

		if(48 <= s[(i*2) + 1] && s[(i*2) + 1] <= 57) {
			s[(i*2) + 1] -= 48;
		} else if(65 <= s[(i*2) + 1] && s[(i*2) + 1] <= 70) {
			s[(i*2) + 1] -= 55;
		} else {
			s[(i*2) + 1] -= 87;
		}
		out[i] |= s[(i*2) + 1];
	}
}


void setup() {

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	unsigned long timestamp = 0;
	unsigned long duration = 0;

/*
// SHA512 TEST: ---------------------------------------------------------------------------------------

	SHA512Hash hash;

	char cornedBeef[64];

	const size_t messageLength = 3;
	char messageBuffer[messageLength + 1] = {"abc"};
	char message[messageLength];
	for(unsigned long long i = 0; i < messageLength; i += 1) {
		message[i] = messageBuffer[i];
	}
	hash.hashBytes(cornedBeef, message, messageLength);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength1 = 0;
	char messageBuffer1[messageLength1 + 1] = {""};
	char message1[messageLength1];
	for(unsigned long long i = 0; i < messageLength1; i += 1) {
		message1[i] = messageBuffer1[i];
	}
	hash.hashBytes(cornedBeef, message1, messageLength1);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength2 = 56;
	char messageBuffer2[messageLength2 + 1] = {"abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq"};
	char message2[messageLength2];
	for(unsigned long long i = 0; i < messageLength2; i += 1) {
		message2[i] = messageBuffer2[i];
	}
	hash.hashBytes(cornedBeef, message2, messageLength2);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength3 = 111;
	char messageBuffer3[messageLength3 + 1] = {"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrst"};
	char message3[messageLength3];
	for(unsigned long long i = 0; i < messageLength3; i += 1) {
		message3[i] = messageBuffer3[i];
	}
	hash.hashBytes(cornedBeef, message3, messageLength3);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength4 = 112;
	char messageBuffer4[messageLength4 + 1] = {"abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"};
	char message4[messageLength4];
	for(unsigned long long i = 0; i < messageLength4; i += 1) {
		message4[i] = messageBuffer4[i];
	}
	hash.hashBytes(cornedBeef, message4, messageLength4);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength5 = 240;
	char messageBuffer5[messageLength5 + 1] = {"apovabapouea198asa8d91f15efgpoasohidoiea89a9da8f1a32sdf4ado8ivha1a6s8e68868668asefadhajfshacasedlkjadADAIPHLKJAzzzzlakdf97165g00abcdefghbcdefghicdefghijdefghijkefghijklfghijklmghijklmnhijklmnoijklmnopjklmnopqklmnopqrlmnopqrsmnopqrstnopqrstu"};
	char message5[messageLength5];
	for(unsigned long long i = 0; i < messageLength5; i += 1) {
		message5[i] = messageBuffer5[i];
	}
	hash.hashBytes(cornedBeef, message5, messageLength5);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength6 = 32;
	char messageBuffer6[messageLength6 + 1] = {"a9b18f7aioushdskjbdnoapiu5427683"};
	char message6[messageLength6];
	for(unsigned long long i = 0; i < messageLength6; i += 1) {
		message6[i] = messageBuffer6[i];
	}
	hash.hashBytes(cornedBeef, message6, messageLength6);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength7 = 32;
	char messageBuffer7[messageLength7 + 1] = {0x01, 0x39, 0x62, 0x31, 0x38, 0x66, 0x37, 0x61, 0x69, 0x6f, 0x75, 0x73, 0x68, 0x64, 0x73, 0x6b, 0x6a, 0x62, 0x64, 0x6e, 0x6f, 0x61, 0x70, 0x69, 0x75, 0x35, 0x34, 0x32, 0x37, 0x36, 0x38, 0x33};
	char message7[messageLength7];
	for(unsigned long long i = 0; i < messageLength7; i += 1) {
		message7[i] = messageBuffer7[i];
	}
	hash.hashBytes(cornedBeef, message7, messageLength7);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');

	const size_t messageLength8 = 5;
	char messageBuffer8[messageLength8 + 1] = {0x9d, 0x61, 0xb1, 0x9d, 0xef};
	char message8[messageLength8];
	for(unsigned long long i = 0; i < messageLength8; i += 1) {
		message8[i] = messageBuffer8[i];
	}
	hash.hashBytes(cornedBeef, message8, messageLength8);
	for(unsigned short i = 0; i < 64; i += 1) {
		if(cornedBeef[i] > 0x0f) {
			Serial.print(cornedBeef[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(cornedBeef[i], HEX);
		}
	}
	Serial.println('\n');
*/

// Ed25519 TEST: --------------------------------------------------------------------------------------

	Ed25519SignatureAlgorithm dsa;

	char privateKeyBuffer[65] = {"833fe62409237b9d62ec77587520911e9a759cec1d19755b7da901b96dca3d42"};
	char privateKey[32];
	char publicKey[32];

	Serial.println("Running");

	stringToHex(privateKey, privateKeyBuffer);

	timestamp = millis();
	dsa.initialize(publicKey, privateKey);
	duration = millis() - timestamp;

	Serial.print("runtime: ");
	Serial.println(duration);

	for(unsigned short i = 0; i < 32; i += 1) {
		if(publicKey[i] > 0x0f) {
			Serial.print(publicKey[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(publicKey[i], HEX);
		}
	}
	Serial.println('\n');

/*
// PKI & AE TEST: -------------------------------------------------------------------------------------

	KeyManagement pki;
	CipherManagement ae;

// -------------------- Necessary Variables --------------------

	char userID[pki.getIDBytes()]; // IDs are used for a fixed portion of a nonce.
	char peerID[pki.getIDBytes()];
	char userPubKey[pki.getKeyBytes()];
	char peerPubKey[pki.getKeyBytes()];

	char sessionID[pki.getKeyBytes()];
	char peerSessionID[pki.getKeyBytes()];

	unsigned long long messageCount; // Used to increment a nonce for each new message sent. Will be sent with each encrypted message.

	char tag[ae.getTagBytes()]; // Used to authenticate encrypted messages. Will be sent with each encrypted message.

// -------------------- Establish Connection --------------------

	pki.initialize(userID, userPubKey); // Generates a random user ID and public key.

//	Exchange IDs and public keys unencrypted.

	if(pki.IDUnique(userID, peerID)) {
		pki.createSessionKey(peerPubKey); // Creates a shared private session key, overwriting peerPubKey, if both users have different IDs.
	}

//	Ensure that the shared private session key has never been used by you before!

	ae.initialize(peerPubKey, userID, peerID);

//	To confirm a secure connection, compare session IDs out-of-band. If they match, the session is secure. This check is only useful if the out-of-band communication method
//	chosen is through a connection that is already known to be secure! sessionIDs are compared, not shared private session keys, as a security measure to keep the key secret.
//	This check could be done in the future using RSA or ECDSA:
	ae.getSessionID(sessionID); // Creates a Session ID derived from the shared private session key. This is to be sent to the other user.
	if(ae.sameSessionID(peerSessionID)) {
		// The connection is secure, assuming sessionIDs were exchanged over an alternative, already-secure connection...
	}

// -------------------- Encrypt and Decrypt --------------------

	// Input message and the number of bytes in message:
	unsigned long long messageBytes = 0;
	char message[messageBytes] = {""};

	ae.encryptAndTagMessage(messageCount, tag, message, messageBytes); // Encrypts message, overwriting it with the ciphertext. Outputs messageCount and the MAC tag as well.

//	Send the messageCount, tag, and message.

	// Upon receiving a ciphertext, messageCount, and tag:
	if(ae.messageAuthentic(message, messageBytes, messageCount, tag)) { // Authenticates the message with the MAC tag.
		ae.decryptAuthenticatedMessage(message, messageBytes, messageCount); // Decrypts message, overwriting it with the plaintext.
	}
*/
}


void loop() {}