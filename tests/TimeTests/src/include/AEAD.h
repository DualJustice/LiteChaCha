#ifndef AEAD_H
#define AEAD_H

#include "chacha.h"
#include "poly1305.h"

#include <stdint.h>


class AEADConstruct {
private:
	ChaChaEncryption cipher;
	Poly1305MAC mac;

	static const constexpr unsigned short KEY_BYTES = 32;
	static const constexpr unsigned short FIXED_NONCE_BYTES = 4;
	static const constexpr unsigned short TAG_BYTES = 16;
	static const constexpr unsigned short POLY_KEY_LENGTH = 8;

	static const constexpr char emptyLength = 1;
	char empty[emptyLength] = {0x00};
	uint32_t* polyKeyMaterial;
	uint32_t polyKey[POLY_KEY_LENGTH];
public:
	AEADConstruct();
	~AEADConstruct();

	void initialize(char[KEY_BYTES], char[FIXED_NONCE_BYTES], char[FIXED_NONCE_BYTES]);

	void createPolyKey();
	void encryptAndTagMessage(char*, unsigned long long, char[TAG_BYTES]);
	bool messageAuthentic();
	void decryptAuthenticatedMessage();
};


AEADConstruct::AEADConstruct() {

}


AEADConstruct::~AEADConstruct() {

}


void AEADConstruct::initialize(char* sharedPrivateKey, char* userFixedNonce, char* peerFixedNonce) {
	cipher.buildEncryption(sharedPrivateKey, userFixedNonce, peerFixedNonce);
}


void AEADConstruct::createPolyKey() {
	cipher.encryptMessage(empty, emptyLength);

	polyKeyMaterial = cipher.getLastEndState();

	for(unsigned short i = 0; i < POLY_KEY_LENGTH; i += 1) {
		polyKey[i] = polyKeyMaterial[i] << 24;
		polyKey[i] |= ((polyKeyMaterial[i] & 0x0000ff00) << 8);
		polyKey[i] |= ((polyKeyMaterial[i] & 0x00ff0000) >> 8);
		polyKey[i] |= (polyKeyMaterial[i] >> 24);
	}
}


void AEADConstruct::encryptAndTagMessage(char* message, unsigned long long messageBytes, char* tagOut) {
	createPolyKey();
}

#endif