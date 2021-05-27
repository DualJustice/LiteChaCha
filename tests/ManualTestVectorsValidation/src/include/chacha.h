#ifndef CHACHA_H
#define CHACAH_H

#include <stdint.h>


class ChaChaEncryption {
private:
	static const constexpr unsigned short CONSTANT_LENGTH = 4;
	static const constexpr unsigned short KEY_LENGTH = 8;
	static const constexpr unsigned short BLOCK_COUNTER_LENGTH = 1; // Some implementations use a BLOCK_COUNTER_LENGTH of 2.
	static const constexpr unsigned short NONCE_LENGTH = 4 - BLOCK_COUNTER_LENGTH;
	static const constexpr unsigned short FIXED_NONCE_LENGTH = 1;
	static const constexpr unsigned short COUNTER_NONCE_LENGTH = NONCE_LENGTH - FIXED_NONCE_LENGTH;
	static const constexpr unsigned short BLOCK_LENGTH = CONSTANT_LENGTH + KEY_LENGTH + BLOCK_COUNTER_LENGTH + NONCE_LENGTH; // MUST be equal to 16.
	static const constexpr unsigned short BLOCK_BYTES = BLOCK_LENGTH*4;
	static const constexpr unsigned short ROUNDS = 20;

	static constexpr uint32_t constant[CONSTANT_LENGTH] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; // In ASCII (little-endian): "expand 32-byte k"
	uint32_t key[KEY_LENGTH] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
	uint32_t initialBlockCounter = 0x00000000;
	uint32_t blockCounter = 0x00000000;
	uint32_t fixedNonce = 0x00000000;
	uint32_t nonceCounter[COUNTER_NONCE_LENGTH] = {0x00000000, 0x00000000};

	uint32_t peerFixedNonce = 0x00000000;
	uint32_t peerNonceCounter[COUNTER_NONCE_LENGTH] = {0x00000000, 0x00000000};

	uint32_t startState[BLOCK_LENGTH];
	uint32_t endState[BLOCK_LENGTH];
	char keyStream[BLOCK_BYTES];

	static const constexpr uint32_t BITMASK = 0x000000ff;

	unsigned short encryptBytes = BLOCK_BYTES;
	unsigned long long messageBlockCount = 0;
	unsigned short messageRemainder = 0;
	unsigned long long blockIndexBytes = 0;

	void initializeEncryption(unsigned long long, unsigned long, uint32_t, uint32_t*);

	uint32_t rotL(uint32_t, unsigned short);
	void quarterRound(uint32_t&, uint32_t&, uint32_t&, uint32_t&);
	void createEndState();
	void createKeyStream();
	void createCipherText(char* message);

	void incrementBlockCounter();

//	void incrementNonceCounter();

	void incrementPeerNonceCounter();
	unsigned long long getPeerNonceCounter() {return (peerNonceCounter[0] << 32) | peerNonceCounter[1];}
	unsigned long long currentPeerNonceCounter = 0;

	void encryptAndDecryptSubProcess(char*);
	void encryptAndDecryptProcess(char*, unsigned long long, unsigned long);
public:
	ChaChaEncryption();
	~ChaChaEncryption();

	void incrementNonceCounter(); // Would not normally be in public!

	const unsigned short getNonceCounterBytes() {return COUNTER_NONCE_LENGTH*4;} // Used to specify the length of the prepended Counter Nonce variable.

	void buildEncryption(char*, char*, char*);

	unsigned long long getNonceCounter() {return (nonceCounter[0] << 32) | nonceCounter[1];}

//	unsigned long* getLastEndState() {return (unsigned long*)endState;}
//	char* getLastKeyStream() {return keyStream;}

	void encryptMessage(char*, unsigned long long, unsigned long);
	void decryptMessage(char*, unsigned long long, unsigned long long, unsigned long);
};


ChaChaEncryption::ChaChaEncryption() {

}


ChaChaEncryption::~ChaChaEncryption() {

}


