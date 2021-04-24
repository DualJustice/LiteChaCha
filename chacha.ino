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

//static const uint32_t constant[CONSTANT_LENGTH] = {0x65787061, 0x6e642033, 0x322d6279, 0x7465206b}; // In ASCII: "expand 32-byte k"
static const uint32_t constant[CONSTANT_LENGTH] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574};
//												  (0x61707865, 0x3320646e, 0x79622d32, 0x6b206574)
// 657870616e642033322d62797465206b

// User defined variables:
static const uint32_t key[KEY_LENGTH] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000}; // User defined key.
static const uint32_t blockNum[BLOCK_NUM_LENGTH] = {0x00000000, 0x00000000}; // User defined block Number.
static const uint32_t nonce[NONCE_LENGTH] = {0x00000000, 0x00000000}; // User defined nonce.

static uint32_t startState[BLOCK_LENGTH];
static uint32_t keyStream[BLOCK_LENGTH];


void constructStartState() {
	/*for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		if(i < CONSTANT_LENGTH) {
			startState[i] = constant[i];
		} else if(i < (CONSTANT_LENGTH + KEY_LENGTH)) {
			startState[i] = key[i];
		} else if(i < (CONSTANT_LENGTH + KEY_LENGTH + BLOCK_NUM_LENGTH)) {
			startState[i] = blockNum[i];
		} else {
			startState[i] = nonce[i];
		}
	}*/

	for(unsigned short i = 0; i < 4; i += 1) {
		startState[i] = constant[i];
	}

	for(unsigned short i = 4; i < 12; i += 1) {
		startState[i] = key[i - 4];
	}

	for(unsigned short i = 12; i < 14; i += 1) {
		startState[i] = blockNum[i - 12];
	}

	for(unsigned short i = 14; i < 16; i += 1) {
		startState[i] = nonce[i - 14];
	}
/*
	Serial.print("constant: ");
	for(unsigned short i = 0; i < CONSTANT_LENGTH; i += 1) {
		Serial.print(constant[i], HEX);
	}
	Serial.println();
	Serial.print("sizeof constant: ");
	Serial.println(sizeof(constant));

	Serial.print("key: ");
	for(unsigned short i = 0; i < KEY_LENGTH; i += 1) {
		Serial.print(key[i], HEX);
	}
	Serial.println();
	Serial.print("sizeof key: ");
	Serial.println(sizeof(key));

	Serial.print("blockNum: ");
	for(unsigned short i = 0; i < BLOCK_NUM_LENGTH; i += 1) {
		Serial.print(blockNum[i], HEX);
	}
	Serial.println();
	Serial.print("sizeof blockNum: ");
	Serial.println(sizeof(blockNum));

	Serial.print("nonce: ");
	for(unsigned short i = 0; i < NONCE_LENGTH; i += 1) {
		Serial.print(nonce[i], HEX);
	}
	Serial.println();
	Serial.print("sizeof nonce: ");
	Serial.println(sizeof(nonce));

	Serial.print("startState: ");
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
			Serial.print(startState[i], HEX);
	}
	Serial.println();
	Serial.print("sizeof startState: ");
	Serial.println(sizeof(startState));
*/
}


void chacha() {
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[i] = startState[i];
	}

	for(unsigned short i = 0; i < ROUNDS; i += 2) {
		quarterRound(keyStream[0], keyStream[4], keyStream[8], keyStream[12]);
		quarterRound(keyStream[1], keyStream[5], keyStream[9], keyStream[13]);
		quarterRound(keyStream[2], keyStream[6], keyStream[10], keyStream[14]);
		quarterRound(keyStream[3], keyStream[7], keyStream[11], keyStream[15]);

		quarterRound(keyStream[0], keyStream[5], keyStream[10], keyStream[15]);
		quarterRound(keyStream[1], keyStream[6], keyStream[11], keyStream[12]);
		quarterRound(keyStream[2], keyStream[7], keyStream[8], keyStream[13]);
		quarterRound(keyStream[3], keyStream[4], keyStream[9], keyStream[14]);
	}

	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		keyStream[i] += startState[i];
	}
}


void printKeyStream() {
	Serial.print("keyStream: ");
	for(unsigned short i = 0; i < BLOCK_LENGTH; i += 1) {
		Serial.print(keyStream[i], HEX);
	}

	Serial.println();
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	constructStartState();
	chacha();
	printKeyStream();
}

void loop() {}