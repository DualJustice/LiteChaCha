#ifndef ERRORFLAGS_H
#define ERRORFLAGS_H

enum ERROR_BIT: unsigned char {
	CURVE25519_ALL_ZEROS_CASE =		0b00000001,
	MPA25519_MATH_ERROR =			0b00000010,
	MPA252ED_MATH_ERROR =			0b00000100,
	CHACHA_BLOCK_COUNT_OVERFLOW =	0b00001000,
	USER_NONCE_OVERFLOW_IMMINENT =	0b00010000,
	PEER_NONCE_OVERFLOW_IMMINENT =	0b00100000,
	POLY_BLOCK_COUNT_OVERFLOW =		0b01000000,
	MPA1305_MATH_ERROR =			0b10000000
};


class Canary {
private:
	unsigned char errorFlags = 0b00000000;

	Canary() {}
public:
	Canary(const Canary&) = delete;

	static Canary& getFlags() {
		static Canary canary;
		return canary;
	}

	void flagError(const ERROR_BIT errorBit) {errorFlags |= errorBit;}
	unsigned char readFlags() {return errorFlags;}
	void clearFlags() {errorFlags = 0b00000000;}
};

#endif