#ifndef X25519_H
#define X25519_H

#include "multiprecision.h"

#include <stdint.h>


class X25519KeyManagement {
private:
	static MultiPrecisionArithmetic math;

	static const constexpr unsigned short BYTELEN = 32;
	static const constexpr unsigned short INTLEN = 8;
	static const constexpr unsigned short INTLENMULTI = math.getMultiLength();
	static const constexpr unsigned short BITS = 255;

	uint32_t nInt[INTLEN];
	uint32_t xInt[INTLEN];
	uint32_t xIntMulti[INTLENMULTI];

	uint32_t X1[INTLENMULTI];
	uint32_t X2[INTLENMULTI];
	uint32_t Z2[INTLENMULTI];
	uint32_t X3[INTLENMULTI];
	uint32_t Z3[INTLENMULTI];

	uint32_t s;
	uint32_t bit;
	uint32_t mask;
	uint32_t swap;

	uint32_t A[INTLENMULTI];
	uint32_t AA[INTLENMULTI];
	uint32_t B[INTLENMULTI];
	uint32_t BB[INTLENMULTI];
	uint32_t E[INTLENMULTI];
	uint32_t C[INTLENMULTI];
	uint32_t D[INTLENMULTI];
	uint32_t DA[INTLENMULTI];
	uint32_t CB[INTLENMULTI];

	uint32_t A24[INTLEN] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0001db41}; // (486662 - 2)/4.
	uint32_t A24Multi[INTLENMULTI];

	char z;

	char* decodeBytesLittleEndian(char*);
	char* clampAndDecodeScalar(char*);
	void toUInt(uint32_t*, char*);
	char* maskAndDecodeXCoord(char*);

	void cSwap(uint32_t);
	void ladderStep();
	void montgomeryLadder();
	void reciprocal();

	char* encodeXCoord(char*);
	void checkAllZerosCase(char*);
public:
	X25519KeyManagement();
	~X25519KeyManagement();

	void curve25519(char[BYTELEN], char[BYTELEN]);
};


X25519KeyManagement::X25519KeyManagement() {

}


X25519KeyManagement::~X25519KeyManagement() {

}


char* X25519KeyManagement::decodeBytesLittleEndian(char* b) {
	char temp;
	for(unsigned short i = 0; i < BYTELEN/2; i += 1) {
		temp = b[i];
		b[i] = b[(BYTELEN - 1) - i];
		b[(BYTELEN - 1) - i] = temp;
	}

	return b;
}


char* X25519KeyManagement::clampAndDecodeScalar(char* n) {
	n[0] &= 0xf8;
	n[31] &= 0x7f;
	n[31] |= 0x40;

	n = decodeBytesLittleEndian(n);

	return n;
}


void X25519KeyManagement::toUInt(uint32_t* outInt, char* b) {
	for(unsigned short i = 0; i < INTLEN; i += 1) {
		outInt[i] = (b[i*4] << 24) | (b[(i*4) + 1] << 16) | (b[(i*4) + 2] << 8) | b[(i*4) + 3];
	}
}


char* X25519KeyManagement::maskAndDecodeXCoord(char* x) {
	x[31] &= 0x7f;

	x = decodeBytesLittleEndian(x);

	return x;
}


void X25519KeyManagement::cSwap(uint32_t s) {
	mask = 0x00000000;
	mask -= s;

	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		swap = mask & (X2[i] ^ X3[i]);
		X2[i] ^= swap;
		X3[i] ^= swap;
	}

	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		swap = mask & (Z2[i] ^ Z3[i]);
		Z2[i] ^= swap;
		Z3[i] ^= swap;
	}
}


void X25519KeyManagement::ladderStep() {
	math.base16Add(A, X2, Z2);
	math.base16Mul(AA, A, A);
	math.base16Sub(B, X2, Z2);
	math.base16Mul(BB, B, B);
	math.base16Sub(E, AA, BB);
	math.base16Add(C, X3, Z3);
	math.base16Sub(D, X3, Z3);
	math.base16Mul(DA, D, A);
	math.base16Mul(CB, C, B);
	math.base16Add(X3, DA, CB);
	math.base16Mul(X3, X3, X3);
	math.base16Sub(Z3, DA, CB);
	math.base16Mul(Z3, Z3, Z3);
	math.base16Mul(Z3, X1, Z3);
	math.base16Mul(X2, AA, BB);
	math.base16Mul(Z2, A24Multi, E);
	math.base16Add(Z2, AA, Z2);
	math.base16Mul(Z2, E, Z2);
}


