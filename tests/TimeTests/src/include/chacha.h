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

	static const constexpr uint32_t BITMASK = 0x000000ff;

	static constexpr uint32_t constant[CONSTANT_LENGTH] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; // In ASCII (little-endian): "expand 32-byte k"
	uint32_t key[KEY_LENGTH] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
	uint32_t initialBlockCounter[BLOCK_COUNTER_LENGTH] = {0x00000000};
	uint32_t blockCounter[BLOCK_COUNTER_LENGTH] = {0x00000000};
	uint32_t nonce[NONCE_LENGTH] = {0x00000000, 0x00000000, 0x00000000};

	uint32_t peerFixedNonce[FIXED_NONCE_LENGTH] = {0x00000000};

	uint32_t startState[BLOCK_LENGTH];
	uint32_t endState[BLOCK_LENGTH];
	char keyStream[BLOCK_BYTES];

	unsigned short encryptBytes = BLOCK_BYTES;
	unsigned long long messageBlockCount = 0;
	unsigned short messageRemainder = 0;
	unsigned long long blockIndexBytes = 0;

	bool decrypt = false;

	void initializeEncryption(unsigned long long, unsigned long);

	uint32_t rotL(uint32_t, unsigned short);
	void updateStartState();
	void quarterRound(uint32_t&, uint32_t&, uint32_t&, uint32_t&);
	void createEndState();
	void createKeyStream();
	void createCipherText(char* message);

	void incrementBlockCounter();

	unsigned long long currentNonceCounter = 0;
	unsigned long long nonceDelta = 0;
//	void incrementNonceCounter();
	void decrementNonceCounter(); // Use carefully (always follow-up with an incrementNonceCounter())!
public:
	ChaChaEncryption();
	~ChaChaEncryption();

	const unsigned short getNonceCounterBytes() {return COUNTER_NONCE_LENGTH*4;} // Used to specify the length of the prepended Counter Nonce variable.

	void buildEncryption(char*, char*, char*);

	unsigned long* getLastEndState() {return (unsigned long*)endState;}
	char* getLastKeyStream() {return keyStream;}

	unsigned long long getNonceCounter() {return (nonce[1] << 32) | nonce[2];}
	void incrementNonceCounter(); // Should not typically be public!

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
	nonce[0] = (userFixedNonceIn[3] << 24) | (userFixedNonceIn[2] << 16) | (userFixedNonceIn[1] << 8) | userFixedNonceIn[0];
	peerFixedNonce[0] = (peerFixedNonceIn[3] << 24) | (peerFixedNonceIn[2] << 16) | (peerFixedNonceIn[1] << 8) | peerFixedNonceIn[0];

	for(unsigned short i = 0; i < CONSTANT_LENGTH; i += 1) {
		startState[i] = constant[i];
	}
	for(unsigned short i = CONSTANT_LENGTH; i < (KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = key[i - CONSTANT_LENGTH];
	}
}


void ChaChaEncryption::initializeEncryption(unsigned long long bytes, unsigned long startBlock) { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	initialBlockCounter[0] = (uint32_t)startBlock;
	blockCounter[0] = initialBlockCounter[0];

	encryptBytes = BLOCK_BYTES;
	messageBlockCount = (bytes/(BLOCK_BYTES + 1)) + 1;
	messageRemainder = bytes % BLOCK_BYTES;
}


uint32_t ChaChaEncryption::rotL(uint32_t n, unsigned short c) {
	return (n << c) | (n >> (32 - c));
}


