#ifndef POLY1305_H
#define POLY1305_H

#include "chacha.h"
#include "multiprecision.h"

#include <stdint.h>

/*
---------- What you'll need ----------
- 256-bit one-time key, s, created using chacha.
- the message.
	- The message will be broken up into 16-byte blocks.
	- The contents of the blocks will be read on little-endian.
- r (128-bit number).
- s (128-bit number).
- p = 3fffffffffffffffffffffffffffffffb = (2^130) - 5.
- a (the accumulator).

---------- What you'll do ----------
1. Call chacha with blockCounter set to 0 to obtain a 512-bit state.
2. Take the first 256 bits of the serialized state and use those as the one-time Poly key.
	2.1. The first 128 bits are clamped and form "r."
	2.2. The next 128 bits become "s."
3. Encrypt the message using chacha with blockCounter set to 1.
4. Initialize a to 0.
5. Divide the block into 16-byte blocks. The last block might be shorter.
LOOP THROUGH BLOCKS {
	6. Read the block in little-endian order.
	7. Add one bit beyond the number of octets. For the shorter block, it can be 2^120, 2^112, or any power of two that is evenly divisible by 8, all the way down to 2^8. (0x01, 0x.. ..)
	8. If the last block is not 17 bytes long (16 + 1 bit), pad (prepend) it with zeros. This is meaningless if you treat the blocks as numbers.
	9. Add this number to the accumulator: a.
	10. Multiply a by "r" and take the result mod p. { a = ((a + block)*r) % p }.
}
11. Add "s" to a.
12. The 128 least significant bits are serialized in little-endian order to form the tag.
*/

class Poly1305MAC {
private:
	ChaChaEncryption keygen;
	MultiPrecisionArithmetic math;

	
public:
	Poly1305MAC();
	~Poly1305MAC();
};


Poly1305MAC::Poly1305MAC() {

}


Poly1305MAC::~Poly1305MAC() {

}

#endif