#include "Arduino.h"
#include "HardwareSerial.h"

#include "stdint.h"


/*
0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.

1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected sum (257 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001 is the largest expected value (512 bits): (ffffffff ... )^2.
*/


uint32_t a[8];
uint32_t b[8];

static const constexpr uint32_t p[16] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed};
static const constexpr uint32_t p2[16] = {0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffda};

uint32_t u[18]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
uint32_t v[17]; // v[0] is used for carry / borrow.

uint32_t carry;
static const constexpr uint32_t base = 0x00010000;

// Modulus variables:
static const constexpr char d = 0x02; // DELETE ME?
static const constexpr unsigned short n = 16;
unsigned short m;
uint32_t qHat;
uint32_t rHat;
char borrow; // DELETE ME?

unsigned long timeStamp;
unsigned long duration;


void base32_16() {
	u[1] = 0x00000000; // Coinsider moving.
	v[0] = 0x00000000; // Coinsider moving.
	for(unsigned short i = 0; i < 8; i += 1) {
		u[(i*2) + 2] = a[i] >> 16;
		u[(i*2) + 3] = a[i] & 0x0000ffff;

		v[(i*2) + 1] = b[i] >> 16;
		v[(i*2) + 2] = b[i] & 0x0000ffff;
	}
}


// ---------- Modulus Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm D ----------
void base16Mod() { // Won't currently work post multiplication!
	for(unsigned short i = 1; i < (n + m + 1); i += 1) { // D1.
		v[i - 1] = u[i];
	}
	carry = 0x00000000;
	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry); // Using a temp might be faster?
		carry = u[i]/base;
		u[i] %= base;
	}
	u[0] = 0x00000000;

	for(unsigned short i = 0; i < (m + 1); i += 1) { // D2.

		qHat = ((base*u[i]) + u[i + 1])/p2[0]; // D3.
		rHat = ((base*u[i]) + u[i + 1]) % p2[0];
		if((qHat == base) || ((qHat*p2[1]) > ((base*rHat) + u[i + 2]))) {
			qHat -= 0x01;
			rHat += p2[0];
			if(rHat < base) {
				if((qHat == base) || ((qHat*p2[1]) > ((base*rHat) + u[i + 2]))) {
					qHat -= 0x01;
					rHat += p2[0];
					if(rHat < base) {
						Serial.print("ERROR!");
						Serial.println('\n');
						abort();
					}
				}
			}
		}

		carry = 0x00000000;
		for(unsigned short j = 15; j < 16; j -= 1) { // D4.
			v[j + 1] = (qHat*p2[j]) + carry; // Using a temporary value (as t) might be faster?
			carry = v[j + 1]/base;
			v[j + 1] %= base;
		}
		v[0] = carry;
		// u -= v.
		for(unsigned short i = 17; i > 0; i -= 1) { // This won't yet quite work! u[0] and u[1] should flip? Indexing is wrong as well!
			u[i] -= (v[i - 1] + carry);
			carry = (u[i] & base)/base;
			u[i] = (u[i] & 0x0001ffff) % base;
		}
	}
}


// ---------- Addition Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm A ----------
void base16Add() { // Check for constant time. Might be able to optimize by combining some steps? Addition could be done with 9 31-bit numbers, which would be an optimization, but it would require extra conversions for multiplication. It might be worth an attempt in the future.
	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry); // Using a temp might be faster?
		carry = u[i]/base;
		u[i] %= base;
	}

	m = 1;
//	base16Mod();
}


// ---------- Subtraction Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm S ----------
void base16Sub() { // Produces the signed two's compliment of the difference if it is negative. u[1] (the sign) will equal ffff if u is negative.
	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] -= (v[i - 1] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}
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