#ifndef ED25519_H
#define ED25519_H

#include "SHA512.h"
#include "multiprecision25519.h"
#include "multiprecision252ed.h"

#include <stdint.h>


struct Point {
	uint32_t X[16]; // INT_LENGTH_MULTI.
	uint32_t Y[16]; // INT_LENGTH_MULTI.
	uint32_t Z[16]; // INT_LENGTH_MULTI.
	uint32_t T[16]; // INT_LENGTH_MULTI.
};


class Ed25519SignatureAlgorithm {
private:
	SHA512Hash hash;
	MultiPrecisionArithmetic25519 math;
	MultiPrecisionArithmetic252ed order;

	static const constexpr unsigned short HASH_BYTES = 64;
	static const constexpr unsigned short KEY_BYTES = 32;
	static const constexpr unsigned short INT_LENGTH = 8;
	static const constexpr unsigned short INT_LENGTH_MULTI = 2*INT_LENGTH;
	static const constexpr unsigned short SIGNATURE_BYTES = 64;
	static const constexpr unsigned short BITS = 255;

	char h[HASH_BYTES]; // Hash buffer.

	char sByte[KEY_BYTES];
	uint32_t sByteInt[INT_LENGTH_MULTI]; // Secret scalar.
	char prefix[KEY_BYTES];

	Point P;
	Point Q;
	Point R;
	Point S;
	Point U;

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

	char bit;

	uint32_t A[INT_LENGTH_MULTI];
	uint32_t B[INT_LENGTH_MULTI];
	uint32_t C[INT_LENGTH_MULTI];
	uint32_t D[INT_LENGTH_MULTI];
	uint32_t E[INT_LENGTH_MULTI];
	uint32_t F[INT_LENGTH_MULTI];
	uint32_t G[INT_LENGTH_MULTI];
	uint32_t H[INT_LENGTH_MULTI];

	uint32_t d[INT_LENGTH_MULTI] = {0x00005203, 0x00006cee, 0x00002b6f, 0x0000fe73, 0x00008cc7, 0x00004079, 0x00007779, 0x0000e898, 0x00000070, 0x00000a4d, 0x00004141, 0x0000d8ab, 0x000075eb, 0x00004dca, 0x00001359, 0x000078a3}; // Curve constant.
	uint32_t d2[INT_LENGTH_MULTI] = {0x00002406, 0x0000d9dc, 0x000056df, 0x0000fce7, 0x0000198e, 0x000080f2, 0x0000eef3, 0x0000d130, 0x000000e0, 0x0000149a, 0x00008283, 0x0000b156, 0x0000ebd6, 0x00009b94, 0x000026b2, 0x0000f159}; // 2*d2 % p.

	Point emptyPoint; // Used as a conditional, unused point for contant-time.

	char encodeBytes[KEY_BYTES];

	char publicKey[KEY_BYTES];

	uint32_t hashInt[2*INT_LENGTH_MULTI];

	uint32_t r[INT_LENGTH_MULTI];

	uint32_t p[INT_LENGTH_MULTI] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed}; // (2^255) - 19.
	unsigned short counter;

	uint32_t oneInt[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};

	uint32_t complex[INT_LENGTH_MULTI] = {0x00002b83, 0x00002480, 0x00004fc1, 0x0000df0b, 0x00002b4d, 0x00000099, 0x00003dfb, 0x0000d7a7, 0x00002f43, 0x00001806, 0x0000ad2f, 0x0000e478, 0x0000c4ee, 0x00001b27, 0x00004a0e, 0x0000a0b0};

	void generateReadAndPruneHash(char[KEY_BYTES]);

	void ladderAdd(uint32_t*, uint32_t*, uint32_t*, uint32_t*);
	void ladderDouble();
	void Ed25519(uint32_t*, uint32_t*, uint32_t*, uint32_t*);

	void inverse();

	void encodePoint();

	void p38p();
	bool decodeXCoord();
