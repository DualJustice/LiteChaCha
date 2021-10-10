#include "src/include/keyinfrastructure.h"
#include "src/include/authenticatedencrypt.h"


void setup() {
	KeyManagement pki;
	CipherManagement ae;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


// -------------------- Necessary Variables --------------------

	const size_t keyBytes = pki.getKeyBytes();
	const size_t signatureBytes = pki.getSignatureBytes();
	const size_t IDBytes = pki.getIDBytes();

	char userDSAPrivateKey[keyBytes]; // Used as either an input or an output depending on whether the user would like to generate a new key pair.
	char userDSAPubKey[keyBytes]; // Used as either an input or an output depending on whether the user would like to generate a new key pair.
	char peerDSAPubKey[keyBytes];
	bool generateNewDSAKeys = true;

	char userEphemeralPubKey[keyBytes];
	char peerEphemeralPubKey[keyBytes];

	char userSignature[signatureBytes];
	char peerSignature[signatureBytes];

	char userID[IDBytes]; // IDs are used for a fixed portion of a nonce.
	char peerID[IDBytes];

	const size_t tagBytes = ae.getTagBytes();

	unsigned long long messageCount; // Used to increment a nonce for each new message sent. Will be sent with each encrypted message.

	char tag[tagBytes]; // Used to authenticate encrypted messages. Will be sent with each encrypted message.


// -------------------- Establish Connection --------------------

	pki.initialize(userDSAPrivateKey, userDSAPubKey, userEphemeralPubKey, userSignature, userID, generateNewDSAKeys);

//	Exchange DSA public keys, ephemeral public keys, signatures, and IDs unencrypted.

	if((pki.IDUnique(userID, peerID)) && (pki.signatureValid(peerDSAPubKey, peerEphemeralPubKey, peerSignature))) {
		pki.createSessionKey(peerEphemeralPubKey); // Creates a shared private session key, overwriting peerEphemeralPubKey, if both users have different IDs and the peer's signature is valid.
	}

//	Ensure that the shared private session key has never been used by you before!

	ae.initialize(peerEphemeralPubKey, userID, peerID);


// -------------------- Encrypt and Decrypt --------------------

//	Input message and the number of bytes in message:
	const unsigned long long messageBytes = 2;
	char messageBuffer[messageBytes + 1] = {"42"};
	char message[messageBytes];

	for(unsigned long long i = 0; i < messageBytes; i += 1) {
		message[i] = messageBuffer[i];
	}

	ae.encryptAndTagMessage(messageCount, tag, message, messageBytes); // Encrypts message, overwriting it with the ciphertext. Outputs messageCount and the MAC tag as well.

//	Send the messageCount, tag, and message.

//	Upon receiving a ciphertext, messageCount, and tag:
	if(ae.messageAuthentic(message, messageBytes, messageCount, tag)) { // Authenticates the message with the MAC tag.
		ae.decryptAuthenticatedMessage(message, messageBytes, messageCount); // Decrypts message, overwriting it with the plaintext.
	}
}


void loop() {}