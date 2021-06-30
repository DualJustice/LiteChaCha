#ifndef POLY1305_H
#define POLY1305_H

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
*/

class Poly1305MAC {
private:
public:
	Poly1305MAC();
	~Poly1305MAC();
};


Poly1305MAC::Poly1305MAC() {

}


Poly1305MAC::~Poly1305MAC() {

}

#endif