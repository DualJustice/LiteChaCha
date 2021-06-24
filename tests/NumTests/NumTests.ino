#include "Arduino.h"
#include "HardwareSerial.h"

#include "stdint.h"


/*
0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.

1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected sum (257 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001 is the largest expected value (512 bits): (ffffffff ... )^2.

0 ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a + b) % p is a D5 negative condition! It works as intended.
*/


uint32_t a[8];
uint32_t b[8];

static const constexpr uint32_t p[16] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed}; // DELETE ME?
static const constexpr uint32_t p2[16] = {0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffda};

uint32_t u[34]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
uint32_t v[17]; // v[0] is used for carry / borrow.

uint32_t carry;
static const constexpr uint32_t base = 0x00010000;

// Modulus variables:
static const constexpr char d = 0x02;
static const constexpr unsigned short n = 16;
unsigned short m;
uint32_t qHat;
uint32_t rHat;
uint32_t q[18]; // DELETE ME?

// Multiplication variables:
uint32_t w[32];

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
	carry = 0x00000000; // D1.
	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] += (u[i] + carry); // Using a temp might be faster?
		carry = u[i]/base;
		u[i] %= base;
	}
	u[0] = 0x00000000;

	Serial.print("D1: ");
	for(unsigned short i = 0; i < 18; i += 1) {
		Serial.print(u[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

	for(unsigned short i = 0; i < (m + 1); i += 1) { // D2 & D7.

		Serial.print("D2 & D7: ");
		Serial.print(i);
		Serial.println('\n');

		qHat = ((base*u[i]) + u[i + 1])/p2[0]; // D3.
		rHat = ((base*u[i]) + u[i + 1]) % p2[0];
		Serial.print("D3: ");
		Serial.print("qHat: ");
		Serial.print(qHat, HEX);
		Serial.print(' ');
		Serial.print("rHat: ");
		Serial.println(rHat, HEX);
		if((qHat == base) || ((qHat*p2[1]) > ((base*rHat) + u[i + 2]))) {
			Serial.print("D3 Flag!");
			Serial.println('\n');
			qHat -= 0x00000001;
			rHat += p2[0];
			if(rHat < base) {
				if((qHat == base) || ((qHat*p2[1]) > ((base*rHat) + u[i + 2]))) {
					qHat -= 0x00000001;
					rHat += p2[0];
					if(rHat < base) {
						Serial.print("D3 Error!");
						Serial.println('\n');
						abort();
					}
				}
			}
		}

		Serial.print("D3: ");
		Serial.print("qHat: ");
		Serial.print(qHat, HEX);
		Serial.print(' ');
		Serial.print("rHat: ");
		Serial.print(rHat, HEX);
		Serial.println('\n');

		carry = 0x00000000; // D4.
		for(unsigned short j = 15; j < 16; j -= 1) {
			v[j + 1] = (qHat*p2[j]) + carry; // Using a temporary value (as t) might be faster? Carry is being used flexibly here (as a "super" carry).
			carry = v[j + 1]/base;
			v[j + 1] %= base;
		}
		v[0] = carry;

		Serial.print("D4 post mul: ");
		for(unsigned short j = 0; j < 17; j += 1) {
			Serial.print(v[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		carry = 0x00000000;
		for(unsigned short j = 17; j > 0; j -= 1) {
			u[j - (m - i)] -= (v[j - 1] + carry); // Using a temp might be faster? Carry is used as a borrow here.
			carry = (u[j - (m - i)] & base)/base;
			u[j - (m - i)] = (u[j - (m - i)] & 0x0001ffff) % base;
		}

		Serial.print("D4 post sub: ");
		for(unsigned short j = 0; j < 18; j += 1) {
			Serial.print(u[j], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');

		q[i] = qHat; // D5. DELETE ME?
		if(carry) {

			Serial.println("D5 Negative!"); // D6.
			q[i] -= 0x00000001; // DELETE ME?
			carry = 0x00000000;
			for(unsigned short j = 15; j < 16; j -= 1) {
				u[(j + 2) - (m - i)] += (p2[j] + carry); // Using a temp might be faster?
				carry = u[(j + 2) - (m - i)]/base;
				u[(j + 2) - (m - i)] %= base;
			}
			u[i] += carry;
			carry = u[i]/base; // FOR TROUBLESHOOTING PURPOSES ONLY! DELETE ME!
			u[i] %= base;
			Serial.print("Carry (should be > 0): "); // FOR TROUBLESHOOTING PURPOSES ONLY! DELETE ME!
			Serial.print(carry, HEX); // FOR TROUBLESHOOTING PURPOSES ONLY! DELETE ME!
			Serial.println('\n');
		}
	}

	carry = 0x00000000; // D8.
	for(unsigned short i = 2; i < 18; i += 1) {
		u[i] += (carry*base); // Carry is used as a remainder here.
		carry = u[i] % d;
		u[i] /= d;
	}

	Serial.print("D8: ");
	for(unsigned short i = 0; i < 18; i += 1) {
		Serial.print(u[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');
}


// ---------- Addition Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm A ----------
void base16Add() { // Check for constant time. Might be able to optimize by combining some steps? Addition could be done with 9 31-bit numbers, which would be an optimization, but it would require extra conversions for multiplication. It might be worth an attempt in the future.
	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry); // Using a temp might be faster?
		carry = u[i]/base;
		u[i] %= base;
	}

	Serial.print("base16Add: ");
	for(unsigned short i = 1; i < 18; i += 1) {
		Serial.print(u[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

	m = 1;
	base16Mod();
}


// ---------- Multiplication Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm M ----------
void base16Mul() { // Might be able to optimize in the future by using the Karatsuba method, or some other method. Maybe within the modulus operation as well?
	for(unsigned short i = 31; i > 15; i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = 16; j > 0; j -= 1) {
		carry = 0x00000000;
		for(unsigned short i = 17; i > 1; i -= 1) {
			w[(i + j) - 2] += ((u[i]*v[j]) + carry); // Using a temporary value (as t) might be faster? Carry is being used flexibly here (as a "super" carry).
			carry = w[(i + j) - 2]/base;
			w[(i + j) - 2] %= base;
		}
		w[j - 1] = carry;
	}

	Serial.print("base16Mul: ");
	for(unsigned short i = 2; i < 34; i += 1) {
		u[i] = w[i - 2];
		Serial.print(u[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');

//	m = 17;
//	base16Mod();
}


// ---------- Subtraction Using: The Art Of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithm S ----------
void base16Sub() { // Produces the signed two's compliment of the difference if it is negative. u[1] (the sign) will equal ffff if u is negative.
	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] -= (v[i - 1] + carry); // Using a temp might be faster? Carry is being used as a borrow here.
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

//	duration = 0;
//	for(unsigned short t = 0; t < 500; t += 1) {
		a[0] = 0xabababab; a[1] = 0xabababab; a[2] = 0xabababab; a[3] = 0xabababab; a[4] = 0xabababab; a[5] = 0xabababab; a[6] = 0xabababab; a[7] = 0xabababab;
		b[0] = 0xabababab; b[1] = 0xabababab; b[2] = 0xabababab; b[3] = 0xabababab; b[4] = 0xabababab; b[5] = 0xabababab; b[6] = 0xabababab; b[7] = 0xabababab;
//               57575757           57575757           57575757           57575757           57575757           57575757           57575757           5757577C = (a + b) % p.
//               731ECA76 21CD7924  D07C27D3 7F2AD682  2DD98530 DC8833DF  8B36E28E 39E5913C  0256AAFF 53A7FC50  A4F94DA1 F64A9EF3  479BF044 98ED4195  EA3E92E7 3B8FE439 = (a * b).

//		a[0] = 0x7fffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0xffffffff; a[6] = 0xffffffff; a[7] = 0xffffffed;
//		a[0] = 0xffffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0xffffffff; a[6] = 0xffffffff; a[7] = 0xffffffff;
//		b[0] = 0xffffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xffffffff; b[4] = 0xffffffff; b[5] = 0xffffffff; b[6] = 0xffffffff; b[7] = 0xffffffff;

		base32_16();

//		timeStamp = micros();
//		base16Add();
		base16Mul();
//		duration += (micros() - timeStamp);

		base16_32();
//	}

//	Serial.print("micros: ");
//	Serial.print(duration);
//	Serial.println('\n');

	Serial.print(u[1], HEX);
	Serial.print(' ');
	for(unsigned short i = 0; i < 8; i += 1) {
		Serial.print(a[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');
}


void loop() {}