void X25519KeyManagement::montgomeryLadder() {
	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		X1[i] = xIntMulti[i];
	}
	for(unsigned short i = 0; i < (INTLENMULTI - 1); i += 1) {
		X2[i] = 0x00000000;
	}
	X2[INTLENMULTI - 1] = 0x00000001;
	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		Z2[i] = 0x00000000;
	}
	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		X3[i] = xIntMulti[i];
	}
	for(unsigned short i = 0; i < (INTLENMULTI - 1); i += 1) {
		Z3[i] = 0x00000000;
	}
	Z3[INTLENMULTI - 1] = 0x00000001;
	s = 0x00000000;

	for(unsigned short i = (BITS - 1); i < BITS; i -= 1) {
		bit = ((nInt[(BITS - i)/32]) >> (i % 32)) & 0x00000001;
		s ^= bit;
		cSwap(s);
		s = bit;

		ladderStep();
	}

	cSwap(s);
}


void X25519KeyManagement::reciprocal() {
	math.base16Mul(A, Z2, Z2);
	math.base16Mul(Z3, A, A);
	math.base16Mul(X3, Z3, Z3);
	math.base16Mul(AA, X3, Z2);
	math.base16Mul(B, AA, A);
	math.base16Mul(X3, B, B);
	math.base16Mul(BB, X3, AA);

	math.base16Mul(X3, BB, BB);
	math.base16Mul(Z3, X3, X3);
	math.base16Mul(X3, Z3, Z3);
	math.base16Mul(Z3, X3, X3);
	math.base16Mul(X3, Z3, Z3);
	math.base16Mul(E, X3, BB);

	math.base16Mul(X3, E, E);
	math.base16Mul(Z3, X3, X3);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(X3, Z3, Z3);
		math.base16Mul(Z3, X3, X3);
	}
	math.base16Mul(C, Z3, E);

	math.base16Mul(X3, C, C);
	math.base16Mul(Z3, X3, X3);
	for(unsigned short i = 2; i < 20; i += 2) {
		math.base16Mul(X3, Z3, Z3);
		math.base16Mul(Z3, X3, X3);
	}
	math.base16Mul(X3, Z3, C);

	math.base16Mul(Z3, X3, X3);
	math.base16Mul(X3, Z3, Z3);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(Z3, X3, X3);
		math.base16Mul(X3, Z3, Z3);
	}
	math.base16Mul(D, X3, E);

	math.base16Mul(X3, D, D);
	math.base16Mul(Z3, X3, X3);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(X3, Z3, Z3);
		math.base16Mul(Z3, X3, X3);
	}
	math.base16Mul(DA, Z3, D);

	math.base16Mul(Z3, DA, DA);
	math.base16Mul(X3, Z3, Z3);
	for(unsigned short i = 2; i < 100; i += 2) {
		math.base16Mul(Z3, X3, X3);
		math.base16Mul(X3, Z3, Z3);
	}
	math.base16Mul(Z3, X3, DA);

	math.base16Mul(X3, Z3, Z3);
	math.base16Mul(Z3, X3, X3);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(X3, Z3, Z3);
		math.base16Mul(Z3, X3, X3);
	}
	math.base16Mul(X3, Z3, D);

	math.base16Mul(Z3, X3, X3);
	math.base16Mul(X3, Z3, Z3);
	math.base16Mul(Z3, X3, X3);
	math.base16Mul(X3, Z3, Z3);
	math.base16Mul(Z3, X3, X3);
	math.base16Mul(Z2, Z3, B);
}


char* X25519KeyManagement::encodeXCoord(char* x) {
	for(unsigned short i = 0; i < INTLEN; i += 1) {
		x[i*4] = xInt[i] >> 24;
		x[(i*4) + 1] = xInt[i] >> 16;
		x[(i*4) + 2] = xInt[i] >> 8;
		x[(i*4) + 3] = xInt[i];
	}

	x = decodeBytesLittleEndian(x);

	return x;
}


void X25519KeyManagement::checkAllZerosCase(char* x) {
	z = 0x00;

	for(unsigned short i = 0; i < BYTELEN; i += 1) {
		z |= x[i];
	}

	if(!z) {
		// Log an error here.
		// Wait until new private keys are chosen.
	}
}


void X25519KeyManagement::curve25519(char* n, char* xp) { // k is the independent, uniform, random secret key. It is an array of 32 bytes and is used as the scalar for the elliptic curve.
	n = clampAndDecodeScalar(n);

	toUInt(nInt, n);

	xp = maskAndDecodeXCoord(xp);

	toUInt(xInt, xp);
	math.base32_16(xIntMulti, xInt);

	math.base32_16(A24Multi, A24);

	montgomeryLadder();

	reciprocal();
	math.base16Mul(xIntMulti, X2, Z2);
	math.base16_32(xInt, xIntMulti);

	xp = encodeXCoord(xp);

	checkAllZerosCase(xp);
}

#endif