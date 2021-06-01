#ifndef X25519V2_H
#define X25519V2_H

#include "Arduino.h" // DELETE ME!
#include "HardwareSerial.h" // DELETE ME!

#include <stdint.h>


//https://datatracker.ietf.org/doc/html/rfc7748
//https://datatracker.ietf.org/doc/html/rfc8031
//https://cr.yp.to/ecdh/curve25519-20060209.pdf
//https://eprint.iacr.org/2017/293.pdf Basics of the Montgomery Ladder.
//https://tches.iacr.org/index.php/TCHES/article/view/8681/8240 <-------------------------------------------------- I don't believe this is applicable. I think curve25519 is a Montgomery curve, not a Weierstrass curve.
//https://eprint.iacr.org/eprint-bin/getfile.pl?entry=2019/1410&version=20191205:080951&file=1410.pdf <---------------------- This is maybe worth looking into.
//https://www.researchgate.net/publication/277940984_High-speed_Curve25519_on_8-bit_16-bit_and_32-bit_microcontrollers


class X25519KeyManagement {
private:
	T1;
	T2;
	T3;
	T4;
	T5;
	T6;
	X1;
	X3;
	Z3;
	X2;
	Z2;

	char bit;

//	U[4] = {X2, Z2, X3, Z3};
//	V[4] = {X3, Z3, X2, Z2};

	static const constexpr unsigned long A24 = 121665; // (486662 - 2)/4
	p = "57896044618658097711785492504343953926634992332820282019728792003956564819949"; // (2^255) - 19

	char* bytesLittleEndian(char*, unsigned short);
	char* transformScalar(char*);
	makeInt(char*, unsigned short);
	void ladderStep(, *);
	curve25519(, char*, );
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
	::begin();
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


X25519KeyManagement::makeInt(char* b, unsigned short c) {
	bigInt; // Potential memory leak?

	for(unsigned short i = 0; i < c; i += 1) {
		bigInt += (b[i])*(256^i);
	}

	return bigInt;
}


void X25519KeyManagement::ladderStep(X1, * UV) {
/*
X2 = UV[0]
Z2 = UV[1]
X3 = UV[2]
Z3 = UV[3]
*/
	T1 = UV[0] + UV[1];	// 1
	T2 = UV[0] - UV[1];	// 2
	T3 = UV[2] + UV[3];	// 3
	T4 = UV[2] - UV[3];	// 4
	T5 = T1^2;			// 5
	T6 = T2^2;			// 6
	T2 *= T3;			// 7
	T1 *= T4;			// 8
	T1 += T2;			// 9
	T2 = T1 - T2;		// 10
	UV[2] = T1^2;		// 11
	T2 = T2^2;			// 12
	UV[3] = T2*X1;		// 13
	UV[0] = T5*T6;		// 14
	T5 -= T6;			// 15
	T1 = A24*T5;		// 16
	T6 += T1;			// 17
	UV[1] = T5*T6;		// 18
}


X25519KeyManagement::curve25519(nInt, char* n, xInit) {
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
U[4] = {X2, Z2, X3, Z3};
V[4] = {X3, Z3, X2, Z2};
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

		Serial.print("bit: ");
		Serial.println(bit);

		if(bit) {
			ladderStep(X1, V);
			Serial.print("X1: ");
			Serial.println(X1);
			Serial.print("V: ");
			for(unsigned short j = 0; j < 4; j += 1) {
				Serial.print(V[i]);
				Serial.print(", ");
				Serial.println();
			}
		} else {
			ladderStep(X1, U);
			Serial.print("X1: ");
			Serial.println(X1);
			Serial.print("U: ");
			for(unsigned short j = 0; j < 4; j += 1) {
				Serial.print(U[i]);
				Serial.print(", ");
				Serial.println();
			}
		}
	}

	Serial.println("post for loop.");

	return X2/Z2;
}


void X25519KeyManagement::createPubKey(char* n) { // k is the independent, uniform, random secret key. It is an array of 32 bytes and is used as the scalar for the elliptic curve.
	unsigned short xInit = 9;

	n = transformScalar(n);
	nInt = makeInt(n, 32);
	x = curve25519(nInt, n, xInit);
	x = x % p;

	Serial.println(x);
}


