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
- p =      0003 ffff ffff ffff ffff ffff ffff ffff fffb = (2^130) - 5 (9, 16-bit words).
	-  d = 2001 HEX = 8,193 DEC (Smallest viable value)
	- pd = 8003 ffff ffff ffff ffff ffff ffff ffff 5ffb
- a (the accumulator).

---------- What you'll do ----------
1. Call chacha with blockCounter set to 0 to obtain a 512-bit state.
2. Take the first 256 bits of the serialized state and use those as the one-time Poly key.
	2.1. The first 128 bits are clamped and form "r."
	2.2. The next 128 bits become "s."
3. Encrypt the message using chacha with blockCounter set to 1.
4. Initialize a to 0.
5. Divide the message into 16-byte blocks. The last block might be shorter.
LOOP THROUGH BLOCKS {
	6. Read the block in little-endian order.
	7. Add one bit beyond the number of octets. For the shorter block, it can be 2^120, 2^112, or any power of two that is evenly divisible by 8, all the way down to 2^8. (0x01, 0x.., ...)
	8. If the last block is not 17 bytes long (16 + 1 bit), pad (prepend) it with zeros. This is meaningless if you treat the blocks as numbers.
	9. Add this number to the accumulator: a.
	10. Multiply a by "r" and take the result mod p. { a = ((a + block)*r) % p }.
}
11. Add "s" to a.
12. The 128 least significant bits are serialized in little-endian order to form the tag.
*/

class Poly1305MAC {
private:
	MultiPrecisionArithmetic1305 math;

	static const constexpr unsigned short KEYLEN = 8;
	static const constexpr unsigned short INTLEN = 4;
	static const constexpr unsigned short INTLENMULTI = (2*INTLEN) + 1;
	static const constexpr unsigned short BLOCKLEN = 16;

	uint32_t r[INTLEN];
	uint32_t s[INTLEN];

	static const constexpr uint32_t BITMASK1 = 0x0ffffffc;
	static const constexpr uint32_t BITMASK2 = 0x0fffffff;

	uint32_t a[INTLENMULTI];

	uint32_t rMulti[INTLENMULTI];
	uint32_t sMulti[INTLENMULTI];

	uint32_t block[INTLENMULTI];

	void clamp(uint32_t*);
	void prepareInt(uint32_t*);
public:
	Poly1305MAC();
	~Poly1305MAC();

	void createTag(char*, uint32_t[KEYLEN], char*, unsigned long long);
	bool authenticate(uint32_t[KEYLEN], char*, unsigned long long, char*);
};


Poly1305MAC::Poly1305MAC() {

}


Poly1305MAC::~Poly1305MAC() {

}


void Poly1305MAC::clamp(uint32_t* r) {
	for(unsigned short i = 0; i < (INTLEN - 1); i += 1) {
		r[i] &= BITMASK1;
	}

	r[INTLEN - 1] &= BITMASK2;
}


void Poly1305MAC::prepareInt(uint32_t* key) {
	for(unsigned short i = 0; i < INTLEN; i += 1) {
		r[i] = key[i];
		s[i] = key[i + 4];
	}

	clamp(r);

	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		a[i] = 0x00000000;
	}
}


void Poly1305MAC::createTag(char* out, uint32_t* key, char* message, unsigned long long bytes) {
	prepareInt(key);

	math.base32_16(rMulti, r);
	math.base32_16(sMulti, s);
}


bool Poly1305MAC::authenticate(uint32_t* key, char* message, unsigned long long bytes, char* tag) {

}

#endif