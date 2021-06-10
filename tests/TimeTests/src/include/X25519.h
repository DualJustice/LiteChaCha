#ifndef X25519_H
#define X25519_H

#include "Arduino.h" // DELETE ME!
#include "HardwareSerial.h" // DELETE ME!

#include <stdint.h>


class X25519KeyManagement {
private:
	static const constexpr unsigned short BYTELEN = 32;
	static const constexpr unsigned short INTLEN = 8;

	uint32_t nInt[INTLEN];
	uint32_t xInt[INTLEN];

	char* decodeBytesLittleEndian(char*);
	char* clampAndDecodeScalar(char*);
	void toUInt(uint32_t*, char*);
	char* maskAndDecodeXCoord(char*);
	void ladderStep();
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
		outInt[i] = (b[i*4] << 24) | (b[i*4 + 1] << 16) | (b[i*4 + 2] << 8) | b[i*4 + 3];
//		outInt[i] = (b[i*4 + 3] << 24) | (b[i*4 + 2] << 16) | (b[i*4 + 1] << 8) | b[i*4];
	}
}


char* X25519KeyManagement::maskAndDecodeXCoord(char* x) {
	x[31] &= 0x7f;

	x = decodeBytesLittleEndian(x);

	return x;
}


void X25519KeyManagement::ladderStep() {

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

//	montgomeryLadder(nInt, n, xInit);

//	reciprocal();

//	encodeXCoord(DABEEEEGBOI);

//	Serial.print("encoded final x-coord: ");
//	for(unsigned short i = 0; i < 32; i += 1) {
//		Serial.print(DAFINALANSWER[i], HEX);
//	}
}

#endif