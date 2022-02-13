#ifndef POLY1305_H
#define POLY1305_H

#include "multiprecision1305.h"

#include <stdint.h>


class Poly1305MAC {
private:
	MultiPrecisionArithmetic1305 math;

	static const constexpr unsigned short TAG_BYTES = 16;
	static const constexpr unsigned short KEY_LENGTH = 8;
	static const constexpr unsigned short INT_LENGTH = 4;
	static const constexpr unsigned short INT_LENGTH_MULTI = (2*INT_LENGTH) + 1;
	static const constexpr unsigned short BLOCK_BYTES = 16;
	static const constexpr unsigned short DEPENDENT_BLOCK_LENGTH = BLOCK_BYTES/2;

	uint32_t blockCounter = 0x00000000;

	uint32_t r[INT_LENGTH];
	uint32_t s[INT_LENGTH];

	uint32_t rMulti[INT_LENGTH_MULTI];
	uint32_t sMulti[INT_LENGTH_MULTI];

	uint32_t acc[INT_LENGTH_MULTI];

	uint32_t block[INT_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

	unsigned long long messageBlockCount = 0;
	unsigned short messageRemainder = 0;
	unsigned long long blockIndexBytes = 0;
	unsigned short finalBlockLastWordIndex = 0;

	static const constexpr uint32_t BITMASK = 0x000000ff;

	unsigned char tempTag[TAG_BYTES];
	unsigned char zerosCheck;

	void clamp();
	void prepareInt(const uint32_t*);
	void initializeMAC(const uint32_t*, const unsigned long long);

	void prepareBlockLittleEndian(const unsigned char*);
	void prepareFinalBlockLittleEndian(const unsigned char*);
	void tagSubProcess();
	void incrementBlockCounter();
	void tagProcess(const unsigned char*);

	void createAndAuthenticateProcess(const uint32_t*, const unsigned char*, const unsigned long long);

	void serializeLittleEndian(unsigned char*);
	bool authenticateLittleEndian(const unsigned char*);
public:
	void createTag(unsigned char[TAG_BYTES], const uint32_t[KEY_LENGTH], const unsigned char*, const unsigned long long);
	bool authenticateTag(const unsigned char[TAG_BYTES], const uint32_t[KEY_LENGTH], const unsigned char*, const unsigned long long);
};


inline void Poly1305MAC::clamp() {
	for(unsigned short i = 0; i < (INT_LENGTH - 1); i += 1) {
		r[i] &= 0x0ffffffc;
	}

	r[INT_LENGTH - 1] &= 0x0fffffff;
}


inline void Poly1305MAC::prepareInt(const uint32_t* key) {
	for(unsigned short i = 0; i < INT_LENGTH; i += 1) {
		r[i] = key[(INT_LENGTH - 1) - i] << 24;
		r[i] |= ((key[(INT_LENGTH - 1) - i] & 0x0000ff00) << 8);
		r[i] |= ((key[(INT_LENGTH - 1) - i] & 0x00ff0000) >> 8);
		r[i] |= (key[(INT_LENGTH - 1) - i] >> 24);

		s[i] = key[(INT_LENGTH + 3) - i] << 24;
		s[i] |= ((key[(INT_LENGTH + 3) - i] & 0x0000ff00) << 8);
		s[i] |= ((key[(INT_LENGTH + 3) - i] & 0x00ff0000) >> 8);
		s[i] |= (key[(INT_LENGTH + 3) - i] >> 24);
	}

	clamp();

	math.base32_16(rMulti, r);
	math.base32_16(sMulti, s);

	if(messageBlockCount > 2) { // This is not constant time, but the condition is derived from the length of the message, which is public.
		math.base16Mod(rMulti, rMulti);
	}

	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		acc[i] = 0x00000000;
	}
}


inline void Poly1305MAC::initializeMAC(const uint32_t* key, const unsigned long long bytes) {
	blockCounter = 0x00000000;
	messageBlockCount = ((bytes - 1)/BLOCK_BYTES) + 1;
	messageRemainder = bytes % BLOCK_BYTES;

	prepareInt(key);
}


inline void Poly1305MAC::prepareBlockLittleEndian(const unsigned char* message) {
	blockIndexBytes = ((unsigned long)blockCounter)*BLOCK_BYTES;

	for(unsigned short i = 0; i < DEPENDENT_BLOCK_LENGTH; i += 1) {
		block[DEPENDENT_BLOCK_LENGTH - i] = (message[(i*2) + blockIndexBytes + 1] << 8) | message[(i*2) + blockIndexBytes];
	}

	block[0] = 0x00000001;
}


