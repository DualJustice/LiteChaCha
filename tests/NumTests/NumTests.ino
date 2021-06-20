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


uint32_t u[8];
uint32_t v[8];

char carry;

uint32_t p[8];


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	u[0] = 0x00000000; u[1] = 0x00000000; u[2] = 0x00000000; u[3] = 0x00000000; u[4] = 0x00000000; u[5] = 0x00000000; u[6] = 0x00000000; u[7] = 0x00000000;
	v[0] = 0x00000000; v[1] = 0x00000000; v[2] = 0x00000000; v[3] = 0x00000000; v[4] = 0x00000000; v[5] = 0x00000000; v[6] = 0x00000000; v[7] = 0x00000000;

	p[0] = 0x7fffffff; p[1] = 0xffffffff; p[2] = 0xffffffff; p[3] = 0xffffffff; p[4] = 0xffffffff; p[5] = 0xffffffff; p[6] = 0xffffffff; p[7] = 0xffffffed;

// ---------- Addition Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm A ----------
	carry = 0x00;

	for(unsigned short i = 7; i < 8; i -= 1) {
		u[i] += (v[i] + carry);
		carry = u[i]/v[i];
	}
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