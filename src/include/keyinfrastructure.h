#ifndef KEYINFRASTRUCTURE_H
#define KEYINFRASTRUCTURE_H

#include "rng.h"
#include "X25519.h"


class KeyManagement {
private:
	RNGen randnum;
	X25519KeyExchange ecdhe;

	static const constexpr unsigned short ID_BYTES = 4;
	static const constexpr unsigned short KEY_BYTES = 32;

	char privateSessionKey[KEY_BYTES];
	char publicSessionKey[KEY_BYTES];
	const char basePoint[KEY_BYTES] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

	char check;
public:
	KeyManagement();
	~KeyManagement();

	const unsigned short getIDBytes() {return ID_BYTES;}
	const unsigned short getKeyBytes() {return KEY_BYTES;}

	void initialize(char[ID_BYTES], char[KEY_BYTES]); // Generates a random user ID (to be used as the fixed nonce in ChaCha) and a random public session key.

	bool IDUnique(char[ID_BYTES], char[ID_BYTES]);

	void createSessionKey(char[KEY_BYTES]); // Input is the peer's public session key. It destructively outputs the session shared secret key over the input key.
};


KeyManagement::KeyManagement() {

}


KeyManagement::~KeyManagement() {

}


void KeyManagement::initialize(char* IDOut, char* keyOut) {
	randnum.generateBytes(IDOut, ID_BYTES);
	randnum.generateBytes(privateSessionKey, KEY_BYTES);

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		publicSessionKey[i] = basePoint[i];
	}

	ecdhe.curve25519(privateSessionKey, publicSessionKey);

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		keyOut[i] = publicSessionKey[i];
	}
}


bool KeyManagement::IDUnique(char* userID, char* peerID) {
	check = 0x00;

	for(unsigned short i = 0; i < ID_BYTES; i += 1) {
		check |= (userID[i] ^ peerID[i]);
	}

	return (check != 0x00);
}


void KeyManagement::createSessionKey(char* peerPubKey) {
	ecdhe.curve25519(privateSessionKey, peerPubKey);
}

#endif