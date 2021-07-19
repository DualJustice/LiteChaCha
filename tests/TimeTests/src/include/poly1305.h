#ifndef POLY1305_H
#define POLY1305_H

#include "Arduino.h" // DELETE ME!
#include "HardwareSerial.h" // DELETE ME!

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

---------- What to expect ----------
The largest expected block: 	0001 ffff ffff ffff ffff ffff ffff ffff ffff
The largest expected a:			0003 ffff ffff ffff ffff ffff ffff ffff fffa
The largest expected sum:		0005 ffff ffff ffff ffff ffff ffff ffff fff9
Above sum*d:					c005 ffff ffff ffff ffff ffff ffff ffff 1ff9

The largest expected product:	0003 ffff ffff ffff ffff ffff ffff ffff fff6 0000 0000 0000 0000 0000 0000 0000 0006
Above product*d:				8003 ffff ffff ffff ffff ffff ffff fffe bff6 0000 0000 0000 0000 0000 0000 0000 c006
*/

class Poly1305MAC {
private:
	MultiPrecisionArithmetic1305 math;

	static const constexpr unsigned short KEYLEN = 8;
	static const constexpr unsigned short INTLEN = 4;
	static const constexpr unsigned short INTLENMULTI = (2*INTLEN) + 1;
	static const constexpr unsigned short BLOCKBYTES = 16;
	static const constexpr unsigned short INITBLOCKLEN = BLOCKBYTES/2;
	static const constexpr unsigned short TAGBYTES = 16;

	uint32_t r[INTLEN];
	uint32_t s[INTLEN];

	static const constexpr uint32_t BITMASK1 = 0x0ffffffc;
	static const constexpr uint32_t BITMASK2 = 0x0fffffff;
	static const constexpr uint32_t BITMASK3 = 0x0000ffff;
	static const constexpr uint32_t BITMASK4 = 0x000000ff;

	uint32_t a[INTLENMULTI];

	uint32_t rMulti[INTLENMULTI];
	uint32_t sMulti[INTLENMULTI];

	uint32_t blockCounter = 0x00000000;
	unsigned long long messageBlockCount = 0;
	unsigned short messageRemainder = 0;
	unsigned long long blockIndexBytes = 0;
	unsigned short finalBlockLastWordIndex = 0;

	uint32_t block[INTLENMULTI];

	void clamp();
	void prepareInt(uint32_t*);
	void initializeMAC(uint32_t*, unsigned long long);

	void prepareBlock(char*);
	void incrementBlockCounter();
	void prepareFinalBlock(char*);
	void tagProcess(char*);
	void serializeLittleEndian(char*);
public:
	Poly1305MAC();
	~Poly1305MAC();

	void createTag(char[TAGBYTES], uint32_t[KEYLEN], char*, unsigned long long);
	bool authenticate(uint32_t[KEYLEN], char*, unsigned long long, char*);
};


Poly1305MAC::Poly1305MAC() {

}


Poly1305MAC::~Poly1305MAC() {

}


void Poly1305MAC::clamp() {
	for(unsigned short i = 0; i < (INTLEN - 1); i += 1) {
		r[i] &= BITMASK1;
	}

	r[INTLEN - 1] &= BITMASK2;
}


void Poly1305MAC::prepareInt(uint32_t* key) {
	for(unsigned short i = 0; i < INTLEN; i += 1) {
		r[i] = key[(INTLEN - 1) - i] << 24;
		r[i] |= ((key[(INTLEN - 1) - i] & 0x0000ff00) << 8);
		r[i] |= ((key[(INTLEN - 1) - i] & 0x00ff0000) >> 8);
		r[i] |= ((key[(INTLEN - 1) - i] & 0xff000000) >> 24);

//		s[i] = (key[(INTLEN + 3) - i] << 24) | ((key[(INTLEN + 3) - i] & 0x0000ff00) << 16) | ((key[(INTLEN + 3) - i] & 0x00ff0000) << 8) | (key[(INTLEN + 3) - i] & 0xff000000);
		s[i] = key[(INTLEN + 3) - i] << 24;
		s[i] |= ((key[(INTLEN + 3) - i] & 0x0000ff00) << 8);
		s[i] |= ((key[(INTLEN + 3) - i] & 0x00ff0000) >> 8);
		s[i] |= ((key[(INTLEN + 3) - i] & 0xff000000) >> 24);
	}

	clamp();

	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		a[i] = 0x00000000;
	}
}


void Poly1305MAC::initializeMAC(uint32_t* key, unsigned long long bytes) {
	blockCounter = 0x00000000;
	messageBlockCount = ((bytes - 1)/BLOCKBYTES) + 1; // OOPS! MAKE SURE TO CHANGE THIS IN CHACHA!
	messageRemainder = bytes % BLOCKBYTES;

	prepareInt(key);
}


