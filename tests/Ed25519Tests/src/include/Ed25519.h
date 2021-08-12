#ifndef ED25519_H
#define ED25519_H

#include "SHA512.h"
#include "multiprecision25519.h"

#include <stdint.h>


static const constexpr unsigned short INT_LENGTH = 8;
static const constexpr unsigned short INT_LENGTH_MULTI = 2*INT_LENGTH;

struct Point {
	uint32_t X[INT_LENGTH_MULTI];
	uint32_t Y[INT_LENGTH_MULTI];
	uint32_t Z[INT_LENGTH_MULTI];
	uint32_t T[INT_LENGTH_MULTI];
};


class Ed25519SignatureAlgorithm {
private:
	SHA512Hash hash;
	MultiPrecisionArithmetic25519 math;

	static const constexpr unsigned short HASH_BYTES = 64;
	static const constexpr unsigned short KEY_BYTES = 32;
//	static const constexpr unsigned short INT_LENGTH = 8;
//	static const constexpr unsigned short INT_LENGTH_MULTI = 2*INT_LENGTH;
	static const constexpr unsigned short BITS = 255;

	char h[HASH_BYTES]; // Hash buffer.

	char sByte[KEY_BYTES]; // Secret scalar.
	char prefix[KEY_BYTES];

	uint32_t s[INT_LENGTH_MULTI];

	Point P;
	Point Q;

//	Base point.
	const uint32_t BX[INT_LENGTH_MULTI] = {0x00002169, 0x000036d3, 0x0000cd6e, 0x000053fe, 0x0000c0a4, 0x0000e231, 0x0000fdd6, 0x0000dc5c, 0x0000692c, 0x0000c760, 0x00009525, 0x0000a7b2, 0x0000c956, 0x00002d60, 0x00008f25, 0x0000d51a};
	const uint32_t BY[INT_LENGTH_MULTI] = {0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006658};
	const uint32_t BZ[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};
	const uint32_t BT[INT_LENGTH_MULTI] = {0x00006787, 0x00005f0f, 0x0000d78b, 0x00007665, 0x000066ea, 0x00004e8e, 0x000064ab, 0x0000e37d, 0x000020f0, 0x00009f80, 0x00007751, 0x000052f5, 0x00006dde, 0x00008ab3, 0x0000a5b7, 0x0000dda3};

//	Neutral element.
	const uint32_t NX[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
	const uint32_t NY[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};
	const uint32_t NZ[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};
	const uint32_t NT[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

	char publicKey[KEY_BYTES];

	void pruneHashBuffer();

	void bytesToMulti();

	void Ed25519();
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


void Ed25519SignatureAlgorithm::bytesToMulti() {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		s[i] = sByte[(i*4)] << 24;
		s[i] |= (sByte[(i*4) + 1] << 16);
		s[i] |= (sByte[(i*4) + 2] << 8);
		s[i] |= sByte[(i*4) + 3];
	}
}


void Ed25519SignatureAlgorithm::Ed25519() {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		Q.X[i] = NX[i];
		Q.Y[i] = NY[i];
		Q.Z[i] = NZ[i];
		Q.T[i] = NT[i];
	}

	for(unsigned short i = (BITS - 1); i < BITS; i -= 1) {
		// How am I going to make this constant time?
	}
}


void Ed25519SignatureAlgorithm::initialize(char* privateKey) {
	hash.hashBytes(h, privateKey, KEY_BYTES);
	pruneHashBuffer();

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		sByte[i] = h[(KEY_BYTES - 1) - i];
	}
	for(unsigned short i = KEY_BYTES; i < HASH_BYTES; i += 1) {
		prefix[i - KEY_BYTES] = h[i];
	}

	bytesToMulti();

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		P.X[i] = BX[i];
		P.Y[i] = BY[i];
		P.Z[i] = BZ[i];
		P.T[i] = BT[i];
	}

	Ed25519();
}


void Ed25519SignatureAlgorithm::sign() {

}


void Ed25519SignatureAlgorithm::verify() {

}

#endif