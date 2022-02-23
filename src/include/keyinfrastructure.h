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

	unsigned char privateSessionKey[KEY_BYTES];
	unsigned char publicSessionKey[KEY_BYTES];
	const unsigned char basePoint[KEY_BYTES] = {0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
	unsigned char curveScalar[KEY_BYTES];

	unsigned char check;
public:
	unsigned short getKeyBytes() {return KEY_BYTES;}
	unsigned short getSignatureBytes() {return SIGNATURE_BYTES;}
	unsigned short getIDBytes() {return ID_BYTES;}

	void initialize(unsigned char[KEY_BYTES], unsigned char[KEY_BYTES], unsigned char[KEY_BYTES], unsigned char[SIGNATURE_BYTES], unsigned char[ID_BYTES], const bool);

	bool IDUnique(const unsigned char[ID_BYTES], const unsigned char[ID_BYTES]);

	bool signatureValid(const unsigned char[KEY_BYTES], const unsigned char[KEY_BYTES], const unsigned char[SIGNATURE_BYTES]);

	void createSessionKey(unsigned char[KEY_BYTES]);
};


inline void KeyManagement::initialize(unsigned char* DSAPrivateKeyInOut, unsigned char* DSAPubKeyInOut, unsigned char* ephemeralKeyOut, unsigned char* signatureOut, unsigned char* IDOut, const bool generateNewDSAKeys = true) {
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


inline bool KeyManagement::IDUnique(const unsigned char* userID, const unsigned char* peerID) {
	check = 0x00;

	for(unsigned short i = 0; i < ID_BYTES; i += 1) {
		check |= (userID[i] ^ peerID[i]);
	}

	return (check != 0x00);
}


inline bool KeyManagement::signatureValid(const unsigned char* DSAPubKey, const unsigned char* ephemeralPubKey, const unsigned char* signature) {
	return hancock.verify(DSAPubKey, ephemeralPubKey, signature);
}


inline void KeyManagement::createSessionKey(unsigned char* peerEphemeralPubKey) {
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		curveScalar[i] = privateSessionKey[i];
	}

	ecdhe.curve25519(curveScalar, peerEphemeralPubKey);
}

#endif