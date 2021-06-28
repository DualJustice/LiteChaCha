#ifndef X25519_H
#define X25519_H

#include "Arduino.h" // DELETE ME!
#include "HardwareSerial.h" // DELETE ME!

#include "multiprecision.h"
multiPrecisionArithmetic math;

#include <stdint.h>


class X25519KeyManagement {
private:
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

	char* decodeBytesLittleEndian(char*);
	char* clampAndDecodeScalar(char*);
	void toUInt(uint32_t*, char*);
	char* maskAndDecodeXCoord(char*);

	void cSwap(uint32_t);
	void ladderStep();
	void montgomeryLadder();
	void reciprocal();

	char* encodeXCoord(char*);

//	void ladderStep(BigNumber, BigNumber, BigNumber, BigNumber, BigNumber, char);
//	void montgomeryLadder(BigNumber, char*, BigNumber);
//	void reciprocal();
//	void encodeXCoord(BigNumber);
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
	/*
	A:  z2
	AA: z9
	B:  z11
	BB: z2_5_0
	E:  z2_10_0
	C:  z2_20_0
	D:  z2_50_0
	DA: z2_100_0
	X3: t0
	Z3: t1
	*/

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


/*
void X25519KeyManagement::encodeXCoord(BigNumber x) {
	unsigned short i = 0;
	BigNumber quotient = 1;
	BigNumber remainder = 0;
	BigNumber stuff = 256;

	while(quotient != 0) {
		quotient = x/stuff;
		remainder = x % stuff;
		x = quotient;

		DAFINALANSWER[i] = remainder;
		i += 1;
	}
}
*/
/*
BigNumber X25519KeyManagement::makeDec(char* b, unsigned short c) {
	BigNumber bigDec = 0; // Potential memory leak?
	BigNumber significand = 0; // Potential memory leak?
	BigNumber base = 256; // Potential memory leak?

	for(unsigned short i = 0; i < c; i += 1) {
		significand = b[i];
		bigDec += significand*(base.pow((c - 1) - i));
//		Serial.println(bigDec);
	}

	return bigDec;
}
*/
/*
void X25519KeyManagement::ladderStep(X1, UV0, UV1, UV2, UV3, char bit) {
	T1 = (UV0 + UV1) % p;			// 0

	T2 = (UV0 - UV1) % p;			// 1
	if(T2 < 0) {
		T2 = T2 + p;
	}

	T3 = (UV2 + UV3) % p;			// 2

	T4 = (UV2 - UV3) % p;			// 3
	if(T4 < 0) {
		T4 = T4 + p;
	}

	T5 = (T1.pow(2)) % p;			// 4

	T6 = (T2.pow(2)) % p;			// 5

	T2 = (T2*T3) % p;				// 6

	T1 = (T1*T4) % p;				// 7

	T1 = (T1 + T2) % p;				// 8

	T2 = (T1 - T2) % p;				// 9
	if(T2 < 0) {
		T2 = T2 + p;
	}

	UV2 = (T1.pow(2)) % p;			// 10

	T2 = (T2.pow(2)) % p;			// 11

	UV3 = (T2*X1) % p;				// 12

	UV0 = (T5*T6) % p;				// 13

	T5 = (T5 - T6) % p;				// 14
	if(T5 < 0) {
		T5 = T5 + p;
	}

	T1 = (A24*T5) % p;				// 15

	T6 = (T6 + T1) % p;				// 16

	UV1 = (T5*T6) % p;				// 17

	if(bit) {
		X3 = UV0;
		Z3 = UV1;
		X2 = UV2;
		Z2 = UV3;
	} else {
		X2 = UV0;
		Z2 = UV1;
		X3 = UV2;
		Z3 = UV3;
	}
}
*/
/*
void X25519KeyManagement::montgomeryLadder(nInt, char* n, xInit) {
	X1 = xInit;
	X2 = 1;
	Z2 = 0;
	X3 = xInit;
	Z3 = 1;

	for(unsigned short i = 254; i < 255; i -= 1) { // Sould run 255 times. There are a total of 255 bits.
		bit = ((n[(255 - i)/8]) >> (i % 8)) & 0x01; // n is also a 255 bit number. I believe the indexing of this for loop is GOOD (for rfc7748).

		if(bit) {

			ladderStep(X1, X3, Z3, X2, Z2, bit);

		} else {

			ladderStep(X1, X2, Z2, X3, Z3, bit);

		}
	}
}
*/
/*
void X25519KeyManagement::reciprocal() {
	ZS = (Z2.pow(2)) % p;
	T8 = (ZS.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	Z9 = (T7*Z2) % p;
	Z11 = (Z9*ZS) % p;
	T7 = (Z11.pow(2)) % p;
	Z2_5_0 = (T7*Z9) % p;

	T7 = (Z2_5_0.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	Z2_10_0 = (T7*Z2_5_0) % p;

	T7 = (Z2_10_0.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	for(unsigned short i = 2; i < 10; i += 2) {
		T7 = (T8.pow(2)) % p;
		T8 = (T7.pow(2)) % p;
	}
	Z2_20_0 = (T8*Z2_10_0) % p;

	T7 = (Z2_20_0.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	for(unsigned short i = 2; i < 20; i += 2) {
		T7 = (T8.pow(2)) % p;
		T8 = (T7.pow(2)) % p;
	}
	T7 = (T8*Z2_20_0) % p;

	T8 = (T7.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	for(unsigned short i = 2; i < 10; i += 2) {
		T8 = (T7.pow(2)) % p;
		T7 = (T8.pow(2)) % p;
	}
	Z2_50_0 = (T7*Z2_10_0) % p;

	T7 = (Z2_50_0.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	for(unsigned short i = 2; i < 50; i += 2) {
		T7 = (T8.pow(2)) % p;
		T8 = (T7.pow(2)) % p;
	}
	Z2_100_0 = (T8*Z2_50_0) % p;

	T8 = (Z2_100_0.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	for(unsigned short i = 2; i < 100; i += 2) {
		T8 = (T7.pow(2)) % p;
		T7 = (T8.pow(2)) % p;
	}
	T8 = (T7*Z2_100_0) % p;

	T7 = (T8.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	for(unsigned short i = 2; i < 50; i += 2) {
		T7 = (T8.pow(2)) % p;
		T8 = (T7.pow(2)) % p;
	}
	T7 = (T8*Z2_50_0) % p;

	T8 = (T7.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	T7 = (T8.pow(2)) % p;
	T8 = (T7.pow(2)) % p;
	daFinalNumber = (T8*Z11) % p;

	DABEEEEGBOI = (X2*daFinalNumber) % p; // I believe this is correct but it might not be.
}
*/


void X25519KeyManagement::curve25519(char* n, char* xp) { // k is the independent, uniform, random secret key. It is an array of 32 bytes and is used as the scalar for the elliptic curve.
	n = clampAndDecodeScalar(n);

	Serial.print("Clamped and decoded scalar: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		if(n[i] > 0x0f) {
			Serial.print(n[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(n[i], HEX);
		}
	}
	Serial.println('\n');

	toUInt(nInt, n);

	xp = maskAndDecodeXCoord(xp);

	Serial.print("Masked and decoded x-coord: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		if(xp[i] > 0x0f) {
			Serial.print(xp[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(xp[i], HEX);
		}
	}
	Serial.println('\n');

	toUInt(xInt, xp);
	math.base32_16(xIntMulti, xInt);

	math.base32_16(A24Multi, A24);

	montgomeryLadder();

	reciprocal();
	math.base16Mul(xIntMulti, X2, Z2);
	math.base16_32(xInt, xIntMulti);

	xp = encodeXCoord(xp);

	Serial.print("Encoded key: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		if(xp[i] > 0x0f) {
			Serial.print(xp[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(xp[i], HEX);
		}
	}
	Serial.println('\n');

//	reciprocal();

//	encodeXCoord(DABEEEEGBOI);

//	Serial.print("encoded final x-coord: ");
//	for(unsigned short i = 0; i < 32; i += 1) {
//		Serial.print(DAFINALANSWER[i], HEX);
//	}
}

#endif