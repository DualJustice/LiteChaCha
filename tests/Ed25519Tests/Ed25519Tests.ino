#include "Arduino.h"
#include "HardwareSerial.h"

//#include "src/include/keyinfrastructure.h"
//#include "src/include/authenticatedencrypt.h"
#include "src/include/Ed25519.h"
//#include "src/include/SHA512.h"


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
*/
// Ed25519 TEST: --------------------------------------------------------------------------------------

	Ed25519SignatureAlgorithm dsa;

	char privateKey[32] = {0x9d, 0x61, 0xb1, 0x9d, 0xef, 0xfd, 0x5a, 0x60, 0xba, 0x84, 0x4a, 0xf4, 0x92, 0xec, 0x2c, 0xc4, 0x44, 0x49, 0xc5, 0x69, 0x7b, 0x32, 0x69, 0x19, 0x70, 0x3b, 0xac, 0x03, 0x1c, 0xae, 0x7f, 0x60};
	char publicKey[32];

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