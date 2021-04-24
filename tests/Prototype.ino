#include "Arduino.h"
#include "HardwareSerial.h"

#include <stdint.h>
//#include <limits.h>


/*
static uint32_t a = 0x11111111;
static uint32_t b = 0x01020304;
static uint32_t c = 0x9b8d6f43;
static uint32_t d = 0x01234567;
*/


static uint32_t rotL(uint32_t n, unsigned short c) {
	return (n << c) | (n >> (32 - c));
}

void quarterRound(uint32_t& a, uint32_t& b, uint32_t& c, uint32_t& d) {
	a += b; d ^= a; d = rotL(d, 16);
	c += d; b ^= c; b = rotL(b, 12);
	a += b; d ^= a; d = rotL(d, 8);
	c += d; b ^= c; b = rotL(b, 7);
}


/*
static uint32_t a = 0x80000000;
static uint32_t b = 0x40000000;
*/


/*
static inline uint32_t rotl32(uint32_t n, unsigned int c) {
  const unsigned int mask = (CHAR_BIT*sizeof(n) - 1);  // assumes width is a power of 2.

  // assert ( (c<=mask) &&"rotate by type width or more");
  c &= mask;
  return (n<<c) | (n>>( (-c)&mask ));
}
*/


/*
void runTest() {
	Serial.print("a: ");
	Serial.println(a, HEX);
	Serial.print("b: ");
	Serial.println(b, HEX);

	a = rotL(a, 1);
	Serial.print("a: ");
	Serial.println(a, HEX);

	b = rotL(b, 1);
	Serial.print("b: ");
	Serial.print(b, HEX);
}
*/


/*
For a test vector, we will use the following inputs to the ChaCha20
   block function:

   o  Key = 00:01:02:03:04:05:06:07:08:09:0a:0b:0c:0d:0e:0f:10:11:12:13:
      14:15:16:17:18:19:1a:1b:1c:1d:1e:1f.  The key is a sequence of
      octets with no particular structure before we copy it into the
      ChaCha state.

   o  Nonce = (00:00:00:09:00:00:00:4a:00:00:00:00)

   o  Block Count = 1.

   After setting up the ChaCha state, it looks like this:

   ChaCha state with the key setup.

       61707865  3320646e  79622d32  6b206574
       03020100  07060504  0b0a0908  0f0e0d0c
       13121110  17161514  1b1a1918  1f1e1d1c
       00000001  09000000  4a000000  00000000

   After running 20 rounds (10 column rounds interleaved with 10
   "diagonal rounds"), the ChaCha state looks like this:

   ChaCha state after 20 rounds

       837778ab  e238d763  a67ae21e  5950bb2f
       c4f2d0c7  fc62bb2f  8fa018fc  3f5ec7b7
       335271c2  f29489f3  eabda8fc  82e46ebd
       d19c12b4  b04e16de  9e83d0cb  4e3c50a2

   Finally, we add the original state to the result (simple vector or
   matrix addition), giving this:

   ChaCha state at the end of the ChaCha20 operation

       e4e7f110  15593bd1  1fdd0f50  c47120a3
       c7f4d1c7  0368c033  9aaa2204  4e6cd4c3
       466482d2  09aa9f07  05d7c214  a2028bd9
       d19c12b5  b94e16de  e883d0cb  4e3c50a2
*/


//                                        61707865    3320646e    79622d32    6b206574    03020100    07060504    0b0a0908    0f0e0d0c    13121110    17161514    1b1a1918    1f1e1d1c    00000001    09000000    4a000000    00000000
static const uint32_t startState[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x00000001, 0x09000000, 0x4a000000, 0x00000000};

static uint32_t keyStream[16] = {0x61707865, 0x3320646e, 0x79622d32, 0x6b206574, 0x03020100, 0x07060504, 0x0b0a0908, 0x0f0e0d0c, 0x13121110, 0x17161514, 0x1b1a1918, 0x1f1e1d1c, 0x00000001, 0x09000000, 0x4a000000, 0x00000000};


void runTest() {
	for(unsigned short i = 0; i < 10; i += 1) {
		quarterRound(keyStream[0], keyStream[4], keyStream[8], keyStream[12]);
		quarterRound(keyStream[1], keyStream[5], keyStream[9], keyStream[13]);
		quarterRound(keyStream[2], keyStream[6], keyStream[10], keyStream[14]);
		quarterRound(keyStream[3], keyStream[7], keyStream[11], keyStream[15]);

		quarterRound(keyStream[0], keyStream[5], keyStream[10], keyStream[15]);
		quarterRound(keyStream[1], keyStream[6], keyStream[11], keyStream[12]);
		quarterRound(keyStream[2], keyStream[7], keyStream[8], keyStream[13]);
		quarterRound(keyStream[3], keyStream[4], keyStream[9], keyStream[14]);
	}

	Serial.print("keyStream: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(keyStream[i], HEX);
	}
	Serial.println();

	for(unsigned short i = 0; i < 16; i += 1) {
		keyStream[i] += startState[i];
	}

	Serial.print("keyStream + startState: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(keyStream[i], HEX);
	}

	Serial.println();

	Serial.println(sizeof(keyStream));
}


/*
Single quarter round test vector:
a = 0x11111111
b = 0x01020304
c = 0x9b8d6f43
d = 0x01234567

Should result in:
a = 0xea2a92f4
b = 0xcb1cf8ce
c = 0x4581472e
d = 0x5881c4bb
*/


/*
void runTest() {
	quarterRound(a, b, c, d);
	//a += b; d ^= a; d = rotL(d, 16);
	//c += d; b ^= c; b = rotL(b, 12);
	//a += b; d ^= a; d = rotL(d, 8);
	//c += d; b ^= c; b = rotL(b, 7);

	Serial.print("a: ");
	Serial.println(a, HEX);
	Serial.print("b: ");
	Serial.println(b, HEX);
	Serial.print("c: ");
	Serial.println(c, HEX);
	Serial.print("d: ");
	Serial.println(d, HEX);
}
*/


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	runTest();
}

void loop() {}