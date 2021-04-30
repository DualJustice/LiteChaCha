#ifndef CHACHA_H
#define CHACAH_H

#include <stdint.h>


/*
   Note also that the original ChaCha had a 64-bit nonce and 64-bit
   block count.  We have modified this here to be more consistent with
   recommendations in Section 3.2 of [RFC5116].  This limits the use of
   a single (key,nonce) combination to 2^32 blocks, or 256 GB, but that
   is enough for most uses.  In cases where a single key is used by
   multiple senders, it is important to make sure that they don't use
   the same nonces.  This can be assured by partitioning the nonce space
   so that the first 32 bits are unique per sender, while the other 64
   bits come from a counter.

   The ChaCha20 state is initialized as follows:

   o  The first four words (0-3) are constants: 0x61707865, 0x3320646e,
      0x79622d32, 0x6b206574.

   o  The next eight words (4-11) are taken from the 256-bit key by
      reading the bytes in little-endian order, in 4-byte chunks.

   o  Word 12 is a block counter.  Since each block is 64-byte, a 32-bit
      word is enough for 256 gigabytes of data.

   o  Words 13-15 are a nonce, which MUST not be repeated for the same
      key.  The 13th word is the first 32 bits of the input nonce taken
      as a little-endian integer, while the 15th word is the last 32
      bits.

       cccccccc  cccccccc  cccccccc  cccccccc
       kkkkkkkk  kkkkkkkk  kkkkkkkk  kkkkkkkk
       kkkkkkkk  kkkkkkkk  kkkkkkkk  kkkkkkkk
       bbbbbbbb  nnnnnnnn  nnnnnnnn  nnnnnnnn

   c=constant k=key b=blockcount n=nonce

========================================================================

   ChaCha20 successively calls the ChaCha20 block function, with the
   same key and nonce, and with successively increasing block counter
   parameters.  ChaCha20 then serializes the resulting state by writing
   the numbers in little-endian order, creating a keystream block.
   Concatenating the keystream blocks from the successive blocks forms a
   keystream.  The ChaCha20 function then performs an XOR of this
   keystream with the plaintext.  Alternatively, each keystream block
   can be XORed with a plaintext block before proceeding to create the
   next block, saving some memory.  There is no requirement for the
   plaintext to be an integral multiple of 512 bits.  If there is extra
   keystream from the last block, it is discarded.  Specific protocols
   MAY require that the plaintext and ciphertext have certain length.
   Such protocols need to specify how the plaintext is padded and how
   much padding it receives.

   The inputs to ChaCha20 are:

   o  A 256-bit key

   o  A 32-bit initial counter.  This can be set to any number, but will
      usually be zero or one.  It makes sense to use one if we use the
      zero block for something else, such as generating a one-time
      authenticator key as part of an AEAD algorithm.

   o  A 96-bit nonce.  In some protocols, this is known as the
      Initialization Vector.

   o  An arbitrary-length plaintext

   The output is an encrypted message, or "ciphertext", of the same
   length.

   Decryption is done in the same way.  The ChaCha20 block function is
   used to expand the key into a keystream, which is XORed with the
   ciphertext giving back the plaintext.

========================================================================

3.2.  Recommended Nonce Formation

   The following method to construct nonces is RECOMMENDED.  The nonce
   is formatted as illustrated in Figure 1, with the initial octets
   consisting of a Fixed field, and the final octets consisting of a
   Counter field.  For each fixed key, the length of each of these
   fields, and thus the length of the nonce, is fixed.  Implementations
   SHOULD support 12-octet nonces in which the Counter field is four
   octets long.

       <----- variable ----> <----------- variable ----------->
      +---------------------+----------------------------------+
      |        Fixed        |              Counter             |
      +---------------------+----------------------------------+

                    Figure 1: Recommended nonce format

   The Counter fields of successive nonces form a monotonically
   increasing sequence, when those fields are regarded as unsigned
   integers in network byte order.  The length of the Counter field MUST
   remain constant for all nonces that are generated for a given
   encryption device.  The Counter part SHOULD be equal to zero for the
   first nonce, and increment by one for each successive nonce that is
   generated.  However, any particular Counter value MAY be skipped
   over, and left out of the sequence of values that are used, if it is
   convenient.  For example, an application could choose to skip the
   initial Counter=0 value, and set the Counter field of the initial
   nonce to 1.  Thus, at most 2^(8*C) nonces can be generated when the
   Counter field is C octets in length.

   The Fixed field MUST remain constant for all nonces that are
   generated for a given encryption device.  If different devices are
   performing encryption with a single key, then each distinct device
   MUST use a distinct Fixed field, to ensure the uniqueness of the
   nonces.  Thus, at most 2^(8*F) distinct encrypters can share a key
   when the Fixed field is F octets in length.
*/


static const constexpr unsigned short ANALOG_PIN_NUM = A0;
static const constexpr unsigned short ANALOG_RESOLUTION = 32;
static const constexpr unsigned short DEFAULT_ANALOG_RESOLUTION = 10;

