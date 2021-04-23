#include "Arduino.h"
#include "HardwareSerial.h"

#include <stdint.h>


#define rotL(x, y) (						\
	(((x) << (y)) | ((x) >> (32 - (y))))	\
)

#define quarterRound(a, b, c, d) (			\
	a += b, d ^= a, rotL(d, 16),			\
	c += d, b ^= c, rotL(b, 12),			\
	a += b, d ^= a, rotL(d, 8),				\
	c += d, b ^= c, rotL(b, 7)				\
)


static const unsigned short BAUD_RATE = 9600;

static const unsigned short CONSTANT_LENGTH = 4;
static const unsigned short KEY_LENGTH = 8;
static const unsigned short BLOCK_NUM_LENGTH = 2;
static const unsigned short NONCE_LENGTH = 2;
static const unsigned short BLOCK_LENGTH = 16;
static const unsigned short ROUNDS = 20;

static const uint32_t constant[CONSTANT_LENGTH] = 0x657870616e642033322d62797465206b; // In ASCII: "expand 32-byte k"

// User defined variables:
static const uint32_t key[KEY_LENGTH] = 0x0000000000000000000000000000000000000000000000000000000000000000; // User defined key.
static const uint32_t blockNum[BLOCK_NUM_LENGTH] = 0x0000000000000000; // User defined block Number.
static const uint32_t nonce[NONCE_LENGTH] = 0x0000000000000000; // User defined nonce.

static uint32_t startState[BLOCK_LENGTH];
static uint32_t keyStream[BLOCK_LENGTH];


void constructStartBlock() {
	for(unsigned short i = 0, i < BLOCK_LENGTH, i += 1) {
		if(i < CONSTANT_LENGTH) {
			startState[i] = constant[i];
		} else if(i < KEY_LENGTH) {
			startState[i] = key[i];
		} else if(i < BLOCK_NUM_LENGTH) {
			startState[i] = blockNum[i];
		} else {
			startState[i] = nonce[i];
		}
	}
}


void chacha() {
	for(unsigned short i = 0, i < BLOCK_LENGTH, i += 1) {
		keyStream[i] = startState[i];
	}

	for(unsigned short i = 0, i < ROUNDS, i += 2) {
		quarterRound(keyStream[0], keyStream[4], keyStream[8], keyStream[12]);
		quarterRound(keyStream[1], keyStream[5], keyStream[9], keyStream[13]);
		quarterRound(keyStream[2], keyStream[6], keyStream[10], keyStream[14]);
		quarterRound(keyStream[3], keyStream[7], keyStream[11], keyStream[15]);

		quarterRound(keyStream[0], keyStream[5], keyStream[10], keyStream[15]);
		quarterRound(keyStream[1], keyStream[6], keyStream[11], keyStream[12]);
		quarterRound(keyStream[2], keyStream[7], keyStream[8], keyStream[13]);
		quarterRound(keyStream[3], keyStream[4], keyStream[9], keyStream[14]);
	}

	for(unsigned short i = 0, i < BLOCK_LENGTH, i += 1) {
		keyStream[i] += startState[i];
	}
}


void printKeyStream() {
	Serial.print("Keystream: ");
	for(unsigned short i = 0, i < BLOCK_LENGTH, i += 1) {
		Serial.print(keyStream[i]);
	}

	Serial.println();
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!serial) {
		delay(250);
	}

	constructStartBlock()
	chacha();
	printKeyStream();
}

void loop() {}