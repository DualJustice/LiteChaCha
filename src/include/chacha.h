#ifndef CHACHA_H
#define CHACAH_H

#include <stdint.h>


class ChaChaEncryption {
private:
	static constexpr unsigned short CONSTANT_LENGTH = 4;
	static constexpr unsigned short KEY_LENGTH = 8;
	static constexpr unsigned short BLOCK_NUM_LENGTH = 2;
	static constexpr unsigned short NONCE_LENGTH = 2;
	static constexpr unsigned short BLOCK_LENGTH = 16;
	static constexpr unsigned short ROUNDS = 20;

	//static const uint32_t constant[CONSTANT_LENGTH] = {0x65787061, 0x6e642033, 0x322d6279, 0x7465206b}; // In ASCII: "expand 32-byte k"
	static constexpr uint32_t constant[CONSTANT_LENGTH] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};

	// User defined variables:
	static constexpr uint32_t key[KEY_LENGTH] = {0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c}; // User defined key.
	uint32_t blockNum[BLOCK_NUM_LENGTH] = {0x00000001, 0x09000000}; // User defined block Number.
	uint32_t nonce[NONCE_LENGTH] = {0x4a000000, 0x00000000}; // User defined nonce.

	uint32_t startState[BLOCK_LENGTH];
	uint32_t keyStream[BLOCK_LENGTH];

	void constructStartState();
	uint32_t rotL(uint32_t, unsigned short);
	void quarterRound(uint32_t&, uint32_t&, uint32_t&, uint32_t&);
	void createKeyStream();
public:
	ChaChaEncryption();
	~ChaChaEncryption();
	void encryptMessage();
	void decryptMessage();

	uint32_t* getKeyStream();
};


ChaChaEncryption::ChaChaEncryption() {

}


ChaChaEncryption::~ChaChaEncryption() {

}


void ChaChaEncryption::constructStartState() {
	for(unsigned short i = 0; i < 4; i += 1) {
		startState[i] = constant[i];
	}

	for(unsigned short i = 4; i < 12; i += 1) {
		startState[i] = key[i - 4];
	}

	for(unsigned short i = 12; i < 14; i += 1) {
		startState[i] = blockNum[i - 12];
	}

	for(unsigned short i = 14; i < 16; i += 1) {
		startState[i] = nonce[i - 14];
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


void ChaChaEncryption::createKeyStream() {
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[i] = startState[i];
	}

	for(unsigned short i = 0; i < ROUNDS; i += 2) {
		quarterRound(keyStream[0], keyStream[4], keyStream[8], keyStream[12]);
		quarterRound(keyStream[1], keyStream[5], keyStream[9], keyStream[13]);
		quarterRound(keyStream[2], keyStream[6], keyStream[10], keyStream[14]);
		quarterRound(keyStream[3], keyStream[7], keyStream[11], keyStream[15]);

		quarterRound(keyStream[0], keyStream[5], keyStream[10], keyStream[15]);
		quarterRound(keyStream[1], keyStream[6], keyStream[11], keyStream[12]);
		quarterRound(keyStream[2], keyStream[7], keyStream[8], keyStream[13]);
		quarterRound(keyStream[3], keyStream[4], keyStream[9], keyStream[14]);
	}

	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[i] += startState[i];
	}
}


void ChaChaEncryption::encryptMessage() {
	constructStartState();
	createKeyStream();
}


uint32_t* ChaChaEncryption::getKeyStream() {
	return keyStream;
}

#endif