void Poly1305MAC::prepareBlock(char* message) {
	blockIndexBytes = ((unsigned long)blockCounter)*BLOCKBYTES;

	for(unsigned short i = 0; i < INITBLOCKLEN; i += 1) {
		block[INITBLOCKLEN - i] = ((message[(i*2) + blockIndexBytes + 1] << 8) | message[(i*2) + blockIndexBytes]) & BITMASK3;
	}

	block[0] = 0x00000001;

	Serial.print("block: ");
	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		Serial.print(block[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

}


void Poly1305MAC::incrementBlockCounter() {
	if(blockCounter == 0xffffffff) {
		// Log an error here.
	}

	blockCounter += 0x00000001;
}


void Poly1305MAC::prepareFinalBlock(char* message) {
	finalBlockLastWordIndex = INITBLOCKLEN - ((messageRemainder - 1)/2);
	blockIndexBytes = ((unsigned long)blockCounter)*BLOCKBYTES;

	Serial.print("blockIndexBytes: ");
	Serial.print(blockIndexBytes);
	Serial.println('\n');

	for(unsigned short i = 0; i < finalBlockLastWordIndex; i += 1) {
		block[i] = 0x00000000;
	}

	for(unsigned short i = 0; i < messageRemainder; i += 1) {
		block[INITBLOCKLEN - (i/2)] = block[INITBLOCKLEN - (i/2)] >> 8;

		Serial.print("step 1: ");
		Serial.println(block[INITBLOCKLEN - (i/2)], HEX);

		block[INITBLOCKLEN - (i/2)] |= (message[i + blockIndexBytes] << 8);

		Serial.print("step 2: ");
		Serial.println(block[INITBLOCKLEN - (i/2)], HEX);

	}

	if(messageRemainder % 2) {
		block[finalBlockLastWordIndex] = block[finalBlockLastWordIndex] >> 8;
		block[finalBlockLastWordIndex] |= 0x00000100;
	} else {
		block[finalBlockLastWordIndex - 1] = 0x00000001;
	}

	Serial.print("block: ");
	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		Serial.print(block[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

}


void Poly1305MAC::tagProcess(char* message) {
	for(unsigned long long i = 0; i < (messageBlockCount - 1); i += 1) {
		prepareBlock(message);
		math.base16Add(a, a, block);

		Serial.print("(Acc+Block) % P: ");
		for(unsigned short j = 0; j < INTLENMULTI; j += 1) {
			Serial.print(a[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		math.base16Mul(a, a, rMulti);

		Serial.print("((Acc+Block)*r) % P: ");
		for(unsigned short j = 0; j < INTLENMULTI; j += 1) {
			Serial.print(a[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		incrementBlockCounter();
	}
	if(messageRemainder == 0) {
		prepareBlock(message);
		math.base16Add(a, a, block);

		Serial.print("(Acc+Block) % P: ");
		for(unsigned short j = 0; j < INTLENMULTI; j += 1) {
			Serial.print(a[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		math.base16Mul(a, a, rMulti);

		Serial.print("((Acc+Block)*r) % P: ");
		for(unsigned short j = 0; j < INTLENMULTI; j += 1) {
			Serial.print(a[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

	} else {
		prepareFinalBlock(message);
		math.base16Add(a, a, block);

		Serial.print("(Acc+Block) % P: ");
		for(unsigned short j = 0; j < INTLENMULTI; j += 1) {
			Serial.print(a[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		math.base16Mul(a, a, rMulti);

		Serial.print("((Acc+Block)*r) % P: ");
		for(unsigned short j = 0; j < INTLENMULTI; j += 1) {
			Serial.print(a[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

	}

	math.base16AddNoMod(a, a, sMulti);

	Serial.print("a + s: ");
	for(unsigned short i = 0; i < INTLENMULTI; i += 1) {
		Serial.print(a[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

}


void Poly1305MAC::serializeLittleEndian(char* out) {
	for(unsigned short i = 0; i < INITBLOCKLEN; i += 1) {
		out[(i*2)] = a[INITBLOCKLEN - i] & BITMASK4;
		out[(i*2) + 1] = (a[INITBLOCKLEN - i] >> 8) & BITMASK4; // Bitwise and statement may not be necessary.
	}
}


void Poly1305MAC::createTag(char* out, uint32_t* key, char* message, unsigned long long bytes) {
	if(bytes > 0) {
		initializeMAC(key, bytes);

		math.base32_16(rMulti, r);
		math.base32_16(sMulti, s);

		Serial.print("rMulti: ");
		for(unsigned short i = 0; i < 9; i += 1) {
			Serial.print(rMulti[i], HEX);
			Serial.print(' ');
		}
		Serial.println();
		Serial.print("sMulti: ");
		for(unsigned short i = 0; i < 9; i += 1) {
			Serial.print(sMulti[i], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		tagProcess(message);

		serializeLittleEndian(out);
	}
}


bool Poly1305MAC::authenticate(uint32_t* key, char* message, unsigned long long bytes, char* tag) {

}

#endif