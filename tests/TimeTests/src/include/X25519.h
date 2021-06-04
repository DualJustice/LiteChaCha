#ifndef X25519_H
#define X25519_H

#include "Arduino.h" // DELETE ME!
#include "HardwareSerial.h" // DELETE ME!

//#include <stdint.h> // A distant dream.

#include "BigNumber.h"


class X25519KeyManagement {
private:
	BigNumber T1;
	BigNumber T2;
	BigNumber T3;
	BigNumber T4;
	BigNumber T5;
	BigNumber T6;
	BigNumber X1;
	BigNumber X3;
	BigNumber Z3;
	BigNumber X2;
	BigNumber Z2;

	char bit;

//	BigNumber U[4] = {X2, Z2, X3, Z3};
//	BigNumber V[4] = {X3, Z3, X2, Z2};

	static const constexpr unsigned long A24 = 121665; // (486662 - 2)/4
	BigNumber p = "57896044618658097711785492504343953926634992332820282019728792003956564819949"; // = (2^255) - 19, which is a 255 bit number.

	BigNumber ZS;
	BigNumber Z9;
	BigNumber Z11;
	BigNumber Z2_5_0;
	BigNumber Z2_10_0;
	BigNumber Z2_20_0;
	BigNumber Z2_50_0;
	BigNumber Z2_100_0;
	BigNumber T7;
	BigNumber T8;

	BigNumber daFinalNumber;
	BigNumber DABEEEEGBOI;

	char DAFINALANSWER[32];

	char* decodeBytesLittleEndian(char*, unsigned short);
	char* maskAndDecodeXCoord(char*);
	void encodeXCoord(BigNumber);
	char* clampAndDecodeScalar(char*);
	BigNumber makeDec(char*, unsigned short);
	void ladderStep(BigNumber, BigNumber&, BigNumber&, BigNumber&, BigNumber&);
	void montgomeryLadder(BigNumber, char*, BigNumber);
	void reciprocal();
public:
	X25519KeyManagement();
	~X25519KeyManagement();

	void startBigNum();

	void curve25519(char*, char[32]);
};


X25519KeyManagement::X25519KeyManagement() {

}


X25519KeyManagement::~X25519KeyManagement() {

}


void X25519KeyManagement::startBigNum() {
	BigNumber::begin();
}


char* X25519KeyManagement::decodeBytesLittleEndian(char* b, unsigned short c) { // Can optimize in the future by removing unnecessary copies.
	char* bLE = new char[c];

	for(unsigned short i = 0; i < c; i += 1) {
		bLE[i] = b[(c - 1) - i];
	}
	for(unsigned short i = 0; i < c; i += 1) {
		b[i] = bLE[i];
	}

	delete[] bLE;
	return b;
}


char* X25519KeyManagement::maskAndDecodeXCoord(char* x) {
	x[31] &= 0x7f;

	x = decodeBytesLittleEndian(x, 32);

	return x;
}


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


char* X25519KeyManagement::clampAndDecodeScalar(char* n) {
	n[0] &= 0xf8;
	n[31] &= 0x7f;
	n[31] |= 0x40;

	n = decodeBytesLittleEndian(n, 32);

	return n;
}


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


void X25519KeyManagement::ladderStep(BigNumber X1, BigNumber& UV0, BigNumber& UV1, BigNumber& UV2, BigNumber& UV3) {
/*
X2 = UV[0]
Z2 = UV[1]
X3 = UV[2]
Z3 = UV[3]
*/
	T1 = (UV0 + UV1) % p;			// 1
//	Serial.print("T1: ");
//	Serial.println(T1);
	T2 = (UV0 - UV1) % p;			// 2
//	Serial.print("T2: ");
//	Serial.println(T2);
	T3 = (UV2 + UV3) % p;			// 3
//	Serial.print("T3: ");
//	Serial.println(T3);
	T4 = (UV2 - UV3) % p;			// 4
//	Serial.print("T4: ");
//	Serial.println(T4);
	T5 = (T1.pow(2)) % p;			// 5
//	Serial.print("T5: ");
//	Serial.println(T5);
	T6 = (T2.pow(2)) % p;			// 6
//	Serial.print("T6: ");
//	Serial.println(T6);
	T2 = (T2*T3) % p;				// 7
//	Serial.print("T2: ");
//	Serial.println(T2);
	T1 = (T1*T4) % p;				// 8
//	Serial.print("T1: ");
//	Serial.println(T1);
	T1 = (T1 + T2) % p;				// 9
//	Serial.print("T1: ");
//	Serial.println(T1);
	T2 = (T1 - T2) % p;				// 10
//	Serial.print("T2: ");
//	Serial.println(T2);
	UV2 = (T1.pow(2)) % p;			// 11
//	Serial.print("UV2: ");
//	Serial.println(UV2);
	T2 = (T2.pow(2)) % p;			// 12
//	Serial.print("T2: ");
//	Serial.println(T2);
	UV3 = (T2*X1) % p;				// 13
//	Serial.print("UV3: ");
//	Serial.println(UV3);
	UV0 = (T5*T6) % p;				// 14
//	Serial.print("UV0: ");
//	Serial.println(UV0);
	T5 = (T5 - T6) % p;				// 15
//	Serial.print("T5: ");
//	Serial.println(T5);
	T1 = (A24*T5) % p;				// 16
//	Serial.print("T1: ");
//	Serial.println(T1);
	T6 = (T6 + T1) % p;				// 17
//	Serial.print("T6: ");
//	Serial.println(T6);
	UV1 = (T5*T6) % p;				// 18
//	Serial.print("UV1: ");
//	Serial.println(UV1);
}


