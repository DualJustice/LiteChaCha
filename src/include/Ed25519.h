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

	char hashBuffer[HASH_BYTES];

	char scalarByte[KEY_BYTES];
	uint32_t scalarInt[INT_LENGTH_MULTI];

	char prefix[KEY_BYTES];

//	Base point:
	const uint32_t BaseX[INT_LENGTH_MULTI] = {0x00002169, 0x000036d3, 0x0000cd6e, 0x000053fe, 0x0000c0a4, 0x0000e231, 0x0000fdd6, 0x0000dc5c, 0x0000692c, 0x0000c760, 0x00009525, 0x0000a7b2, 0x0000c956, 0x00002d60, 0x00008f25, 0x0000d51a};
	const uint32_t BaseY[INT_LENGTH_MULTI] = {0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006666, 0x00006658};
	const uint32_t BaseT[INT_LENGTH_MULTI] = {0x00006787, 0x00005f0f, 0x0000d78b, 0x00007665, 0x000066ea, 0x00004e8e, 0x000064ab, 0x0000e37d, 0x000020f0, 0x00009f80, 0x00007751, 0x000052f5, 0x00006dde, 0x00008ab3, 0x0000a5b7, 0x0000dda3};
//	BaseZ = 1, using oneInt.

	const uint32_t oneInt[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000001};

//	Working points:
	Point ptP;
	Point ptQ;

//	Storage points:
	Point ptA;
	Point ptR;
	Point ptS;

	Point emptyPoint; // Used as a conditional, unused point to conserve contant-time.

	char bit;

//	Working variables (registers):
//	Used primarily in Ed25519:
	uint32_t regA[INT_LENGTH_MULTI];
	uint32_t regB[INT_LENGTH_MULTI];
	uint32_t regC[INT_LENGTH_MULTI];
	uint32_t regD[INT_LENGTH_MULTI];
	uint32_t regE[INT_LENGTH_MULTI];
	uint32_t regF[INT_LENGTH_MULTI];
	uint32_t regG[INT_LENGTH_MULTI];
	uint32_t regH[INT_LENGTH_MULTI];

//	Used primarily in Ed25519 & commonInverse:
	uint32_t regI[INT_LENGTH_MULTI];
	uint32_t regJ[INT_LENGTH_MULTI];

//	Used primarily to store hashes as integers mod L:
	uint32_t regK[INT_LENGTH_MULTI];

	const uint32_t d[INT_LENGTH_MULTI] = {0x00005203, 0x00006cee, 0x00002b6f, 0x0000fe73, 0x00008cc7, 0x00004079, 0x00007779, 0x0000e898, 0x00000070, 0x00000a4d, 0x00004141, 0x0000d8ab, 0x000075eb, 0x00004dca, 0x00001359, 0x000078a3}; // Curve constant.
	const uint32_t d2[INT_LENGTH_MULTI] = {0x00002406, 0x0000d9dc, 0x000056df, 0x0000fce7, 0x0000198e, 0x000080f2, 0x0000eef3, 0x0000d130, 0x000000e0, 0x0000149a, 0x00008283, 0x0000b156, 0x0000ebd6, 0x00009b94, 0x000026b2, 0x0000f159}; // (2*d) % p.

	uint32_t inverseX[INT_LENGTH_MULTI];
	uint32_t inverseY[INT_LENGTH_MULTI];
	char encodeBytes[KEY_BYTES];

	char publicKey[KEY_BYTES];

	uint32_t hashInt[2*INT_LENGTH_MULTI];

	const uint32_t p[INT_LENGTH_MULTI] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed}; // (2^255) - 19.
	const uint32_t L[INT_LENGTH_MULTI] = {0x00001000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000014de, 0x0000f9de, 0x0000a2f7, 0x00009cd6, 0x00005812, 0x0000631a, 0x00005cf5, 0x0000d3ed}; // Order of the prime-order subgroup.

	const uint32_t complex[INT_LENGTH_MULTI] = {0x00002b83, 0x00002480, 0x00004fc1, 0x0000df0b, 0x00002b4d, 0x00000099, 0x00003dfb, 0x0000d7a7, 0x00002f43, 0x00001806, 0x0000ad2f, 0x0000e478, 0x0000c4ee, 0x00001b27, 0x00004a0e, 0x0000a0b0}; // sqrt(-1) = 2^(((2^255) - 20)/4) % p.

	void generateReadAndPruneHash(char[KEY_BYTES]);

	void ladderAdd(uint32_t*, uint32_t*, uint32_t*, uint32_t*);
	void ladderDouble();
	void Ed25519(const uint32_t*, const uint32_t*, const uint32_t*, const uint32_t*);
	void quickEd25519(const uint32_t*, const uint32_t*, const uint32_t*, const uint32_t*);

	void commonInverse();
	void inverse();
	void p38p();

	void encodePoint();

	void hashModOrder(uint32_t*, char*, unsigned long long);

	bool greaterThanOrEqualToP(uint32_t*);
	bool equalToZero(uint32_t*);
	bool greaterThanOrEqualToOrder(uint32_t*);

	bool recoverXCoord();

	bool decodePoint(uint32_t*, uint32_t*, uint32_t*, uint32_t*, char*);