inline void Poly1305MAC::prepareFinalBlockLittleEndian(const unsigned char* message) {
	finalBlockLastWordIndex = DEPENDENT_BLOCK_LENGTH - ((messageRemainder - 1)/2);
	blockIndexBytes = ((unsigned long)blockCounter)*BLOCK_BYTES;

	for(unsigned short i = 0; i < finalBlockLastWordIndex; i += 1) {
		block[i] = 0x00000000;
	}

	for(unsigned short i = 0; i < messageRemainder; i += 1) {
		block[DEPENDENT_BLOCK_LENGTH - (i/2)] = block[DEPENDENT_BLOCK_LENGTH - (i/2)] >> 8;
		block[DEPENDENT_BLOCK_LENGTH - (i/2)] |= (message[i + blockIndexBytes] << 8);
	}

	if(messageRemainder % 2) {
		block[finalBlockLastWordIndex] = block[finalBlockLastWordIndex] >> 8;
		block[finalBlockLastWordIndex] |= 0x00000100;
	} else {
		block[finalBlockLastWordIndex - 1] = 0x00000001;
	}
}


inline void Poly1305MAC::tagSubProcess() {
	math.base16Add(acc, acc, block);
	math.base16Mul(acc, acc, rMulti, (messageBlockCount > 2)); // This is not constant time, but the condition is derived from the length of the message, which is public.
}


inline void Poly1305MAC::incrementBlockCounter() {
	if(blockCounter == 0xffffffff) {
		// Log an error here.
	}

	blockCounter += 0x00000001;
}


inline void Poly1305MAC::tagProcess(const unsigned char* message) {
	for(unsigned long long i = 0; i < (messageBlockCount - 1); i += 1) {
		prepareBlockLittleEndian(message);
		tagSubProcess();
		incrementBlockCounter();
	}
	if(messageRemainder == 0) {
		prepareBlockLittleEndian(message);
		tagSubProcess();
		math.base16Add(acc, acc, sMulti, false);
	} else {
		prepareFinalBlockLittleEndian(message);
		tagSubProcess();
		math.base16Add(acc, acc, sMulti, false);
	}
}


inline void Poly1305MAC::createAndAuthenticateProcess(const uint32_t* key, const unsigned char* message, const unsigned long long bytes) {
		initializeMAC(key, bytes);
		tagProcess(message);
}


inline void Poly1305MAC::serializeLittleEndian(unsigned char* out) {
	for(unsigned short i = 0; i < DEPENDENT_BLOCK_LENGTH; i += 1) {
		out[(i*2)] = acc[DEPENDENT_BLOCK_LENGTH - i] & BITMASK;
		out[(i*2) + 1] = acc[DEPENDENT_BLOCK_LENGTH - i] >> 8;
	}
}


inline bool Poly1305MAC::authenticateLittleEndian(const unsigned char* tag) {
	for(unsigned short i = 0; i < TAG_BYTES; i += 1) {
		tempTag[i] = tag[i];
	}

	zerosCheck = 0x00;

	for(unsigned short i = 0; i < DEPENDENT_BLOCK_LENGTH; i += 1) {
		tempTag[i*2] ^= (acc[DEPENDENT_BLOCK_LENGTH - i] & BITMASK);
		tempTag[(i*2) + 1] ^= (acc[DEPENDENT_BLOCK_LENGTH - i] >> 8);

		zerosCheck |= tempTag[i*2];
		zerosCheck |= tempTag[(i*2) + 1];
	}

	return (!zerosCheck);
}


inline void Poly1305MAC::createTag(unsigned char* out, const uint32_t* key, const unsigned char* message, const unsigned long long bytes) {
	if(bytes > 0) {
		createAndAuthenticateProcess(key, message, bytes);
		serializeLittleEndian(out);
	}
}


inline bool Poly1305MAC::authenticateTag(const unsigned char* tag, const uint32_t* key, const unsigned char* message, const unsigned long long bytes) {
	if(bytes > 0) {
		createAndAuthenticateProcess(key, message, bytes);
		return authenticateLittleEndian(tag);
	}

	return false;
}

#endif