void ChaChaEncryption::updateStartState() {
	for(unsigned short i = (KEY_LENGTH + CONSTANT_LENGTH); i < (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = blockCounter[i - (KEY_LENGTH + CONSTANT_LENGTH)];
	}
	if(!decrypt) {
		for(unsigned short i = (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < (FIXED_NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
			startState[i] = nonce[i - (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH)];
		}
	} else {
		for(unsigned short i = (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < (FIXED_NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
			startState[i] = nonce[i - (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH)];
		}
	}
	for(unsigned short i = (FIXED_NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < BLOCK_LENGTH; i += 1) {
		startState[i] = rotL((nonce[i - (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH)]), 24);
	}
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


void ChaChaEncryption::createKeyStream() { // Consider using different bitmasks for each index as opposed to shifting endState[i] each time (might save on processing time).
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[(i*4)] = endState[i] & BITMASK;
		keyStream[(i*4) + 1] = (endState[i] >> 8) & BITMASK;
		keyStream[(i*4) + 2] = (endState[i] >> 16) & BITMASK;
		keyStream[(i*4) + 3] = (endState[i] >> 24) & BITMASK;
	}
}


void ChaChaEncryption::createCipherText(char* message) { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	blockIndexBytes = ((unsigned long)blockCounter[0] - (unsigned long)initialBlockCounter[0])*BLOCK_BYTES;

	for(unsigned short i = 0; i < encryptBytes; i += 1) {
		message[i + blockIndexBytes] ^= keyStream[i];
	}
}


void ChaChaEncryption::incrementBlockCounter() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(blockCounter[0] == 0xffffffff) {
		// Log an error here.
	}

	blockCounter[0] += 1;
}


void ChaChaEncryption::incrementNonceCounter() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(!(nonce[1] == 0xffffffff && nonce[2] == 0xffffffff)) {
		if(nonce[2] == 0xffffffff) {
			nonce[2] = 0x00000000;
			nonce[1] += 1;
		} else {
			nonce[2] += 1;
		}
	} else {
		// Log an error here.
		// Wait until new user key and / or fixedNonce is chosen.
	}
}


// ----------Use carefully!----------
void ChaChaEncryption::decrementNonceCounter() { // Not generalized for BLOCK_COUNTER_LENGTH > 1.
	if(!(nonce[1] == 0x00000000 && nonce[2] == 0x00000000)) {
		if(nonce[2] == 0x00000000) {
			nonce[2] = 0xffffffff;
			nonce[1] -= 1;
		} else {
			nonce[2] -= 1;
		}
	} else {
		// Log a warning here.
	}
}
// ----------Use carefully!----------


void ChaChaEncryption::encryptMessage(char* message, unsigned long long bytes, unsigned long startBlock = 0) {
	if(bytes > 0) {
		initializeEncryption(bytes, startBlock);

		for(unsigned short i = 0; i < (messageBlockCount - 1); i += 1) {
			updateStartState();
			createEndState();
			createKeyStream();
			createCipherText(message);
			incrementBlockCounter();
		}
		if(messageRemainder == 0) {
			updateStartState();
			createEndState();
			createKeyStream();
			createCipherText(message);
		} else {
			encryptBytes = messageRemainder;
			updateStartState();
			createEndState();
			createKeyStream();
			createCipherText(message);
		}

		incrementNonceCounter();
	}
}


void ChaChaEncryption::decryptMessage(char* messageDecrypt, unsigned long long bytesDecrypt, unsigned long long nonceCounterDecrypt, unsigned long startBlockDecrypt = 0) {
	decrypt = true;
	currentNonceCounter = getNonceCounter();

	if(nonceCounterDecrypt == currentNonceCounter) { // Will end up incremented up 1 from start because incrementNonceCounter() is in encryptMessage().
		encryptMessage(messageDecrypt, bytesDecrypt, startBlockDecrypt);

	} else if(nonceCounterDecrypt > currentNonceCounter) { // Can sacrifice amount of messages which can be sent by skipping decrementNonceCounter() to save on processing time.
		nonceDelta = nonceCounterDecrypt - currentNonceCounter;

		for(unsigned short i = 0; i < nonceDelta; i += 1) {
			incrementNonceCounter();
		}

		encryptMessage(messageDecrypt, bytesDecrypt, startBlockDecrypt);

		for(unsigned short i = 0; i < nonceDelta; i += 1) { // Will end up incremented up 1 from start because incrementNonceCounter() is in encryptMessage().
			decrementNonceCounter();
		}

	} else {
		nonceDelta = currentNonceCounter - nonceCounterDecrypt;

		for(unsigned short i = 0; i < nonceDelta; i += 1) {
			decrementNonceCounter();
		}

		encryptMessage(messageDecrypt, bytesDecrypt, startBlockDecrypt);

		for(unsigned short i = 0; i < nonceDelta; i += 1) { // Will end up incremented up 1 from start because incrementNonceCounter() is in encryptMessage().
			incrementNonceCounter();
		}
	}

	decrypt = false;
}

#endif