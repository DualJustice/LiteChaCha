#ifndef CHACHA_H
#define CHACAH_H

#include <stdint.h>


class ChaChaEncryption {
private:
	static constexpr unsigned short CONSTANT_LENGTH = 4;
	static constexpr unsigned short KEY_LENGTH = 8;
	static constexpr unsigned short BLOCK_COUNTER_LENGTH = 1;
	static constexpr unsigned short NONCE_LENGTH = 3;
	static constexpr unsigned short BLOCK_LENGTH = 16;
	static constexpr unsigned short ROUNDS = 20;

	static constexpr uint32_t constant[CONSTANT_LENGTH] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; // In ASCII: "expand 32-byte k"

	// User defined variables:
	static constexpr uint32_t key[KEY_LENGTH] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}; // User defined key.
	uint32_t blockCounter[BLOCK_COUNTER_LENGTH] = {0x00000000}; // User defined block Number.
	uint32_t nonce[NONCE_LENGTH] = {0x00000000, 0x00000000, 0x00000000}; // User defined nonce.

	uint32_t startState[BLOCK_LENGTH];
	uint32_t endState[BLOCK_LENGTH];

	void constructStartState();
	uint32_t rotL(uint32_t, unsigned short);
	void quarterRound(uint32_t&, uint32_t&, uint32_t&, uint32_t&);
	void createEndState();
public:
	ChaChaEncryption();
	~ChaChaEncryption();
	void encryptMessage();
	void decryptMessage();

	uint32_t* getEndState();
};


ChaChaEncryption::ChaChaEncryption() {

}


ChaChaEncryption::~ChaChaEncryption() {

}


void ChaChaEncryption::constructStartState() {
	for(unsigned short i = 0; i < CONSTANT_LENGTH; i += 1) {
		startState[i] = constant[i];
	}
	for(unsigned short i = CONSTANT_LENGTH; i < (KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = key[i - CONSTANT_LENGTH];
	}
	for(unsigned short i = (KEY_LENGTH + CONSTANT_LENGTH); i < (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = blockCounter[i - (KEY_LENGTH + CONSTANT_LENGTH)];
	}
	for(unsigned short i = (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < (NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = nonce[i - (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH)];
	}
}


uint32_t ChaChaEncryption::rotL(uint32_t n, unsigned short c) {
	return (n << c) | (n >> (32 - c));
}


void ChaChaEncryption::quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
	a += b; d ^= a; d = rotL(d, 16);
	c += d; b ^= c; b = rotL(b, 12);
	a += b; d ^= a; d = rotL(d, 8);
	c += d; b ^= c; b = rotL(b, 7);
}


void ChaChaEncryption::createEndState() {
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		endState[i] = startState[i];
	}

	for(unsigned short i = 0; i < ROUNDS; i += 2) {
		quarterRound(endState[0], endState[4], endState[8], endState[12]);
		quarterRound(endState[1], endState[5], endState[9], endState[13]);
		quarterRound(endState[2], endState[6], endState[10], endState[14]);
		quarterRound(endState[3], endState[7], endState[11], endState[15]);

		quarterRound(endState[0], endState[5], endState[10], endState[15]);
		quarterRound(endState[1], endState[6], endState[11], endState[12]);
		quarterRound(endState[2], endState[7], endState[8], endState[13]);
		quarterRound(endState[3], endState[4], endState[9], endState[14]);
	}

	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		endState[i] += startState[i];
	}
}


void ChaChaEncryption::encryptMessage() {
	constructStartState();
	createEndState();
}


uint32_t* ChaChaEncryption::getEndState() {
	return endState;
}

#endif