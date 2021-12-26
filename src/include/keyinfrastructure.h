#ifndef KEYINFRASTRUCTURE_H
#define KEYINFRASTRUCTURE_H

#include "rng.h"
#include "X25519.h"
#include "Ed25519.h"


class KeyManagement {
private:
	RNGen randnum;
	X25519KeyExchange ecdhe;
	Ed25519SignatureAlgorithm hancock;

	static const constexpr unsigned short KEY_BYTES = 32;
	static const constexpr unsigned short SIGNATURE_BYTES = 64;
	static const constexpr unsigned short ID_BYTES = 4;

	char privateSessionKey[KEY_BYTES];
	char publicSessionKey[KEY_BYTES];
	const char basePoint[KEY_BYTES] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	char curveScalar[KEY_BYTES];

	char check;
public:
	unsigned short getKeyBytes() {return KEY_BYTES;}
	unsigned short getSignatureBytes() {return SIGNATURE_BYTES;}
	unsigned short getIDBytes() {return ID_BYTES;}

	void initialize(char[KEY_BYTES], char[KEY_BYTES], char[KEY_BYTES], char[SIGNATURE_BYTES], char[ID_BYTES], bool);

	bool IDUnique(char[ID_BYTES], char[ID_BYTES]);

	bool signatureValid(char[KEY_BYTES], char[KEY_BYTES], char[SIGNATURE_BYTES]);

	void createSessionKey(char[KEY_BYTES]);
};


inline void KeyManagement::initialize(char* DSAPrivateKeyInOut, char* DSAPubKeyInOut, char* ephemeralKeyOut, char* signatureOut, char* IDOut, bool generateNewDSAKeys = true) {
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
		ephemeralKeyOut[i] = publicSessionKey[i];
	}

	if(generateNewDSAKeys) {
		randnum.generateBytes(DSAPrivateKeyInOut, KEY_BYTES);
	}

	hancock.sign(signatureOut, DSAPubKeyInOut, DSAPrivateKeyInOut, publicSessionKey, generateNewDSAKeys);
}


inline bool KeyManagement::IDUnique(char* userID, char* peerID) {
	check = 0x00;

	for(unsigned short i = 0; i < ID_BYTES; i += 1) {
		check |= (userID[i] ^ peerID[i]);
	}

	return (check != 0x00);
}


inline bool KeyManagement::signatureValid(char* DSAPubKey, char* ephemeralPubKey, char* signature) {
	return hancock.verify(DSAPubKey, ephemeralPubKey, signature);
}


inline void KeyManagement::createSessionKey(char* peerEphemeralPubKey) {
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		curveScalar[i] = privateSessionKey[i];
	}

	ecdhe.curve25519(curveScalar, peerEphemeralPubKey);
}

#endif