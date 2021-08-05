#ifndef ED25519_H
#define ED25519_H

#include "SHA512.h"
#include "multiprecision25519.h"

#include <stdint.h>


class Ed25519SignatureAlgorithm {
private:
	SHA512Hash hash;
	MultiPrecisionArithmetic25519 math;

	static const constexpr unsigned short BYTE_LENGTH = 32;
	static const constexpr unsigned short INT_LENGTH = 8;
	static const constexpr unsigned short INT_LENGTH_MULTI = 2*INT_LENGTH;
	static const constexpr unsigned short BITS = 255;

	uint32_t nInt[INT_LENGTH];
	uint32_t xInt[INT_LENGTH];
	uint32_t xIntMulti[INT_LENGTH_MULTI];

	uint32_t X1[INT_LENGTH_MULTI];
	uint32_t X2[INT_LENGTH_MULTI];
	uint32_t Z2[INT_LENGTH_MULTI];
	uint32_t X3[INT_LENGTH_MULTI];
	uint32_t Z3[INT_LENGTH_MULTI];

	uint32_t s;
	uint32_t bit;
	uint32_t mask;
	uint32_t swap;

	uint32_t A[INT_LENGTH_MULTI];
	uint32_t AA[INT_LENGTH_MULTI];
	uint32_t B[INT_LENGTH_MULTI];
	uint32_t BB[INT_LENGTH_MULTI];
	uint32_t E[INT_LENGTH_MULTI];
	uint32_t C[INT_LENGTH_MULTI];
	uint32_t D[INT_LENGTH_MULTI];
	uint32_t DA[INT_LENGTH_MULTI];
	uint32_t CB[INT_LENGTH_MULTI];

	uint32_t A24[INT_LENGTH] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0001db41}; // (486662 - 2)/4.
	uint32_t A24Multi[INT_LENGTH_MULTI];

	char zerosCheck;

	char* decodeBytesLittleEndian(char*);
	char* clampAndDecodeScalar(char*);
	void toUInt(uint32_t*, char*);
	char* maskAndDecodeXCoord(char*);

	void cSwap(uint32_t);
	void ladderStep();
	void montgomeryLadder();
	void inverse();

	char* encodeXCoord(char*);
	void checkAllZerosCase(char*);
public:
	Ed25519SignatureAlgorithm();
	~Ed25519SignatureAlgorithm();

	void curve25519(char[BYTE_LENGTH], char[BYTE_LENGTH]);
};


Ed25519SignatureAlgorithm::Ed25519SignatureAlgorithm() {

}


Ed25519SignatureAlgorithm::~Ed25519SignatureAlgorithm() {

}




#endif