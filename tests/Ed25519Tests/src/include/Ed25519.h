#ifndef ED25519_H
#define ED25519_H

#include "SHA512.h"
#include "multiprecision25519.h"

#include <stdint.h>


class Ed25519SignatureAlgorithm {
private:
	SHA512Hash hash;
	MultiPrecisionArithmetic25519 math;

	static const constexpr unsigned short HASH_BYTES = 64;
	static const constexpr unsigned short KEY_BYTES = 32;

	char h[HASH_BYTES]; // Hash buffer.

	char s[KEY_BYTES]; // Secret scalar.
	char prefix[KEY_BYTES];
	char publicKey[KEY_BYTES];

	void pruneHashBuffer();
public:
	Ed25519SignatureAlgorithm();
	~Ed25519SignatureAlgorithm();

	void initialize(char[KEY_BYTES]);

	void sign();
	void verify();
};


Ed25519SignatureAlgorithm::Ed25519SignatureAlgorithm() {

}


Ed25519SignatureAlgorithm::~Ed25519SignatureAlgorithm() {

}


void Ed25519SignatureAlgorithm::pruneHashBuffer() {
	h[0] &= 0xf8;
	h[31] &= 0x7f;
	h[31] |= 0x40;
}


void Ed25519SignatureAlgorithm::initialize(char* privateKey) {
	hash.hashBytes(h, privateKey, KEY_BYTES);
	pruneHashBuffer();

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		s[i] = h[(KEY_BYTES - 1) - i];
	}
	for(unsigned short i = KEY_BYTES; i < HASH_BYTES; i += 1) {
		prefix[i - KEY_BYTES] = h[i];
	}

	
}


void Ed25519SignatureAlgorithm::sign() {

}


void Ed25519SignatureAlgorithm::verify() {

}

#endif