void X25519KeyManagement::montgomeryLadder(BigNumber nInt, char* n, BigNumber xInit) {
	X1 = xInit;
	X2 = 1;
	Z2 = 0;
	X3 = xInit;
	Z3 = 1;
/*
	Serial.println(X1);
	Serial.println(X2);
	Serial.println(Z2);
	Serial.println(X3);
	Serial.println(Z3);
	Serial.println();
*/
//BigNumber U[4] = {X2, Z2, X3, Z3};
//BigNumber V[4] = {X3, Z3, X2, Z2};
/*
	Serial.println(U[0]);
	Serial.println(U[1]);
	Serial.println(U[2]);
	Serial.println(U[3]);
	Serial.println();
	Serial.println(V[0]);
	Serial.println(V[1]);
	Serial.println(V[2]);
	Serial.println(V[3]);
	Serial.println();
*/
	Serial.println("Pre for loop.");

	Serial.println("bit should be: 100010010011010010001001011101001000100001000100110101001010000000110000101101011111100110000010000101001001100000101000110001011011101010111100100011010000010010010110001010100010110001110111001110101111100010100101111000001101011111000110100011010100000");
	Serial.print("bit:           ");

	for(unsigned short i = 254; i < 255; i -= 1) { // Sould run 255 times. There are a total of 255 bits.
		bit = ((n[(255 - i)/8]) >> (i % 8)) & 0x01; // n is also a 255 bit number. I believe the indexing of this for loop is GOOD (for rfc7748).

//		Serial.print("bit: ");
		Serial.print(bit, HEX); // This seems to be pumping out the correct bits (at least definitely according to rfc7748).
//		How this is done might be different between rfc7748 & 1410.pdf.
//		We are getting the bytes at index 0 to index 254 respectively.
//		Because we decoded in bytes and not in binary, it is possible that the 1410.pdf version requires us to...
//		... pull the bits at the END of the bytes first as opposed to at the beginning.
//		Maybe, just MAYBE, you might need to pull bits in the reverse order, from the end to the beginning.

		if(bit) {
			ladderStep(X1, X3, Z3, X2, Z2);
//			Serial.print("X1: ");
//			Serial.println(X1);
//			Serial.print("V: ");
//			Serial.print(X3);
//			Serial.print(", ");
//			Serial.print(Z3);
//			Serial.print(", ");
//			Serial.print(X2);
//			Serial.print(", ");
//			Serial.println(Z2);
		} else {
			ladderStep(X1, X2, Z2, X3, Z3);
//			Serial.print("X1: ");
//			Serial.println(X1);
//			Serial.print("U: ");
//			Serial.print(X2);
//			Serial.print(", ");
//			Serial.print(Z2);
//			Serial.print(", ");
//			Serial.print(X3);
//			Serial.print(", ");
//			Serial.println(Z3);
		}
	}

	Serial.println();
	Serial.println("post for loop.");

/*	BigNumber daFinalNumber = Z2;
	for(unsigned short i = 0; i < 255; i += 1) {
		daFinalNumber = (daFinalNumber.pow(2)) % p;
		Serial.println(daFinalNumber);
	}
	Serial.println("Done with squarings loop.");
	daFinalNumber = daFinalNumber/((Z2.pow(21)) % p);
	Serial.println(daFinalNumber);
	daFinalNumber = (daFinalNumber*X2) % p;
	Serial.print("Da big boi: ");
	Serial.println(daFinalNumber);*/
}


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

	DABEEEEGBOI = (X2*daFinalNumber) % p;

	Serial.print("Da big boi: ");
	Serial.println(DABEEEEGBOI);

	encodeXCoord(DABEEEEGBOI);
}


void X25519KeyManagement::curve25519(char* n, char* xp) { // k is the independent, uniform, random secret key. It is an array of 32 bytes and is used as the scalar for the elliptic curve.
	n = clampAndDecodeScalar(n);

	Serial.print("Clamped and decoded scalar: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(n[i], HEX);
		Serial.print(' ');
	}
	Serial.println();
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(n[i], BIN);
		Serial.print(' ');
	}
	Serial.println('\n');

	BigNumber nInt = makeDec(n, 32); // Potential memory leak?

	Serial.print("Input scalar as decimal: ");
	Serial.println(nInt);
	Serial.println();

	xp = maskAndDecodeXCoord(xp);

	Serial.print("Masked and decoded x-coord: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(xp[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

	BigNumber xInit = makeDec(xp, 32); // Potential memory leak?

/*	encodeXCoord(xInit);
	Serial.print("encoded x-coord: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(DAFINALANSWER[i], HEX);
	}
	Serial.println('\n');*/

	Serial.print("Input x-coord as decimal: ");
	Serial.println(xInit);
	Serial.println();

	montgomeryLadder(nInt, n, xInit);

	reciprocal();

	Serial.print("encoded final x-coord: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(DAFINALANSWER[i], HEX);
	}
//	x = x % p;

//	Serial.println(x);
}

#endif