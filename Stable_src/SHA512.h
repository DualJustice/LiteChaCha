#ifndef SHA512_H
#define SHA512_H

#include <stdint.h>


class SHA512Hash {
private:
	static const constexpr unsigned short HASH_BYTES = 64;
	static const constexpr unsigned short WORD_CONVERSION = 8; // 8 bytes/1 word, 1 word/8 bytes.
	static const constexpr unsigned short HASH_WORDS = HASH_BYTES/WORD_CONVERSION;
	static const constexpr unsigned short ROUNDS = 80;
	static const constexpr unsigned short BIT_CONVERSION = 8; // 8 bits/1 byte, 1 byte/8 bits.
	static const constexpr unsigned short BLOCK_BITS = 1024;
	static const constexpr unsigned short BLOCK_BYTES = BLOCK_BITS/BIT_CONVERSION;
	static const constexpr unsigned short APPEND_BIT = 1;
	static const constexpr unsigned short MESSAGE_LENGTH_BITS = 128;
	static const constexpr unsigned short WORD_BITS = 64;
	static const constexpr unsigned short MESSAGE_LENGTH_WORDS = MESSAGE_LENGTH_BITS/WORD_BITS;
	static const constexpr unsigned short BLOCK_WORDS = BLOCK_BITS/WORD_BITS;
	static const constexpr unsigned short HALF_WORD_CONVERSION = WORD_CONVERSION/2;
	static const constexpr unsigned short HALF_WORD_SHIFT = 24;
	static const constexpr unsigned short HALF_WORD_BITS = WORD_BITS/2;
	static const constexpr unsigned short WORD_SHIFT = WORD_BITS - BIT_CONVERSION;

	uint64_t h[HASH_WORDS];

	const uint64_t hInit[HASH_WORDS] = {0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179}; // Derived from the first 64 bits of the fractional parts of the square roots of the first 8 primes.

	unsigned short messageRemainderBits;
	unsigned short zeroBits;
	unsigned short zeroWords;

	unsigned long long messageWords;
	unsigned short wordRemainder;

	unsigned long long wordIndex;
	unsigned long long blockCount;

	uint32_t halfWordBuffer;
	uint64_t wordBuffer;

	uint64_t w[ROUNDS]; // Message schedule array.
	uint64_t s0;
	uint64_t s1;

	uint64_t a[HASH_WORDS]; // Working variables.
	uint64_t t0;
	uint64_t t1;

	const uint64_t k[ROUNDS] = {
		0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 
		0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694, 
		0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65, 0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 
		0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70, 
		0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b, 
		0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 
		0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3, 
		0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec, 0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 
		0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b, 
		0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
	}; // Derived from the first 64 bits of the fractional parts of the cube roots of the first 80 primes.

	void initialize(const unsigned long long);

	void buildMessage(uint64_t*, const unsigned char*, const unsigned long long);

	void rotR(const uint64_t, const unsigned short);
	void hashProcess(const uint64_t*);

	void outputHash(unsigned char[HASH_BYTES]);
public:
	void hashBytes(unsigned char[HASH_BYTES], const unsigned char*, const unsigned long long);
};


inline void SHA512Hash::initialize(const unsigned long long messageBytes) {
	for(unsigned short i = 0; i < HASH_WORDS; i += 1) {
		h[i] = hInit[i];
	}

	messageRemainderBits = (messageBytes % BLOCK_BYTES)*BIT_CONVERSION;
	zeroBits = BLOCK_BITS - ((((messageRemainderBits + APPEND_BIT + MESSAGE_LENGTH_BITS) - 1) % BLOCK_BITS) + 1);
	zeroWords = zeroBits/WORD_BITS;

	messageWords = messageBytes/WORD_CONVERSION;
	wordRemainder = messageBytes % WORD_CONVERSION;

	wordIndex = messageWords + zeroWords + MESSAGE_LENGTH_WORDS;
	blockCount = (wordIndex + 1)/BLOCK_WORDS;
}