static const constexpr unsigned short MAX_BYTE = 0x100;
static const constexpr unsigned short KEY_BYTES = 32;
static const constexpr unsigned short MAX_USER_KEY_LENGTH = (KEY_BYTES*2);
static const constexpr unsigned short FIXED_NONCE_BYTES = 4;
static const constexpr unsigned short MAX_USER_FIXED_NONCE_LENGTH = (FIXED_NONCE_BYTES*2);


class ChaChaEncryption {
private:
	static const constexpr unsigned short CONSTANT_LENGTH = 4;
	static const constexpr unsigned short KEY_LENGTH = 8;
	static const constexpr unsigned short BLOCK_COUNTER_LENGTH = 1;
	static const constexpr unsigned short NONCE_LENGTH = 3;
	static const constexpr unsigned short BLOCK_LENGTH = 16;
	static const constexpr unsigned short ROUNDS = 20;

	static const constexpr unsigned short STREAM_BYTES = 64;
	static const constexpr uint32_t BITMASK = 0x000000ff;

	static constexpr uint32_t constant[CONSTANT_LENGTH] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574}; // In ASCII: "expand 32-byte k"
	uint32_t key[KEY_LENGTH] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
	uint32_t blockCounter[BLOCK_COUNTER_LENGTH] = {0x00000000};
	uint32_t nonce[NONCE_LENGTH] = {0x00000000, 0x00000000, 0x00000000};

	uint32_t peerFixedNonce = 0x00000000;

	uint32_t startState[BLOCK_LENGTH];
	uint32_t endState[BLOCK_LENGTH];
	char keyStream[STREAM_BYTES];
	char cipherText[STREAM_BYTES];

	void constructStartState();
	uint32_t rotL(uint32_t, unsigned short);
	void quarterRound(uint32_t&, uint32_t&, uint32_t&, uint32_t&);
	void createEndState();
	void createKeyStream();
	void createCipherText(const char* message, unsigned int bytes);
public:
	ChaChaEncryption();
	~ChaChaEncryption();

	bool buildEncryption(char*, char*, char*);
	uint32_t* getEndState() {return endState;}
	char* getKeyStream() {return keyStream;}
	char* getCipherText() {return cipherText;}

	void encryptMessage(const char*, unsigned int);
	void decryptMessage();
};


ChaChaEncryption::ChaChaEncryption() {

}


ChaChaEncryption::~ChaChaEncryption() {

}


bool ChaChaEncryption::buildEncryption(char* userKeyIn, char* userFixedNonceIn, char* peerFixedNonceIn) {
	for(unsigned short i = 0; i < KEY_LENGTH; i += 1) {
		key[i] = (userKeyIn[(i*4) + 3] << 24) | (userKeyIn[(i*4) + 2] << 16) | (userKeyIn[(i*4) + 1] << 8) | userKeyIn[i*4];
	}
	nonce[0] = (userFixedNonceIn[3] << 24) | (userFixedNonceIn[2] << 16) | (userFixedNonceIn[1] << 8) | userFixedNonceIn[0];
	peerFixedNonce = (peerFixedNonceIn[3] << 24) | (peerFixedNonceIn[2] << 16) | (peerFixedNonceIn[1] << 8) | peerFixedNonceIn[0];

	return true;
}


void ChaChaEncryption::constructStartState() {
	for(unsigned short i = 0; i < CONSTANT_LENGTH; i += 1) {
		startState[i] = constant[i];
	}
	for(unsigned short i = CONSTANT_LENGTH; i < (KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = key[i - CONSTANT_LENGTH];
	}
	for(unsigned short i = (KEY_LENGTH + CONSTANT_LENGTH); i < (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = blockCounter[i - (KEY_LENGTH + CONSTANT_LENGTH)];
	}
	for(unsigned short i = (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i < (NONCE_LENGTH + BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH); i += 1) {
		startState[i] = nonce[i - (BLOCK_COUNTER_LENGTH + KEY_LENGTH + CONSTANT_LENGTH)];
	}
}


uint32_t ChaChaEncryption::rotL(uint32_t n, unsigned short c) {
	return (n << c) | (n >> (32 - c));
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


void ChaChaEncryption::createKeyStream() {
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[(i*4)] = endState[i] & BITMASK;
		keyStream[(i*4) + 1] = (endState[i] >> 8) & BITMASK;
		keyStream[(i*4) + 2] = (endState[i] >> 16) & BITMASK;
		keyStream[(i*4) + 3] = (endState[i] >> 24) & BITMASK;
	}
}


void ChaChaEncryption::createCipherText(const char* message, unsigned int bytes) {
	for(unsigned short i = 0; i < bytes; i += 1) {
		cipherText[i] = keyStream[i] ^ message[i];
	}
}


void ChaChaEncryption::encryptMessage(const char* message, unsigned int bytes) {
	constructStartState();
	createEndState();
	createKeyStream();
	createCipherText(message, bytes);
}

#endif