void ChaChaEncryption::buildEncryption(char* userKeyIn, char* userFixedNonceIn, char* peerFixedNonceIn) { // Assumes fixed portion of nonce is 32 bits.
	for(unsigned short i = 0; i < KEY_LENGTH; i += 1) {
		key[i] = (userKeyIn[(i*4) + 3] << 24) | (userKeyIn[(i*4) + 2] << 16) | (userKeyIn[(i*4) + 1] << 8) | userKeyIn[i*4];
	}
	fixedNonce = (userFixedNonceIn[3] << 24) | (userFixedNonceIn[2] << 16) | (userFixedNonceIn[1] << 8) | userFixedNonceIn[0];
	peerFixedNonce = (peerFixedNonceIn[3] << 24) | (peerFixedNonceIn[2] << 16) | (peerFixedNonceIn[1] << 8) | peerFixedNonceIn[0];

	for(unsigned short i = 0; i < CONSTANT_LENGTH; i += 1) {
		startState[i] = constant[i];
	}
	for(unsigned short i = CONSTANT_LENGTH; i < (KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = key[i - CONSTANT_LENGTH];
	}
}


void ChaChaEncryption::initializeEncryption(unsigned long long bytes, unsigned long startBlock, uint32_t fixedNonce, uint32_t* nonceCounter) { // Not generalized for BLOCK_COUNTER_LENGTH > 1. Assumes fixed portion of nonce is 32 bits.
	initialBlockCounter = (uint32_t)startBlock;
	blockCounter = initialBlockCounter;

	for(unsigned short i = (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < (FIXED_NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = fixedNonce;
	}
	for(unsigned short i = (FIXED_NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < BLOCK_LENGTH; i += 1) {
		startState[i] = rotL((nonceCounter[i - (FIXED_NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH)]), 24);
	}

	encryptBytes = BLOCK_BYTES;
	messageBlockCount = (bytes/(BLOCK_BYTES + 1)) + 1;
	messageRemainder = bytes % BLOCK_BYTES;
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


void ChaChaEncryption::createEndState() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	for(unsigned short i = (KEY_LENGTH + CONSTANT_LENGTH); i < (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = blockCounter;
	}

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


void ChaChaEncryption::createKeyStream() { // Consider using different bitmasks for each index as opposed to shifting endState[i] each time (might save on processing time).
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[(i*4)] = endState[i] & BITMASK;
		keyStream[(i*4) + 1] = (endState[i] >> 8) & BITMASK;
		keyStream[(i*4) + 2] = (endState[i] >> 16) & BITMASK;
		keyStream[(i*4) + 3] = (endState[i] >> 24) & BITMASK;
	}
}


void ChaChaEncryption::createCipherText(char* message) { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	blockIndexBytes = ((unsigned long)blockCounter - (unsigned long)initialBlockCounter)*BLOCK_BYTES;

	for(unsigned short i = 0; i < encryptBytes; i += 1) {
		message[i + blockIndexBytes] ^= keyStream[i];
	}
}


void ChaChaEncryption::incrementBlockCounter() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(blockCounter == 0xffffffff) {
		// Log an error here.
	}

	blockCounter += 1;
}


void ChaChaEncryption::incrementNonceCounter() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(!(nonceCounter[0] == 0xffffffff && nonceCounter[1] == 0xffffffff)) {
		if(nonceCounter[1] == 0xffffffff) {
			nonceCounter[1] = 0x00000000;
			nonceCounter[0] += 1;
		} else {
			nonceCounter[1] += 1;
		}
	} else {
		// Log an error here.
		// Wait until new user key and / or fixedNonce is chosen.
	}
}


void ChaChaEncryption::incrementPeerNonceCounter() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(!(peerNonceCounter[0] == 0xffffffff && peerNonceCounter[1] == 0xffffffff)) {
		if(peerNonceCounter[1] == 0xffffffff) {
			peerNonceCounter[1] = 0x00000000;
			peerNonceCounter[0] += 1;
		} else {
			peerNonceCounter[1] += 1;
		}
	} else {
		// Log an error here.
		// Wait until new peer key and / or fixedNonce is chosen.
	}
}


void ChaChaEncryption::encryptAndDecryptSubProcess(char* message) {
	createEndState();
	createKeyStream();
	createCipherText(message);
}


void ChaChaEncryption::encryptAndDecryptProcess(char* message, unsigned long long bytes, unsigned long startBlock = 0) {
	for(unsigned short i = 0; i < (messageBlockCount - 1); i += 1) {
		encryptAndDecryptSubProcess(message);
		incrementBlockCounter();
	}
	if(messageRemainder == 0) {
		encryptAndDecryptSubProcess(message);
	} else {
		encryptBytes = messageRemainder;
		encryptAndDecryptSubProcess(message);
	}
}


void ChaChaEncryption::encryptMessage(char* message, unsigned long long bytes, unsigned long startBlock = 0) {
	if(bytes > 0) {
		initializeEncryption(bytes, startBlock, fixedNonce, nonceCounter);
		encryptAndDecryptProcess(message, bytes, startBlock);
		incrementNonceCounter();
	}
}


void ChaChaEncryption::decryptMessage(char* message, unsigned long long bytes, unsigned long long nonceCounter, unsigned long startBlock = 0) { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(bytes > 0) {
		currentPeerNonceCounter = getPeerNonceCounter();
		if(nonceCounter != currentPeerNonceCounter) {
			for(unsigned short i = 0; i < COUNTER_NONCE_LENGTH; i += 1) {
				peerNonceCounter[i] = nonceCounter >> (32*((COUNTER_NONCE_LENGTH - 1) - i));
			}
		}
		initializeEncryption(bytes, startBlock, peerFixedNonce, peerNonceCounter);
		encryptAndDecryptProcess(message, bytes, startBlock);
		incrementPeerNonceCounter();
	}
}

#endif