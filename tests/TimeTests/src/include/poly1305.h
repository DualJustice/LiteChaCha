#ifndef POLY1305_H
#define POLY1305_H

#include "multiprecision1305.h"

#include <stdint.h>

/*
---------- What you'll need ----------
- 256-bit one-time key, s, created using chacha.
- the message.
	- The message will be broken up into 16-byte blocks. Example block: 0001 6f46 2063 6968 7061 7267 6f74 7079 7243 (9, 16-bit words).
	- The contents of the blocks will be read in little-endian.
- r (128-bit number).
- s (128-bit number).
- p = 		0003 ffff ffff ffff ffff ffff ffff ffff fffb = (2^130) - 5 (9, 16-bit words).
	-  d =	2001 HEX = 8,193 DEC (Smallest viable value)
	- pd =	8003 ffff ffff ffff ffff ffff ffff ffff 5ffb
- acc (the accumulator).

---------- What you'll do ----------
1. Call chacha with blockCounter set to 0 to obtain acc 512-bit state.
2. Take the first 256 bits of the serialized state and use those as the one-time Poly key.
	2.1. The first 128 bits are clamped and form "r."
	2.2. The next 128 bits become "s."
3. Encrypt the message using chacha with blockCounter set to 1.
4. Initialize acc to 0.
5. Divide the message into 16-byte blocks. The last block might be shorter.
LOOP THROUGH BLOCKS {
	6. Read the block in little-endian order.
	7. Add one bit beyond the number of octets. For the shorter block, it can be 2^120, 2^112, or any power of two that is evenly divisible by 8, all the way down to 2^8. (0x01, 0x.., ...)
	8. If the last block is not 17 bytes long (16 + 1 bit), pad (prepend) it with zeros. This is meaningless if you treat the blocks as numbers.
	9. Add this number to the accumulator: acc.
	10. Multiply acc by "r" and take the result mod p. { acc = ((acc + block)*r) % p }.
}
11. Add "s" to acc.
12. The 128 least significant bits are serialized in little-endian order to form the tag.

---------- What to expect ----------
The largest expected block: 	0001 ffff ffff ffff ffff ffff ffff ffff ffff
The largest expected acc:		0003 ffff ffff ffff ffff ffff ffff ffff fffa
The largest expected sum:		0005 ffff ffff ffff ffff ffff ffff ffff fff9
Above sum*d:					c005 ffff ffff ffff ffff ffff ffff ffff 1ff9

The largest expected product:	0003 ffff ffff ffff ffff ffff ffff ffff fff6 0000 0000 0000 0000 0000 0000 0000 0006
Above product*d:				8003 ffff ffff ffff ffff ffff ffff fffe bff6 0000 0000 0000 0000 0000 0000 0000 c006
*/

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

	void clamp();
	void prepareInt(uint32_t*);
	void initializeMAC(uint32_t*, unsigned long long);

	void prepareBlockLittleEndian(char*);
	void prepareFinalBlockLittleEndian(char*);
	void tagSubProcess();
	void incrementBlockCounter();
	void tagProcess(char*);

	void serializeLittleEndian(char*);
public:
	Poly1305MAC();
	~Poly1305MAC();

	void createTag(char[TAG_BYTES], uint32_t[KEY_LENGTH], char*, unsigned long long);
	bool authenticate(uint32_t[KEY_LENGTH], char*, unsigned long long, char*);
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
	math.base16Mul(acc, acc, rMulti);
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


void Poly1305MAC::serializeLittleEndian(char* out) {
	for(unsigned short i = 0; i < DEPENDENT_BLOCK_LENGTH; i += 1) {
		out[(i*2)] = acc[DEPENDENT_BLOCK_LENGTH - i] & BITMASK;
		out[(i*2) + 1] = acc[DEPENDENT_BLOCK_LENGTH - i] >> 8;
	}
}


void Poly1305MAC::createTag(char* out, uint32_t* key, char* message, unsigned long long bytes) {
	if(bytes > 0) {
		initializeMAC(key, bytes);

		tagProcess(message);

		serializeLittleEndian(out);
	}
}


bool Poly1305MAC::authenticate(uint32_t* key, char* message, unsigned long long bytes, char* tag) {

}

#endif