/*
Here, the "bits" parameter should be set to 255 for X25519.

	def decodeLittleEndian(b, bits):											THIS READS BYTES IN LITTLE ENDIAN, RETURNS A 32 BYTE NUMBER (NOT AN ARRAY)
		return sum([b[i] << 8*i for i in range((bits+7)/8)])

	def decodeUCoordinate(u, bits):												THIS TAKES AN X COORD, SHORTENS IT IF POSSIBLE, AND READS IT IN LITTLE ENDIAN
		u_list = [ord(b) for b in u]
		# Ignore any unused bits.
		if bits % 8:
			u_list[-1] &= (1<<(bits%8))-1
		return decodeLittleEndian(u_list, bits)

	def encodeUCoordinate(u, bits):												TAKES X COORD, MODULOS IT WITH P, AND WRITES IT IN LITTLE ENDIAN
		u = u % p
		return ''.join([chr((u >> 8*i) & 0xff) for i in range((bits+7)/8)])


Scalars are assumed to be randomly generated bytes.  For X25519, in
order to decode 32 random bytes as an integer scalar, set the three
least significant bits of the first byte and the most significant bit
of the last to zero, set the second most significant bit of the last
byte to 1 and, finally, decode as little-endian.  This means that the
resulting integer is of the form 2^254 plus eight times a value
between 0 and 2^251 - 1 (inclusive).

def decodeScalar25519(k):														DOES FANCY STUFF TO RANDOM SCALAR, THEN MAKES IT INTO LITTLE ENDIAN 32 BYTE NUMBER
	k_list = [ord(b) for b in k]
	k_list[0] &= 248
	k_list[31] &= 127
	k_list[31] |= 64
	return decodeLittleEndian(k_list, 255)


To implement the X25519(k, u) function (where k is
the scalar and u is the u-coordinate), first decode k and u and then
perform the following procedure, which is taken from [curve25519] and
based on formulas from [montgomery].  All calculations are performed
in GF(p), i.e., they are performed modulo p.  The constant a24 is
(486662 - 2) / 4 = 121665 for curve25519/X25519.
																				I GUESS JUST MAKE EVERYTHING (ALL # AND MORE) BIG NUMS FOR NOW
	x_1 = u																		X_1 IS A # AND IS (CONST) EQUAL TO 9
	x_2 = 1																		X_2 IS A #
	z_2 = 0																		Z_2 IS A #
	x_3 = u																		X_3 IS A # AND IS INITIALLY EQUAL TO 9
	z_3 = 1																		Z_3 IS A #
	swap = 0																	SWAP IS NOT A BIG NUM!

	For t = bits-1 down to 0:													FOR T = 254 TO 0 {
		k_t = (k >> t) & 1														K_T IS A #, K IS AN ARRAY OF 32 BYTES WHICH WILL BE MANIPULATED TO FIND THE T'TH (?) BIT
		swap ^= k_t
		// Conditional swap; see text below.
		(x_2, x_3) = cswap(swap, x_2, x_3)
		(z_2, z_3) = cswap(swap, z_2, z_3)
		swap = k_t

		A = x_2 + z_2
		AA = A^2
		B = x_2 - z_2
		BB = B^2
		E = AA - BB
		C = x_3 + z_3
		D = x_3 - z_3
		DA = D * A
		CB = C * B
		x_3 = (DA + CB)^2
		z_3 = x_1 * (DA - CB)^2
		x_2 = AA * BB
		z_2 = E * (AA + a24 * E)

	// Conditional swap; see text below.
	(x_2, x_3) = cswap(swap, x_2, x_3)
	(z_2, z_3) = cswap(swap, z_2, z_3)
	Return x_2 * (z_2^(p - 2))

	(Note that these formulas are slightly different from Montgomery's
	original paper.  Implementations are free to use any correct
	formulas.)

	Finally, encode the resulting value as 32 or 56 bytes in little-
	endian order.  For X25519, the unused, most significant bit MUST be
	zero.

	The cswap function SHOULD be implemented in constant time (i.e.,
	independent of the swap argument).  For example, this can be done as
	follows:

	cswap(swap, x_2, x_3):
		dummy = mask(swap) AND (x_2 XOR x_3)
		x_2 = x_2 XOR dummy
		x_3 = x_3 XOR dummy
		Return (x_2, x_3)

	Where mask(swap) is the all-1 or all-0 word of the same length as x_2
	and x_3, computed, e.g., as mask(swap) = 0 - swap.
*/




#endif