public:
	Ed25519SignatureAlgorithm();
	~Ed25519SignatureAlgorithm();

	void initialize(char[KEY_BYTES], char[KEY_BYTES]);

	void sign(char[SIGNATURE_BYTES], char[KEY_BYTES], char[KEY_BYTES], char*, bool, unsigned long long);
	bool verify(char[KEY_BYTES], char*, char[SIGNATURE_BYTES], unsigned long long);
};


Ed25519SignatureAlgorithm::Ed25519SignatureAlgorithm() {

}


Ed25519SignatureAlgorithm::~Ed25519SignatureAlgorithm() {

}


void Ed25519SignatureAlgorithm::generateReadAndPruneHash(char* privateKey) {
	hash.hashBytes(h, privateKey, KEY_BYTES);

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		sByte[i] = h[(KEY_BYTES - 1) - i];
	}
	for(unsigned short i = KEY_BYTES; i < HASH_BYTES; i += 1) {
		prefix[i - KEY_BYTES] = h[i];
	}

//	sByte[0] &= 0x3f; // This is what is in the python implementation.
	sByte[0] &= 0x7f; // This is what is in the algorithm description.
	sByte[31] &= 0xf8;
	sByte[0] |= 0x40;

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		sByteInt[i] = sByte[i*2] << 8;
		sByteInt[i] |= sByte[(i*2) + 1];
	}
}


void Ed25519SignatureAlgorithm::ladderAdd(uint32_t* outX, uint32_t* outY, uint32_t* outZ, uint32_t* outT) {
	math.base16Sub(A, Q.Y, Q.X);
	math.base16Sub(B, P.Y, P.X);
	math.base16Mul(A, A, B);
	math.base16Add(B, Q.X, Q.Y);
	math.base16Add(C, P.X, P.Y);
	math.base16Mul(B, B, C);
	math.base16Mul(C, d2, Q.T);
	math.base16Mul(C, C, P.T);
	math.base16Add(D, Q.Z, Q.Z);
	math.base16Mul(D, D, P.Z);
	math.base16Sub(E, B, A);
	math.base16Sub(F, D, C);
	math.base16Add(G, C, D);
	math.base16Add(H, A, B);
	math.base16Mul(outX, E, F);
	math.base16Mul(outY, G, H);
	math.base16Mul(outZ, F, G);
	math.base16Mul(outT, E, H);
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


void Ed25519SignatureAlgorithm::Ed25519(uint32_t* PX, uint32_t* PY, uint32_t* PZ, uint32_t* PT) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		P.X[i] = PX[i];
		P.Y[i] = PY[i];
		P.Z[i] = PZ[i];
		P.T[i] = PT[i];
	}

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		Q.X[i] = NX[i];
		Q.Y[i] = NY[i];
		Q.Z[i] = NZ[i];
		Q.T[i] = NT[i];
	}

	for(unsigned short i = 0; i < BITS; i += 1) { // Potential optimization found in Crypto library for Arduino, Ed25519.cpp! Also, quick modulo using subtraction after additions and subtractions!
		bit = (sByte[(BITS - i)/8] >> (i % 8)) & 0x01;

		if(bit == 0x01) {
			ladderAdd(Q.X, Q.Y, Q.Z, Q.T);
		} else {
			ladderAdd(emptyPoint.X, emptyPoint.Y, emptyPoint.Z, emptyPoint.T); // This is not elegant, nor is it efficient, but it should be relatively constant time.
		}
		ladderDouble();
	}
}


void Ed25519SignatureAlgorithm::inverse() { // Copied directly from Daniel J. Bernstein.
	math.base16Mul(A, Q.Z, Q.Z);
	math.base16Mul(B, A, A);
	math.base16Mul(C, B, B);
	math.base16Mul(D, C, Q.Z);
	math.base16Mul(E, D, A);
	math.base16Mul(C, E, E);
	math.base16Mul(F, C, D);

	math.base16Mul(C, F, F);
	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(G, C, F);

	math.base16Mul(C, G, G);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(H, B, G);

	math.base16Mul(C, H, H);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 20; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(C, B, H);

	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(B, C, C);
		math.base16Mul(C, B, B);
	}
	math.base16Mul(P.X, C, G);

	math.base16Mul(C, P.X, P.X);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(P.Y, B, P.X);

	math.base16Mul(B, P.Y, P.Y);
	math.base16Mul(C, B, B);
	for(unsigned short i = 2; i < 100; i += 2) {
		math.base16Mul(B, C, C);
		math.base16Mul(C, B, B);
	}
	math.base16Mul(B, C, P.Y);

	math.base16Mul(C, B, B);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(C, B, P.X);

	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(B, C, C);
	math.base16Mul(Q.Z, B, E);
}