public:
	void initialize(char[KEY_BYTES], char[KEY_BYTES]);

	void sign(char[SIGNATURE_BYTES], char[KEY_BYTES], char[KEY_BYTES], char*, bool, unsigned long long);
	bool verify(char[KEY_BYTES], char*, char[SIGNATURE_BYTES], unsigned long long);
};


inline void Ed25519SignatureAlgorithm::generateReadAndPruneHash(char* privateKey) {
	hash.hashBytes(hashBuffer, privateKey, KEY_BYTES);

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		scalarByte[i] = hashBuffer[(KEY_BYTES - 1) - i];
	}
	for(unsigned short i = KEY_BYTES; i < HASH_BYTES; i += 1) {
		prefix[i - KEY_BYTES] = hashBuffer[i];
	}

	scalarByte[0] &= 0x7f;
	scalarByte[31] &= 0xf8;
	scalarByte[0] |= 0x40;

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		scalarInt[i] = scalarByte[i*2] << 8;
		scalarInt[i] |= scalarByte[(i*2) + 1];
	}
}


inline void Ed25519SignatureAlgorithm::ladderAdd(uint32_t* outX, uint32_t* outY, uint32_t* outZ, uint32_t* outT) {
	math.base16Sub(regA, ptQ.Y, ptQ.X);
	math.base16Sub(regB, ptP.Y, ptP.X);
	math.base16Mul(regA, regA, regB);
	math.base16Add(regB, ptQ.X, ptQ.Y);
	math.base16Add(regC, ptP.X, ptP.Y);
	math.base16Mul(regB, regB, regC);
	math.base16Mul(regC, d2, ptQ.T);
	math.base16Mul(regC, regC, ptP.T);
	math.base16Add(regD, ptQ.Z, ptQ.Z);
	math.base16Mul(regD, regD, ptP.Z);
	math.base16Sub(regE, regB, regA);
	math.base16Sub(regF, regD, regC);
	math.base16Add(regG, regC, regD);
	math.base16Add(regH, regA, regB);
	math.base16Mul(outX, regE, regF);
	math.base16Mul(outY, regG, regH);
	math.base16Mul(outZ, regF, regG);
	math.base16Mul(outT, regE, regH);
}


inline void Ed25519SignatureAlgorithm::ladderDouble() {
	math.base16Mul(regA, ptP.X, ptP.X);
	math.base16Mul(regB, ptP.Y, ptP.Y);
	math.base16Mul(regC, ptP.Z, ptP.Z);
	math.base16Add(regC, regC, regC);
	math.base16Add(regD, regA, regB);
	math.base16Add(regE, ptP.X, ptP.Y);
	math.base16Mul(regE, regE, regE);
	math.base16Sub(regE, regD, regE);
	math.base16Sub(regF, regA, regB);
	math.base16Add(regG, regC, regF);
	math.base16Mul(ptP.X, regE, regG);
	math.base16Mul(ptP.Y, regD, regF);
	math.base16Mul(ptP.Z, regF, regG);
	math.base16Mul(ptP.T, regD, regE);
}


inline void Ed25519SignatureAlgorithm::Ed25519(const uint32_t* PX, const uint32_t* PY, const uint32_t* PZ, const uint32_t* PT) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		ptP.X[i] = PX[i];
		ptP.Y[i] = PY[i];
		ptP.Z[i] = PZ[i];
		ptP.T[i] = PT[i];
	}

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) { // ptQ = Neutral element.
		ptQ.X[i] = 0x00000000;
		ptQ.Y[i] = oneInt[i];
		ptQ.Z[i] = oneInt[i];
		ptQ.T[i] = 0x00000000;
	}

	for(unsigned short i = 0; i < BITS; i += 1) {
		bit = (scalarByte[(BITS - i)/8] >> (i % 8)) & 0x01;

		if(bit == 0x01) {
			ladderAdd(ptQ.X, ptQ.Y, ptQ.Z, ptQ.T);
		} else {
			ladderAdd(emptyPoint.X, emptyPoint.Y, emptyPoint.Z, emptyPoint.T); // This is not elegant, nor is it efficient, but it should be relatively constant time.
		}
		ladderDouble();
	}
}


