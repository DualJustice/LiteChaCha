/*
This file exists to demonstrate the functions that are available to users in LiteChaCha, 
as well as the order of operations necessary to implement encrypted peer-to-peer communication.

The necessary order of operations is laid out by this file when read from the top down, line by line.
Although the functions used here should maintain their order, they need not be run successively! 
Place them in your implementation wherever it makes sense. The instructions on any commented lines 
which come between functions should be carried out by you, and in the correct order.

There are two basic units to LiteChaCha: establishing a secure connection and processing messages.
These are shown below in the Establish Connection, and Encrypt and Decrypt Blocks.
Generally speaking the KeyManagement object is used for establishing a connection 
while the CipherManagement object is used for processing messages. A secure connection need only be 
established once per session. Processing messages, on the other hand, can be carried out multiple 
times in a session. Once you are done with the Establish Connection Block, and have stored all 
desired variables, you may safely destruct the KeyManagement object, as well as the associated variables.

In encrypted peer-to-peer communication there are, unsurprisingly, two users: the user (you) 
and the peer (who you're communicating with). The variable names used below are not arbitrary; 
"user" variables apply to you while "peer" variables apply to your peer. 
Your "user" variables are your peer's "peer" variables, and vice versa.
As such, when you see the "Exchange DSA public keys, ephemeral public keys, signatures, and IDs unencrypted" 
step below, you should send the "user" versions, and should write to the "peer" versions (upon receiving) 
of said variables. Variables that don't specify "user" or "peer" can be used interchangeably.

This goes without saying, but do not send private variables unless you explicitly intend to do so 
and know the consequences! The private variables used by LiteChaCha are peerEphemeralPubKey after 
pki.createSessionKey(peerEphemeralPubKey) is called, and userDSAPrivateKey.
*/


#include "keyinfrastructure.h"
#include "authenticatedencrypt.h"


void setup() {
	KeyManagement pki;
	CipherManagement ae;

	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


// -------------------- Necessary Variables --------------------

//	Establish Connection Block Variables:
	const size_t keyBytes = pki.getKeyBytes();
	const size_t signatureBytes = pki.getSignatureBytes();
	const size_t IDBytes = pki.getIDBytes();

	unsigned char userDSAPrivateKey[keyBytes]; // Used as either an input or an output depending on whether the user would like to generate a new key pair.
	unsigned char userDSAPubKey[keyBytes]; // Used as either an input or an output depending on whether the user would like to generate a new key pair.
	unsigned char peerDSAPubKey[keyBytes];
	bool generateNewDSAKeys = true;

	unsigned char userEphemeralPubKey[keyBytes];
	unsigned char peerEphemeralPubKey[keyBytes];

	unsigned char userSignature[signatureBytes];
	unsigned char peerSignature[signatureBytes];

	unsigned char userID[IDBytes]; // IDs are used for a fixed portion of a nonce.
	unsigned char peerID[IDBytes];

//	Encrypt and Decrypt Block Variables:
	const size_t tagBytes = ae.getTagBytes();

	unsigned long long messageCount; // Used to increment a nonce for each new message sent. Will be sent with each encrypted message.

	unsigned char tag[tagBytes]; // Used to authenticate encrypted messages. Will be sent with each encrypted message.


// -------------------- Establish Connection Block --------------------

	pki.initialize(userDSAPrivateKey, userDSAPubKey, userEphemeralPubKey, userSignature, userID, generateNewDSAKeys);

//	Exchange DSA public keys, ephemeral public keys, signatures, and IDs unencrypted.

	if((pki.IDUnique(userID, peerID)) && (pki.signatureValid(peerDSAPubKey, peerEphemeralPubKey, peerSignature))) {
		pki.createSessionKey(peerEphemeralPubKey); // Creates a shared private session key, overwriting peerEphemeralPubKey, if both users have different IDs and the peer's signature is valid.
	}

//	If possible, ensure that the shared private session key has never been used by you before!

	ae.initialize(peerEphemeralPubKey, userID, peerID);


// -------------------- Encrypt and Decrypt Block --------------------

//	Outgoing message and the number of bytes in the message:
	const unsigned long long messageBytes = 2;
	char messageBuffer[messageBytes + 1] = {"42"};
	char message[messageBytes];

	for(unsigned long long i = 0; i < messageBytes; i += 1) {
		message[i] = messageBuffer[i];
	}

	ae.encryptAndTagMessage(messageCount, tag, (unsigned char*)message, messageBytes); // Encrypts message, overwriting it with the ciphertext. Outputs messageCount and the MAC tag as well.

//	Send the messageCount, tag, and message.

//	Upon receiving a ciphertext, messageCount, and tag:
	if(ae.messageAuthentic((unsigned char*)message, messageBytes, messageCount, tag)) { // Authenticates the message with the MAC tag.
		ae.decryptAuthenticatedMessage((unsigned char*)message, messageBytes, messageCount); // Decrypts message, overwriting it with the plaintext.
	}
}


void loop() {}