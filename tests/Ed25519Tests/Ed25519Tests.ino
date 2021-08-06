#include "Arduino.h"
#include "HardwareSerial.h"

//#include "src/include/keyinfrastructure.h"
//#include "src/include/authenticatedencrypt.h"
//#include "src/include/Ed25519.h"
#include "src/include/SHA512.h"


void setup() {

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	unsigned long timeStamp = 0;
	unsigned long duration = 0;


// SHA512 TEST: ---------------------------------------------------------------------------------------

	SHA512Hash hash;

	char cornedBeef[64];
	unsigned long long messageLength = 12;
	char message[messageLength] = {"Hello world!"};

	hash.hashBytes(cornedBeef, message, messageLength);

	for(unsigned short i = 0; i < 64; i += 1) {
		Serial.print(cornedBeef[i], HEX);
	}
	Serial.println();

// Ed25519 TEST: --------------------------------------------------------------------------------------

//	Ed25519SignatureAlgorithm dsa;

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