inline void Ed25519SignatureAlgorithm::commonInverse() {
	math.base16Mul(regA, ptQ.Z, ptQ.Z);
	math.base16Mul(regB, regA, regA);
	math.base16Mul(regC, regB, regB);
	math.base16Mul(regD, regC, ptQ.Z);
	math.base16Mul(regE, regD, regA);
	math.base16Mul(regC, regE, regE);
	math.base16Mul(regF, regC, regD);

	math.base16Mul(regC, regF, regF);
	math.base16Mul(regB, regC, regC);
	math.base16Mul(regC, regB, regB);
	math.base16Mul(regB, regC, regC);
	math.base16Mul(regC, regB, regB);
	math.base16Mul(regG, regC, regF);

	math.base16Mul(regC, regG, regG);
	math.base16Mul(regB, regC, regC);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(regC, regB, regB);
		math.base16Mul(regB, regC, regC);
	}
	math.base16Mul(regH, regB, regG);

	math.base16Mul(regC, regH, regH);
	math.base16Mul(regB, regC, regC);
	for(unsigned short i = 2; i < 20; i += 2) {
		math.base16Mul(regC, regB, regB);
		math.base16Mul(regB, regC, regC);
	}
	math.base16Mul(regC, regB, regH);

	math.base16Mul(regB, regC, regC);
	math.base16Mul(regC, regB, regB);
	for(unsigned short i = 2; i < 10; i += 2) {
		math.base16Mul(regB, regC, regC);
		math.base16Mul(regC, regB, regB);
	}
	math.base16Mul(regI, regC, regG);

	math.base16Mul(regC, regI, regI);
	math.base16Mul(regB, regC, regC);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(regC, regB, regB);
		math.base16Mul(regB, regC, regC);
	}
	math.base16Mul(regJ, regB, regI);

	math.base16Mul(regB, regJ, regJ);
	math.base16Mul(regC, regB, regB);
	for(unsigned short i = 2; i < 100; i += 2) {
		math.base16Mul(regB, regC, regC);
		math.base16Mul(regC, regB, regB);
	}
	math.base16Mul(regB, regC, regJ);

	math.base16Mul(regC, regB, regB);
	math.base16Mul(regB, regC, regC);
	for(unsigned short i = 2; i < 50; i += 2) {
		math.base16Mul(regC, regB, regB);
		math.base16Mul(regB, regC, regC);
	}
	math.base16Mul(regC, regB, regI);

	math.base16Mul(regB, regC, regC);
	math.base16Mul(regC, regB, regB);
}


inline void Ed25519SignatureAlgorithm::inverse() { // Copied directly from Daniel J. Bernstein.
	commonInverse();

	math.base16Mul(regB, regC, regC);
	math.base16Mul(regC, regB, regB);
	math.base16Mul(regB, regC, regC);
	math.base16Mul(ptQ.Z, regB, regE);
}


inline void Ed25519SignatureAlgorithm::encodePoint() {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		encodeBytes[(i*2)] = inverseY[(INT_LENGTH_MULTI - 1) - i];
		encodeBytes[(i*2) + 1] = inverseY[(INT_LENGTH_MULTI - 1) - i] >> 8;
	}

	encodeBytes[31] |= ((inverseX[INT_LENGTH_MULTI - 1] & 0x00000001) << 7);
}


inline void Ed25519SignatureAlgorithm::hashModOrder(uint32_t* intOut, char* message, unsigned long long messageBytes) {
	hash.hashBytes(hashBuffer, message, messageBytes);

	for(unsigned short i = 0; i < (2*INT_LENGTH_MULTI); i += 1) {
		hashInt[i] = hashBuffer[(HASH_BYTES - 1) - (i*2)] << 8;
		hashInt[i] |= hashBuffer[(HASH_BYTES - 1) - ((i*2) + 1)];
	}

	order.base16Mod(intOut, hashInt);
}


inline bool Ed25519SignatureAlgorithm::greaterThanOrEqualToP(uint32_t* a) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		if(a[i] > p[i]) {
			return true;
		}
		if(a[i] < p[i]) {
			return false;
		}
	}

	return true;
}