void Ed25519SignatureAlgorithm::encodePoint() {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		encodeBytes[(i*2)] = C[(INT_LENGTH_MULTI - 1) - i];
		encodeBytes[(i*2) + 1] = C[(INT_LENGTH_MULTI - 1) - i] >> 8;
	}

	encodeBytes[31] |= ((B[INT_LENGTH_MULTI - 1] & 0x00000001) << 7);
}


void Ed25519SignatureAlgorithm::p38p() { // Adapted from Daniel J. Bernstein. Calculates Q.X = (Q.Z)^((p+3)/8) % p.
	math.base16Mul(A, Q.Z, Q.Z);
	math.base16Mul(B, A, A);
	math.base16Mul(C, B, B);
	math.base16Mul(D, C, Q.Z);
	math.base16Mul(E, D, A);
	math.base16Mul(C, E, E);
	math.base16Mul(F, C, D);

	math.base16Mul(C, F, F);
	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(G, C, F);

	math.base16Mul(C, G, G);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(H, B, G);

	math.base16Mul(C, H, H);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 20; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(C, B, H);

	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(B, C, C);
		math.base16Mul(C, B, B);
	}
	math.base16Mul(P.X, C, G);

	math.base16Mul(C, P.X, P.X);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(P.Y, B, P.X);

	math.base16Mul(B, P.Y, P.Y);
	math.base16Mul(C, B, B);
	for(unsigned short i = 2; i < 100; i += 2) {
		math.base16Mul(B, C, C);
		math.base16Mul(C, B, B);
	}
	math.base16Mul(B, C, P.Y);

	math.base16Mul(C, B, B);
	math.base16Mul(B, C, C);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(C, B, B);
		math.base16Mul(B, C, C);
	}
	math.base16Mul(C, B, P.X);

	math.base16Mul(B, C, C);
	math.base16Mul(C, B, B);
	math.base16Mul(Q.X, C, A);
}


bool Ed25519SignatureAlgorithm::decodeXCoord() {
	counter = 0;
	while((Q.Y[counter] >= p[counter]) && (counter < INT_LENGTH_MULTI)) { // Not sure this will work. Not constant time because Q.Y is derived from the public key.
		counter += 1;
	}
	if(counter == INT_LENGTH_MULTI) {
		return false;
	}

	math.base16Mul(Q.Z, Q.Y, Q.Y); // Q.Y is being used for y.
	math.base16Mul(Q.Z, d, Q.Z);
	math.base16Add(Q.Z, oneInt, Q.Z);
	inverse();
	math.base16Mul(Q.T, Q.Y, Q.Y);
	math.base16Sub(Q.T, Q.T, oneInt);
	math.base16Mul(Q.Z, Q.T, Q.Z); // Q.Z is being used for x2.

	counter = 0;
	while((Q.Z[counter] == 0x00000000) && (counter < INT_LENGTH_MULTI)) {
		counter += 1;
	}
	if(counter == INT_LENGTH_MULTI) {
		if(bit) {
			return false;
		}

		for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
			Q.X[i] = 0x00000000; // Q.X is being used for x.
		}
		return true;
	}

	p38p();
	math.base16Mul(Q.T, Q.X, Q.X);
	math.base16Sub(Q.T, Q.T, Q.Z);
	counter = 0;
	while((Q.T[counter] == 0x00000000) && (counter < INT_LENGTH_MULTI)) {
		counter += 1;
	}
	if(counter != INT_LENGTH_MULTI) {
		math.base16Mul(Q.X, Q.X, complex); // May want to double check complex.
	}
	math.base16Mul(Q.T, Q.X, Q.X);
	math.base16Sub(Q.T, Q.T, Q.Z);
	counter = 0;
	while((Q.T[counter] == 0x00000000) && (counter < INT_LENGTH_MULTI)) {
		counter += 1;
	}
	if(counter != INT_LENGTH_MULTI) {
		return false;
	}

	if((Q.X[15] & 0x00000001) != bit) {
		math.base16Sub(Q.X, p, Q.X);
	}

	return true;
}


