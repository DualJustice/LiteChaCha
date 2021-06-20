#include "Arduino.h"
#include "HardwareSerial.h"

#include "stdint.h"


/*
p[0] = 0x7fffffff; p[1] = 0xffffffff; p[2] = 0xffffffff; p[3] = 0xffffffff; p[4] = 0xffffffff; p[5] = 0xffffffff; p[6] = 0xffffffff; p[7] = 0xffffffed;
np[0] = 0x00000000; np[1] = 0x00000000; np[2] = 0x00000000; np[3] = 0x00000000; np[4] = 0x00000000; np[5] = 0x00000000; np[6] = 0x00000000; np[7] = 0x00000013;

0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits, because 7fffffff HEX is 01111111111111111111111111111111 BIN).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.

1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected (HEX) sum (257 bits).
2 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffeb is the largest expected sum + p.
fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe0000000000000000000000000000000000000000000000000000000000000001 is the largest expected (HEX) value. (0xffffffff ...)^2

80000000 = 10000000000000000000000000000000
7fffffff = 01111111111111111111111111111111

p (BIN):
111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101101
two's complement of p (BIN):
000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010011
*/


uint32_t p[8]; // Might need to be 16 long?

uint32_t a[8];
uint32_t b[8];

uint32_t u[16];
uint32_t v[16];

char carry;


void base32_16() {
	for(unsigned short i = 0; i < 8; i += 1) {
		u[i*2] = a[i] >> 16;
		v[i*2] = b[i] >> 16;
		u[(i*2) + 1] = a[i] & 0x0000ffff;
		v[(i*2) + 1] = b[i] & 0x0000ffff;
	}
}


// ---------- Addition Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm A ----------
void base16Add() { // Check for constant time. Might be able to optimize by combining some steps?
	carry = 0x00;

	for(unsigned short i = 15; i < 16; i -= 1) {
		u[i] += (v[i] + carry);
		carry = u[i]/0x00010000;
		u[i] %= 0x00010000;
	}
}


void base16_32() {
	for(unsigned short i = 0; i < 8; i += 1) {
		a[i] = (u[i*2] << 16) | u[(i*2) + 1];
		b[i] = (v[i*2] << 16) | v[(i*2) + 1];
	}
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	p[0] = 0x7fffffff; p[1] = 0xffffffff; p[2] = 0xffffffff; p[3] = 0xffffffff; p[4] = 0xffffffff; p[5] = 0xffffffff; p[6] = 0xffffffff; p[7] = 0xffffffed;

	a[0] = 0xffffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0xffffffff; a[6] = 0xffffffff; a[7] = 0xffffffff;
	b[0] = 0xffffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xffffffff; b[4] = 0xffffffff; b[5] = 0xffffffff; b[6] = 0xffffffff; b[7] = 0xffffffff;

	base32_16();
	base16Add();
	base16_32();


	Serial.print(carry, HEX);
	Serial.print(' ');
	for(unsigned short i = 0; i < 8; i += 1) {
		Serial.print(a[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');
}

/*
void add() { // Will want to check all functions for constant time!
	carry = 0x00;

	for(unsigned short i = 7; i < 8; i -= 1) {
		temp = b[i];
		a[i] += (b[i] + carry);

		if(((carry == 0x00) && (a[i] < temp)) || ((carry == 0x01) && (a[i] <= temp))) { // Carry check necessary for all 0's case and all f's case.
			carry = 0x01;
		} else {
			carry = 0x00;
		}
	}
}
*/

void loop() {}