inline bool Ed25519SignatureAlgorithm::equalToZero(uint32_t* a) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		if(a[i] != 0x00000000) {
			return false;
		}
	}

	return true;
}


inline void Ed25519SignatureAlgorithm::p38p() { // Adapted from Daniel J. Bernstein. Calculates ptQ.X = (ptQ.Z)^((p+3)/8) % p.
	commonInverse();

	math.base16Mul(ptQ.X, regC, regA);
}


inline bool Ed25519SignatureAlgorithm::recoverXCoord() {
	if(greaterThanOrEqualToP(ptQ.Y)) {
		return false;
	}

	math.base16Mul(ptQ.Z, ptQ.Y, ptQ.Y); // ptQ.Y is storing y.
	math.base16Mul(ptQ.Z, d, ptQ.Z);
	math.base16Add(ptQ.Z, oneInt, ptQ.Z);
	inverse();
	math.base16Mul(ptQ.T, ptQ.Y, ptQ.Y);
	math.base16Sub(ptQ.T, ptQ.T, oneInt);
	math.base16Mul(ptQ.Z, ptQ.T, ptQ.Z); // ptQ.Z is storing x2.

	if(equalToZero(ptQ.Z)) {
		if(bit) {
			return false;
		}

		for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
			ptQ.X[i] = 0x00000000; // ptQ.X is storing x.
		}

		return true;
	}

	p38p();

	math.base16Mul(ptQ.T, ptQ.X, ptQ.X);
	math.base16Sub(ptQ.T, ptQ.T, ptQ.Z);
	if(!(equalToZero(ptQ.T))) {
		math.base16Mul(ptQ.X, ptQ.X, complex);
	}
	math.base16Mul(ptQ.T, ptQ.X, ptQ.X);
	math.base16Sub(ptQ.T, ptQ.T, ptQ.Z);
	if(!(equalToZero(ptQ.T))) {
		return false;
	}

	if((ptQ.X[15] & 0x00000001) != bit) {
		math.base16Sub(ptQ.X, p, ptQ.X);
	}

	return true;
}


inline bool Ed25519SignatureAlgorithm::decodePoint(uint32_t* pointOutX, uint32_t* pointOutY, uint32_t* pointOutZ, uint32_t* pointOutT, char* encodedPoint) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		ptQ.Y[i] = encodedPoint[31 - (i*2)] << 8; // 31 = KEY_BYTES - 1, or (SIGNATURE_BYTES/2) - 1.
		ptQ.Y[i] |= encodedPoint[31 - ((i*2) + 1)];
	}
	ptQ.Y[0] &= 0x00007fff;
	bit = encodedPoint[31] >> 7;

	if(!recoverXCoord()) {
		return false;
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		pointOutX[i] = ptQ.X[i];
		pointOutY[i] = ptQ.Y[i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		pointOutZ[i] = oneInt[i];
	}
	math.base16Mul(pointOutT, ptQ.X, ptQ.Y);

	return true;
}


inline bool Ed25519SignatureAlgorithm::greaterThanOrEqualToOrder(uint32_t* a) {
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		if(a[i] > L[i]) {
			return true;
		}
		if(a[i] < L[i]) {
			return false;
		}
	}

	return true;
}


inline void Ed25519SignatureAlgorithm::quickEd25519(const uint32_t* PX, const uint32_t* PY, const uint32_t* PZ, const uint32_t* PT) { // A variable-time implementation of Ed25519 for signature verification.
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		ptP.X[i] = PX[i];
		ptP.Y[i] = PY[i];
		ptP.Z[i] = PZ[i];
		ptP.T[i] = PT[i];
	}

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) { // ptQ = Neutral element.
		ptQ.X[i] = 0x00000000;
		ptQ.Y[i] = oneInt[i];
		ptQ.Z[i] = oneInt[i];
		ptQ.T[i] = 0x00000000;
	}

	for(unsigned short i = 0; i < BITS; i += 1) {
		bit = (scalarByte[(BITS - i)/8] >> (i % 8)) & 0x01;

		if(bit == 0x01) {
			ladderAdd(ptQ.X, ptQ.Y, ptQ.Z, ptQ.T);
		}

		ladderDouble();
	}
}


inline void Ed25519SignatureAlgorithm::initialize(char* publicKeyOut, char* privateKey) {
	generateReadAndPruneHash(privateKey);

	Ed25519(BaseX, BaseY, oneInt, BaseT);

	inverse();
	math.base16Mul(inverseX, ptQ.X, ptQ.Z);
	math.base16Mul(inverseY, ptQ.Y, ptQ.Z);

	encodePoint();

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		publicKey[i] = encodeBytes[i];
		publicKeyOut[i] = publicKey[i];
	}
}


