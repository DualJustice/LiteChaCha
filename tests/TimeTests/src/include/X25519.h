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
	BigNumber p = "57896044618658097711785492504343953926634992332820282019728792003956564819949"; // (2^255) - 19

	char* bytesLittleEndian(char*, unsigned short);
	char* transformScalar(char*);
	BigNumber makeInt(char*, unsigned short);
	void ladderStep(BigNumber, BigNumber&, BigNumber&, BigNumber&, BigNumber&);
	BigNumber curve25519(BigNumber, char*, BigNumber);
public:
	X25519KeyManagement();
	~X25519KeyManagement();

	void startBigNum();

	void createPubKey(char*);
};


X25519KeyManagement::X25519KeyManagement() {

}


X25519KeyManagement::~X25519KeyManagement() {

}


void X25519KeyManagement::startBigNum() {
	BigNumber::begin();
}


char* X25519KeyManagement::bytesLittleEndian(char* b, unsigned short c) { // Can optimize in the future by removing unnecessary copies.
	char* bLE = new char[c];

	for(unsigned short i = 0; i < c; i += 1) {
		bLE[i] = b[c - i];
	}
	for(unsigned short i = 0; i < c; i += 1) {
		b[i] = bLE[i];
	}

	delete[] bLE;
	return b;
}


char* X25519KeyManagement::transformScalar(char* n) {
	n[0] &= 0xf8;
	n[31] &= 0x7f;
	n[31] |= 0x40;

	n = bytesLittleEndian(n, 32);

	return n;
}


BigNumber X25519KeyManagement::makeInt(char* b, unsigned short c) {
	BigNumber bigInt; // Potential memory leak?

	for(unsigned short i = 0; i < c; i += 1) {
		bigInt += (b[i])*(256^i);
	}

	return bigInt;
}


void X25519KeyManagement::ladderStep(BigNumber X1, BigNumber& UV0, BigNumber& UV1, BigNumber& UV2, BigNumber& UV3) {
/*
X2 = UV[0]
Z2 = UV[1]
X3 = UV[2]
Z3 = UV[3]
*/
	T1 = UV0 + UV1;			// 1
//	Serial.print("T1: ");
//	Serial.println(T1);
	T2 = UV0 - UV1;			// 2
//	Serial.print("T2: ");
//	Serial.println(T2);
	T3 = UV2 + UV3;			// 3
//	Serial.print("T3: ");
//	Serial.println(T3);
	T4 = UV2 - UV3;			// 4
//	Serial.print("T4: ");
//	Serial.println(T4);
	T5 = T1.pow(2);			// 5
//	Serial.print("T5: ");
//	Serial.println(T5);
	T6 = T2.pow(2);			// 6
//	Serial.print("T6: ");
//	Serial.println(T6);
	T2 *= T3;				// 7
//	Serial.print("T2: ");
//	Serial.println(T2);
	T1 *= T4;				// 8
//	Serial.print("T1: ");
//	Serial.println(T1);
	T1 += T2;				// 9
//	Serial.print("T1: ");
//	Serial.println(T1);
	T2 = T1 - T2;			// 10
//	Serial.print("T2: ");
//	Serial.println(T2);
	UV2 = (T1.pow(2)) % p;	// 11
//	Serial.print("UV2: ");
//	Serial.println(UV2);
	T2 = T2.pow(2);			// 12
//	Serial.print("T2: ");
//	Serial.println(T2);
	UV3 = (T2*X1) % p;		// 13
//	Serial.print("UV3: ");
//	Serial.println(UV3);
	UV0 = (T5*T6) % p;		// 14
//	Serial.print("UV0: ");
//	Serial.println(UV0);
	T5 -= T6;				// 15
//	Serial.print("T5: ");
//	Serial.println(T5);
	T1 = A24*T5;			// 16
//	Serial.print("T1: ");
//	Serial.println(T1);
	T6 += T1;				// 17
//	Serial.print("T6: ");
//	Serial.println(T6);
	UV1 = (T5*T6) % p;		// 18
//	Serial.print("UV1: ");
//	Serial.println(UV1);
}


BigNumber X25519KeyManagement::curve25519(BigNumber nInt, char* n, BigNumber xInit) {
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

	for(unsigned short i = 254; i > 0; i -= 1) {
		bit = (n[i/8] >> (i % 8)) & 0x01;

//		Serial.print("bit: ");
//		Serial.println(bit);

		if(bit) {
			ladderStep(X1, X3, Z3, X2, Z2);
//			Serial.print("X1: ");
//			Serial.println(X1);
			Serial.print("V: ");
			Serial.print(X3);
			Serial.print(", ");
			Serial.print(Z3);
			Serial.print(", ");
			Serial.print(X2);
			Serial.print(", ");
			Serial.println(Z2);
		} else {
			ladderStep(X1, X2, Z2, X3, Z3);
//			Serial.print("X1: ");
//			Serial.println(X1);
			Serial.print("U: ");
			Serial.print(X2);
			Serial.print(", ");
			Serial.print(Z2);
			Serial.print(", ");
			Serial.print(X3);
			Serial.print(", ");
			Serial.println(Z3);
		}
	}

	Serial.println("post for loop.");

	return X2/Z2;
}


void X25519KeyManagement::createPubKey(char* n) { // k is the independent, uniform, random secret key. It is an array of 32 bytes and is used as the scalar for the elliptic curve.
	BigNumber xInit = "34426434033919594451155107781188821651316167215306631574996226621102155684838";

	n = transformScalar(n);
	BigNumber nInt = makeInt(n, 32);
	BigNumber x = curve25519(nInt, n, xInit);
	x = x % p;

	Serial.println(x);
}

#endif