void Ed25519SignatureAlgorithm::initialize(char* publicKeyOut, char* privateKey) {
	generateReadAndPruneHash(privateKey);

	Ed25519(BX, BY, BZ, BT);

	inverse();
	math.base16Mul(B, Q.X, Q.Z);
	math.base16Mul(C, Q.Y, Q.Z);

	encodePoint();

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		publicKey[i] = encodeBytes[i];
		publicKeyOut[i] = publicKey[i];
	}
}


void Ed25519SignatureAlgorithm::sign(char* signatureOut, char* publicKeyInOut, char* privateKeyIn, char* message, bool createPublicKey, unsigned long long messageBytes = KEY_BYTES) {
	if(createPublicKey == true) {
		initialize(publicKeyInOut, privateKeyIn);
	} else {
		generateReadAndPruneHash(privateKeyIn);

		for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
			publicKey[i] = publicKeyInOut[i];
		}
	}

	char* prefixMsg = new char[KEY_BYTES + messageBytes];
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		prefixMsg[i] = prefix[i];
	}
	for(unsigned long long i = 0; i < messageBytes; i += 1) {
		prefixMsg[i + KEY_BYTES] = message[i];
	}
	hash.hashBytes(h, prefixMsg, (KEY_BYTES + messageBytes));
	delete[] prefixMsg;

	for(unsigned short i = 0; i < (2*INT_LENGTH_MULTI); i += 1) {
		hashInt[i] = h[(HASH_BYTES - 1) - (i*2)] << 8;
		hashInt[i] |= h[(HASH_BYTES - 1) - ((i*2) + 1)];
	}

	order.base16Mod(r, hashInt);

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		sByte[i*2] = r[i] >> 8;
		sByte[(i*2) + 1] = r[i];
	}

	Ed25519(BX, BY, BZ, BT);

	inverse();
	math.base16Mul(B, Q.X, Q.Z);
	math.base16Mul(C, Q.Y, Q.Z);

	encodePoint();

	char* RAMsg = new char[(2*KEY_BYTES) + messageBytes];
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		RAMsg[i] = encodeBytes[i];
		RAMsg[i + KEY_BYTES] = publicKey[i];
	}
	for(unsigned long long i = 0; i < messageBytes; i += 1) {
		RAMsg[i + (2*KEY_BYTES)] = message[i];
	}
	hash.hashBytes(h, RAMsg, ((2*KEY_BYTES) + messageBytes));
	delete[] RAMsg;

	for(unsigned short i = 0; i < (2*INT_LENGTH_MULTI); i += 1) {
		hashInt[i] = h[(HASH_BYTES - 1) - (i*2)] << 8;
		hashInt[i] |= h[(HASH_BYTES - 1) - ((i*2) + 1)];
	}

	order.base16Mod(B, hashInt);

	order.base16Mul(C, B, sByteInt);
	order.base16Add(C, r, C);

	for(unsigned short i = 0; i < SIGNATURE_BYTES/2; i += 1) {
		signatureOut[i] = encodeBytes[i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		signatureOut[(i*2) + (SIGNATURE_BYTES/2)] = C[(INT_LENGTH_MULTI - 1) - i];
		signatureOut[(i*2) + 1 + (SIGNATURE_BYTES/2)] = C[(INT_LENGTH_MULTI - 1) - i] >> 8;
	}
}


