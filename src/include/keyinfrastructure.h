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
	char curveScalar[KEY_BYTES];

	char check;
public:
	KeyManagement();
	~KeyManagement();

	const unsigned short getIDBytes() {return ID_BYTES;}
	const unsigned short getKeyBytes() {return KEY_BYTES;}

	void initialize(char[ID_BYTES], char[KEY_BYTES]);

	bool IDUnique(char[ID_BYTES], char[ID_BYTES]);

	void createSessionKey(char[KEY_BYTES]);
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

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		curveScalar[i] = privateSessionKey[i];
	}

	ecdhe.curve25519(curveScalar, publicSessionKey);

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
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		curveScalar[i] = privateSessionKey[i];
	}

	ecdhe.curve25519(curveScalar, peerPubKey);
}

#endif