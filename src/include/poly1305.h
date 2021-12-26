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

	char tempTag[TAG_BYTES];
	char zerosCheck;

	void clamp();
	void prepareInt(uint32_t*);
	void initializeMAC(uint32_t*, unsigned long long);

	void prepareBlockLittleEndian(char*);
	void prepareFinalBlockLittleEndian(char*);
	void tagSubProcess();
	void incrementBlockCounter();
	void tagProcess(char*);

	void createAndAuthenticateProcess(uint32_t*, char*, unsigned long long);

	void serializeLittleEndian(char*);
	bool authenticateLittleEndian(char*);
public:
	Poly1305MAC();
	~Poly1305MAC();

	void createTag(char[TAG_BYTES], uint32_t[KEY_LENGTH], char*, unsigned long long);
	bool authenticateTag(char[TAG_BYTES], uint32_t[KEY_LENGTH], char*, unsigned long long);
};


Poly1305MAC::Poly1305MAC() {

}


Poly1305MAC::~Poly1305MAC() {

}


void Poly1305MAC::clamp() {
	for(unsigned short i = 0; i < (INT_LENGTH - 1); i += 1) {
		r[i] &= 0x0ffffffc;
	}

	r[INT_LENGTH - 1] &= 0x0fffffff;
}


void Poly1305MAC::prepareInt(uint32_t* key) {
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


void Poly1305MAC::initializeMAC(uint32_t* key, unsigned long long bytes) {
	blockCounter = 0x00000000;
	messageBlockCount = ((bytes - 1)/BLOCK_BYTES) + 1;
	messageRemainder = bytes % BLOCK_BYTES;

	prepareInt(key);
}


void Poly1305MAC::prepareBlockLittleEndian(char* message) {
	blockIndexBytes = ((unsigned long)blockCounter)*BLOCK_BYTES;

	for(unsigned short i = 0; i < DEPENDENT_BLOCK_LENGTH; i += 1) {
		block[DEPENDENT_BLOCK_LENGTH - i] = (message[(i*2) + blockIndexBytes + 1] << 8) | message[(i*2) + blockIndexBytes];
	}

	block[0] = 0x00000001;
}


void Poly1305MAC::prepareFinalBlockLittleEndian(char* message) {
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


void Poly1305MAC::tagSubProcess() {
	math.base16Add(acc, acc, block);
	math.base16Mul(acc, acc, rMulti, (messageBlockCount > 2)); // This is not constant time, but the condition is derived from the length of the message, which is public.
}


void Poly1305MAC::incrementBlockCounter() {
	if(blockCounter == 0xffffffff) {
		// Log an error here.
	}

	blockCounter += 0x00000001;
}


void Poly1305MAC::tagProcess(char* message) {
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


void Poly1305MAC::createAndAuthenticateProcess(uint32_t* key, char* message, unsigned long long bytes) {
		initializeMAC(key, bytes);
		tagProcess(message);
}


void Poly1305MAC::serializeLittleEndian(char* out) {
	for(unsigned short i = 0; i < DEPENDENT_BLOCK_LENGTH; i += 1) {
		out[(i*2)] = acc[DEPENDENT_BLOCK_LENGTH - i] & BITMASK;
		out[(i*2) + 1] = acc[DEPENDENT_BLOCK_LENGTH - i] >> 8;
	}
}


bool Poly1305MAC::authenticateLittleEndian(char* tag) {
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


void Poly1305MAC::createTag(char* out, uint32_t* key, char* message, unsigned long long bytes) {
	if(bytes > 0) {
		createAndAuthenticateProcess(key, message, bytes);
		serializeLittleEndian(out);
	}
}


bool Poly1305MAC::authenticateTag(char* tag, uint32_t* key, char* message, unsigned long long bytes) {
	if(bytes > 0) {
		createAndAuthenticateProcess(key, message, bytes);
		return authenticateLittleEndian(tag);
	}

	return false;
}

#endif