bool Ed25519SignatureAlgorithm::verify(char* publicKey, char* message, char* signature, unsigned long long messageBytes = KEY_BYTES) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		Q.Y[i] = publicKey[(KEY_BYTES - 1) - (i*2)] << 8;
		Q.Y[i] |= publicKey[(KEY_BYTES - 1) - ((i*2) + 1)];
	}
	Q.Y[0] &= 0x00007fff;
	bit = publicKey[31] >> 7;

	if(!decodeXCoord()) {
		return false;
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		R.X[i] = Q.X[i];
		R.Y[i] = Q.Y[i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		R.Z[i] = oneInt[i];
	}
	math.base16Mul(R.T, Q.X, Q.Y); // R is now the public key in point form (aka. A). This could be optimized.



	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		Q.Y[i] = signature[((SIGNATURE_BYTES/2) - 1) - (i*2)] << 8;
		Q.Y[i] |= signature[((SIGNATURE_BYTES/2) - 1) - ((i*2) + 1)];
	}
	Q.Y[0] &= 0x00007fff;
	bit = signature[31] >> 7;

	if(!decodeXCoord()) {
		return false;
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		S.X[i] = Q.X[i];
		S.Y[i] = Q.Y[i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		S.Z[i] = oneInt[i];
	}
	math.base16Mul(S.T, Q.X, Q.Y); // S is now R.



	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		sByte[i] = signature[(SIGNATURE_BYTES - 1) - i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		sByteInt[i] = sByte[i*2] << 8;
		sByteInt[i] |= sByte[(i*2) + 1];
	}
	counter = 0;
	while((sByteInt[counter] >= p[counter]) && (counter < INT_LENGTH_MULTI)) { // Not sure this will work. Not constant time because sByteInt is derived from the signature.
		counter += 1;
	}
	if(counter == INT_LENGTH_MULTI) {
		return false;
	}



	char* RAMsg = new char[(2*KEY_BYTES) + messageBytes];
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		RAMsg[i] = signature[i];
		RAMsg[i + KEY_BYTES] = publicKey[i];
	}
	for(unsigned long long i = 0; i < messageBytes; i += 1) {
		RAMsg[i + (2*KEY_BYTES)] = message[i];
	}
	hash.hashBytes(h, RAMsg, ((2*KEY_BYTES) + messageBytes));
	delete[] RAMsg;

	for(unsigned short i = 0; i < (2*INT_LENGTH_MULTI); i += 1) {
		hashInt[i] = h[(HASH_BYTES - 1) - (i*2)] << 8;
		hashInt[i] |= h[(HASH_BYTES - 1) - ((i*2) + 1)];
	}

	order.base16Mod(r, hashInt); // r is storing h.



	Ed25519(BX, BY, BZ, BT);

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		U.X[i] = Q.X[i];
		U.Y[i] = Q.Y[i];
		U.Z[i] = Q.Z[i];
		U.T[i] = Q.T[i]; // Might not be necessary.
	} // U is storing sB.



	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		sByte[i*2] = r[i] >> 8;
		sByte[(i*2) + 1] = r[i]; // I think this will work...
	}

	Ed25519(R.X, R.Y, R.Z, R.T); // Q is storing hA.



	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		P.X[i] = S.X[i];
		P.Y[i] = S.Y[i];
		P.Z[i] = S.Z[i];
		P.T[i] = S.T[i];
	}

	ladderAdd(Q.X, Q.Y, Q.Z, Q.T); // Now check if U and Q are equal.

	math.base16Mul(B, U.X, Q.Z);
	math.base16Mul(C, Q.X, U.Z);
	math.base16Sub(B, B, C);

	counter = 0;
	while((B[counter] == 0x00000000) && (counter < INT_LENGTH_MULTI)) {
		counter += 1;
	}
	if(counter != INT_LENGTH_MULTI) {
		return false;
	}

	math.base16Mul(B, U.Y, Q.Z);
	math.base16Mul(C, Q.Y, U.Z);
	math.base16Sub(B, B, C);

	counter = 0;
	while((B[counter] == 0x00000000) && (counter < INT_LENGTH_MULTI)) {
		counter += 1;
	}
	if(counter != INT_LENGTH_MULTI) {
		return false;
	}

	return true;
}

#endif