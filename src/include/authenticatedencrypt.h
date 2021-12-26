#ifndef AUTHENTICATEDENCRYPT_H
#define AUTHENTICATEDENCRYPT_H

#include "chacha.h"
#include "poly1305.h"

#include <stdint.h>


class CipherManagement {
private:
	ChaChaEncryption cipher;
	Poly1305MAC mac;

	static const constexpr unsigned short KEY_BYTES = 32;
	static const constexpr unsigned short POLY_KEY_LENGTH = 8;
	static const constexpr unsigned short TAG_BYTES = 16;
	static const constexpr unsigned short FIXED_NONCE_BYTES = 4;

	uint32_t* polyKeyMaterial;
	uint32_t polyKey[POLY_KEY_LENGTH];

	static const constexpr unsigned long INITIAL_BLOCK = 1;

	void createPolyKey();
public:
	CipherManagement();
	~CipherManagement();

	unsigned short getTagBytes() {return TAG_BYTES;}

	void initialize(char[KEY_BYTES], char[FIXED_NONCE_BYTES], char[FIXED_NONCE_BYTES]);

	void encryptAndTagMessage(unsigned long long&, char[TAG_BYTES], char*, unsigned long long);
	bool messageAuthentic(char*, unsigned long long, unsigned long long, char[TAG_BYTES]);
	void decryptAuthenticatedMessage(char*, unsigned long long, unsigned long long);
};


CipherManagement::CipherManagement() {

}


CipherManagement::~CipherManagement() {

}


void CipherManagement::initialize(char* sharedPrivateKey, char* userFixedNonce, char* peerFixedNonce) {
	cipher.buildEncryption(sharedPrivateKey, userFixedNonce, peerFixedNonce);
}


void CipherManagement::createPolyKey() {
	for(unsigned short i = 0; i < POLY_KEY_LENGTH; i += 1) {
		polyKey[i] = polyKeyMaterial[i] << 24;
		polyKey[i] |= ((polyKeyMaterial[i] & 0x0000ff00) << 8);
		polyKey[i] |= ((polyKeyMaterial[i] & 0x00ff0000) >> 8);
		polyKey[i] |= (polyKeyMaterial[i] >> 24);
	}
}


void CipherManagement::encryptAndTagMessage(unsigned long long& messageCountOut, char* tagOut, char* message, unsigned long long messageBytes) {
	if(messageBytes > 0) {
		polyKeyMaterial = cipher.generateEndState();
		createPolyKey();

		messageCountOut = cipher.getNonceCounter();
		cipher.encryptMessage(message, messageBytes, INITIAL_BLOCK);

		mac.createTag(tagOut, polyKey, message, messageBytes);
	}
}


bool CipherManagement::messageAuthentic(char* message, unsigned long long messageBytes, unsigned long long messageCount, char* tag) {
	if(messageBytes > 0) {
		polyKeyMaterial = cipher.generatePeerEndState(messageCount);
		createPolyKey();

		return mac.authenticateTag(tag, polyKey, message, messageBytes);
	}

	return false;
}


void CipherManagement::decryptAuthenticatedMessage(char* message, unsigned long long messageBytes, unsigned long long messageCount) {
	if(messageBytes > 0) {
		cipher.decryptMessage(message, messageBytes, messageCount, INITIAL_BLOCK);
	}
}

#endif