inline void Ed25519SignatureAlgorithm::sign(char* signatureOut, char* publicKeyInOut, char* privateKey, char* message, bool createPublicKey, unsigned long long messageBytes = KEY_BYTES) {
	if(createPublicKey == true) {
		initialize(publicKeyInOut, privateKey);
	} else {
		generateReadAndPruneHash(privateKey);

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
	hashModOrder(regK, prefixMsg, (KEY_BYTES + messageBytes)); // regK is storing r % L.
	delete[] prefixMsg;

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		scalarByte[i*2] = regK[i] >> 8;
		scalarByte[(i*2) + 1] = regK[i];
	}

	Ed25519(BaseX, BaseY, oneInt, BaseT);

	inverse();
	math.base16Mul(inverseX, ptQ.X, ptQ.Z);
	math.base16Mul(inverseY, ptQ.Y, ptQ.Z);

	encodePoint(); // encodeBytes is storing R.

	char* RAMsg = new char[(2*KEY_BYTES) + messageBytes];
	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		RAMsg[i] = encodeBytes[i];
		RAMsg[i + KEY_BYTES] = publicKey[i];
	}
	for(unsigned long long i = 0; i < messageBytes; i += 1) {
		RAMsg[i + (2*KEY_BYTES)] = message[i];
	}
	hashModOrder(regB, RAMsg, ((2*KEY_BYTES) + messageBytes)); // regB is storing k % L.
	delete[] RAMsg;

	order.base16Mul(regC, regB, scalarInt);
	order.base16Add(regC, regK, regC); // regC is storing S.

	for(unsigned short i = 0; i < SIGNATURE_BYTES/2; i += 1) {
		signatureOut[i] = encodeBytes[i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		signatureOut[(i*2) + (SIGNATURE_BYTES/2)] = regC[(INT_LENGTH_MULTI - 1) - i];
		signatureOut[(i*2) + 1 + (SIGNATURE_BYTES/2)] = regC[(INT_LENGTH_MULTI - 1) - i] >> 8;
	}
}


inline bool Ed25519SignatureAlgorithm::verify(char* publicKey, char* message, char* signature, unsigned long long messageBytes = KEY_BYTES) { // Not constant time: all components are public.
	if(!decodePoint(ptA.X, ptA.Y, ptA.Z, ptA.T, publicKey)) {
		return false;
	}

	if(!decodePoint(ptR.X, ptR.Y, ptR.Z, ptR.T, signature)) {
		return false;
	}

	for(unsigned short i = 0; i < KEY_BYTES; i += 1) {
		scalarByte[i] = signature[(SIGNATURE_BYTES - 1) - i];
	}
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		scalarInt[i] = scalarByte[i*2] << 8;
		scalarInt[i] |= scalarByte[(i*2) + 1];
	}
	if(greaterThanOrEqualToOrder(scalarInt)) {
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
	hashModOrder(regK, RAMsg, ((2*KEY_BYTES) + messageBytes)); // regK is storing k % L.
	delete[] RAMsg;

	quickEd25519(BaseX, BaseY, oneInt, BaseT);

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		ptS.X[i] = ptQ.X[i];
		ptS.Y[i] = ptQ.Y[i];
		ptS.Z[i] = ptQ.Z[i];
	} // ptS is storing [S]B.

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		scalarByte[i*2] = regK[i] >> 8;
		scalarByte[(i*2) + 1] = regK[i];
	}

	quickEd25519(ptA.X, ptA.Y, ptA.Z, ptA.T); // ptQ is storing [k]A.

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		ptP.X[i] = ptR.X[i];
		ptP.Y[i] = ptR.Y[i];
		ptP.Z[i] = ptR.Z[i];
		ptP.T[i] = ptR.T[i];
	}

	ladderAdd(ptQ.X, ptQ.Y, ptQ.Z, ptQ.T); // ptQ is storing R + [k]A.

	math.base16Mul(regB, ptS.X, ptQ.Z);
	math.base16Mul(regC, ptQ.X, ptS.Z);
	math.base16Sub(regB, regB, regC);

	if(!(equalToZero(regB))) {
		return false;
	}

	math.base16Mul(regB, ptS.Y, ptQ.Z);
	math.base16Mul(regC, ptQ.Y, ptS.Z);
	math.base16Sub(regB, regB, regC);

	if(!(equalToZero(regB))) {
		return false;
	}

	return true;
}

#endif