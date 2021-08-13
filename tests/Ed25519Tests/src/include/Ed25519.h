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

	char bit;
	Point Conditional; // Used as a conditional, unused point for contant-time.

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

	uint32_t A[INT_LENGTH_MULTI];
	uint32_t B[INT_LENGTH_MULTI];
	uint32_t C[INT_LENGTH_MULTI];
	uint32_t D[INT_LENGTH_MULTI];
	uint32_t E[INT_LENGTH_MULTI];
	uint32_t F[INT_LENGTH_MULTI];
	uint32_t G[INT_LENGTH_MULTI];
	uint32_t H[INT_LENGTH_MULTI];

	uint32_t d[INT_LENGTH_MULTI] = {0x00002406, 0x0000d9dc, 0x000056df, 0x0000fce7, 0x0000198e, 0x000080f2, 0x0000eef3, 0x0000d130, 0x000000e0, 0x0000149a, 0x00008283, 0x0000b156, 0x0000ebd6, 0x00009b94, 0x000026b2, 0x0000f159}; // 2*d, technically.

	char publicKey[KEY_BYTES];

	void pruneHashBuffer();

	void bytesToMulti();

	void ladderAdd(Point);
	void ladderDouble();
	void Ed25519();

	void inverse();
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
		s[i] = sByte[i*2] << 8;
		s[i] |= sByte[(i*2) + 1];
	}
}


void Ed25519SignatureAlgorithm::ladderAdd(Point out) {
	math.base16Sub(A, Q.Y, Q.X);
	math.base16Sub(B, P.Y, P.X);
	math.base16Mul(A, A, B);
	math.base16Add(B, Q.X, Q.Y);
	math.base16Add(C, P.X, P.Y);
	math.base16Mul(B, B, C);
	math.base16Mul(C, d, Q.T);
	math.base16Mul(C, C, P.T);
	math.base16Add(D, Q.Z, Q.Z);
	math.base16Mul(D, D, P.Z);
	math.base16Sub(E, B, A);
	math.base16Sub(F, D, C);
	math.base16Add(G, C, D);
	math.base16Add(H, A, B);
	math.base16Mul(out.X, E, F);
	math.base16Mul(out.Y, G, H);
	math.base16Mul(out.Z, F, G);
	math.base16Mul(out.T, E, H);
}


void Ed25519SignatureAlgorithm::ladderDouble() {
	math.base16Mul(A, P.X, P.X);
	math.base16Mul(B, P.Y, P.Y);
	math.base16Mul(C, P.Z, P.Z);
	math.base16Add(C, C, C);
	math.base16Add(D, A, B);
	math.base16Add(E, P.X, P.Y);
	math.base16Mul(E, E, E);
	math.base16Sub(E, D, E);
	math.base16Sub(F, A, B);
	math.base16Add(G, C, F);
	math.base16Mul(P.X, E, G);
	math.base16Mul(P.Y, D, F);
	math.base16Mul(P.Z, F, G);
	math.base16Mul(P.T, D, E);
}


void Ed25519SignatureAlgorithm::Ed25519() {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		Q.X[i] = NX[i];
		Q.Y[i] = NY[i];
		Q.Z[i] = NZ[i];
		Q.T[i] = NT[i];
	}

	for(unsigned short i = 0; i < BITS; i -= 1) {
		bit = ((sByte[(BITS - i)/8]) >> (i % 8)) & 0x01;

		if(bit == 0x01) {
			ladderAdd(Q);
		} else {
			ladderAdd(Conditional); // This is not elegant, nor is it efficient, but it should be constant time.
		}
		ladderDouble();
	}
}


/*
What you have:
A - H | 8 buffers. Use P.X, P.Y, and P.Z

what you need:
A, Z3, X3, AA, B, X3, BB, DA, E, C, D | 11 buffers.
*/


void Ed25519SignatureAlgorithm::inverse() { // Copied directly from Daniel J. Bernstein.
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

	inverse();
}


void Ed25519SignatureAlgorithm::sign() {

}


void Ed25519SignatureAlgorithm::verify() {

}

#endif