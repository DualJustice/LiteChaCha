#include "Arduino.h"
#include "HardwareSerial.h"

#include "stdint.h"


/*
0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits).

1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected sum (257 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001 is the largest expected value (512 bits): (ffffffff ... )^2.

0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.
*/


uint32_t p[16]; // These might need to be of a much larger size to account for the carry bit of addition, and the additional bits caused by multiplication.

uint32_t a[8];
uint32_t b[8];

uint32_t u[18];
uint32_t v[18];

char carry;

uint32_t qHat;
uint32_t rHat;

unsigned long timeStamp;
unsigned long duration;


void base32_16() {
	for(unsigned short i = 0; i < 8; i += 1) {
		u[0] = 0x00000000;
		v[0] = 0x00000000;
		u[1] = 0x00000000;
		v[1] = 0x00000000;
		u[(i*2) + 2] = a[i] >> 16;
		v[(i*2) + 2] = b[i] >> 16;
		u[(i*2) + 3] = a[i] & 0x0000ffff;
		v[(i*2) + 3] = b[i] & 0x0000ffff;
	}
}


// ---------- Modulus Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm D ----------
void addMod() {
	for(unsigned short i = 0; i < 18; i += 1) { // D1
		v[i] = u[i];
	}
	base16Add();

	for(unsigned short i = 0; i < 2; i += 1) { // D2 { D3 - D7 }
		qHat = ((2*u[i]) + u[i + 1])/0x0000ffff;
		rHat = ((2*u[i]) + u[i + 1]) % 0x0000ffff;

		if((qHat == 0x00000002) || ((qHat*v[1]) > (2*rHat + u[2]))) {
			qHat -= 1;
			rHat += 0x0000ffff;
			while(rHat < 0x00010000) {

			} // Uh... come back to this.
		}
	}
}


// ---------- Addition Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm A ----------
void base16Add() { // Check for constant time. Might be able to optimize by combining some steps? Addition could be done with 9 31-bit numbers, which would be an optimization, but it would require extra conversions for multiplication. It might be worth an attempt in the future.
	carry = 0x00;

	for(unsigned short i = 17; i > 1; i -= 1) {
		u[i] += (v[i] + carry);
		carry = u[i]/0x00010000;
		u[i] %= 0x00010000;
	}

	u[1] += carry;
}


void base16_32() {
	for(unsigned short i = 0; i < 8; i += 1) {
		a[i] = (u[(i*2) + 2] << 16) | u[(i*2) + 3];
	}
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	p[0] = 0x7fffffff; p[1] = 0xffffffff; p[2] = 0xffffffff; p[3] = 0xffffffff; p[4] = 0xffffffff; p[5] = 0xffffffff; p[6] = 0xffffffff; p[7] = 0xffffffed;

	duration = 0;
	for(unsigned short t = 0; t < 500; t += 1) {
		a[0] = 0xabababab; a[1] = 0xabababab; a[2] = 0xabababab; a[3] = 0xabababab; a[4] = 0xabababab; a[5] = 0xabababab; a[6] = 0xabababab; a[7] = 0xabababab;
		b[0] = 0xabababab; b[1] = 0xabababab; b[2] = 0xabababab; b[3] = 0xabababab; b[4] = 0xabababab; b[5] = 0xabababab; b[6] = 0xabababab; b[7] = 0xabababab;

		base32_16();

		timeStamp = micros();
		base16Add();
		duration += (micros() - timeStamp);

		base16_32();
	}

	Serial.print("micros: ");
	Serial.print(duration);
	Serial.println('\n');

	Serial.print(u[1], HEX);
	Serial.print(' ');
	for(unsigned short i = 0; i < 8; i += 1) {
		Serial.print(a[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');
}


void loop() {}