inline void SHA512Hash::buildMessage(uint64_t* message, const unsigned char* messageIn, const unsigned long long messageBytes) {
	for(unsigned long long i = 0; i < messageWords; i += 1) {
		message[i] = 0x0000000000000000;
		for(unsigned short j = 0; j < HALF_WORD_CONVERSION; j += 1) {
			message[i] |= ((uint64_t)messageIn[(WORD_CONVERSION*i) + j] << (HALF_WORD_SHIFT - (BIT_CONVERSION*j)));
		}
		message[i] = message[i] << HALF_WORD_BITS;
		halfWordBuffer = 0x00000000;
		for(unsigned short j = HALF_WORD_CONVERSION; j < WORD_CONVERSION; j += 1) {
			halfWordBuffer |= (messageIn[(WORD_CONVERSION*i) + j] << (HALF_WORD_SHIFT - (BIT_CONVERSION*(j - HALF_WORD_CONVERSION))));
		}
		message[i] |= halfWordBuffer;
	}

	if(wordRemainder == 0) {
		message[messageWords] = 0x8000000000000000;
	} else {
		message[messageWords] = 0x0000000000000000;
		if(wordRemainder < HALF_WORD_CONVERSION) {
			for(unsigned short i = 0; i < wordRemainder; i += 1) {
				message[messageWords] |= ((uint64_t)messageIn[(WORD_CONVERSION*messageWords) + i] << (HALF_WORD_SHIFT - (BIT_CONVERSION*i)));
			}
			message[messageWords] |= (0x0000000000000080 << (HALF_WORD_SHIFT - (BIT_CONVERSION*wordRemainder)));
			message[messageWords] = message[messageWords] << HALF_WORD_BITS;

		} else if(wordRemainder == HALF_WORD_CONVERSION) {
			for(unsigned short i = 0; i < wordRemainder; i += 1) {
				message[messageWords] |= ((uint64_t)messageIn[(WORD_CONVERSION*messageWords) + i] << (HALF_WORD_SHIFT - (BIT_CONVERSION*i)));
			}
			message[messageWords] = message[messageWords] << HALF_WORD_BITS;
			halfWordBuffer = 0x80000000;
			message[messageWords] |= halfWordBuffer;

		} else {
			for(unsigned short i = 0; i < HALF_WORD_CONVERSION; i += 1) {
				message[messageWords] |= ((uint64_t)messageIn[(WORD_CONVERSION*messageWords) + i] << (HALF_WORD_SHIFT - (BIT_CONVERSION*i)));
			}
			message[messageWords] = message[messageWords] << HALF_WORD_BITS;
			halfWordBuffer = 0x00000000;
			for(unsigned short i = HALF_WORD_CONVERSION; i < wordRemainder; i += 1) {
				halfWordBuffer |= (messageIn[(WORD_CONVERSION*messageWords) + i] << (HALF_WORD_SHIFT - (BIT_CONVERSION*(i - HALF_WORD_CONVERSION))));
			}
			halfWordBuffer |= (0x80 << (HALF_WORD_SHIFT - (BIT_CONVERSION*(wordRemainder % HALF_WORD_CONVERSION))));
			message[messageWords] |= halfWordBuffer;
		}
	}

	for(unsigned long long i = 0; i < zeroWords; i += 1) {
		message[(messageWords + 1) + i] = 0x0000000000000000;
	}

	message[wordIndex - 1] = (uint64_t)messageBytes >> 61; // Has the effect of multiplying messageBytes by 8 (to convert from bytes to bits).
	message[wordIndex] = (uint64_t)messageBytes << 3;
}


inline void SHA512Hash::rotR(const uint64_t n, const unsigned short c) {
	wordBuffer = ((n >> c) | (n << (WORD_BITS - c)));
}


inline void SHA512Hash::hashProcess(const uint64_t* message) {
	for(unsigned long long b = 0; b < blockCount; b += 1) {
		for(unsigned short i = 0; i < BLOCK_WORDS; i += 1) {
			w[i] = message[(BLOCK_WORDS*b) + i];
		}

		for(unsigned short i = BLOCK_WORDS; i < ROUNDS; i += 1) {
			rotR(w[i - 15], 1);
			s0 = wordBuffer;
			rotR(w[i - 15], 8);
			s0 ^= wordBuffer;
			s0 ^= (w[i - 15] >> 7);

			rotR(w[i - 2], 19);
			s1 = wordBuffer;
			rotR(w[i - 2], 61);
			s1 ^= wordBuffer;
			s1 ^= (w[i - 2] >> 6);

			w[i] = w[i - 16] + s0 + w[i - 7] + s1;
		}

		for(unsigned short i = 0; i < HASH_WORDS; i += 1) {
			a[i] = h[i];
		}

		for(unsigned short i = 0; i < ROUNDS; i += 1) {
			rotR(a[4], 14);
			s1 = wordBuffer;
			rotR(a[4], 18);
			s1 ^= wordBuffer;
			rotR(a[4], 41);
			s1 ^= wordBuffer;

			s0 = (a[4] & a[5]) ^ ((~a[4]) & a[6]);

			t0 = a[7] + s1 + s0 + k[i] + w[i];

			rotR(a[0], 28);
			s0 = wordBuffer;
			rotR(a[0], 34);
			s0 ^= wordBuffer;
			rotR(a[0], 39);
			s0 ^= wordBuffer;

			s1 = (a[0] & a[1]) ^ (a[0] & a[2]) ^ (a[1] & a[2]);
			t1 = s0 + s1;

			a[7] = a[6];
			a[6] = a[5];
			a[5] = a[4];
			a[4] = a[3] + t0;
			a[3] = a[2];
			a[2] = a[1];
			a[1] = a[0];
			a[0] = t0 + t1;
		}

		for(unsigned short i = 0; i < HASH_WORDS; i += 1) {
			h[i] += a[i];
		}
	}
}


inline void SHA512Hash::outputHash(unsigned char* hashOut) {
	for(unsigned short i = 0; i < HASH_WORDS; i += 1) {
		for(unsigned short j = 0; j < WORD_CONVERSION; j += 1) {
			hashOut[(WORD_CONVERSION*i) + j] = h[i] >> (WORD_SHIFT - (BIT_CONVERSION*j));
		}
	}
}


inline void SHA512Hash::hashBytes(unsigned char* hashOut, const unsigned char* messageIn, const unsigned long long messageBytes) {
	initialize(messageBytes);

	uint64_t* message = new uint64_t[wordIndex + 1];

	buildMessage(message, messageIn, messageBytes);
	hashProcess(message);
	outputHash(hashOut);

	delete[] message;
}

#endif