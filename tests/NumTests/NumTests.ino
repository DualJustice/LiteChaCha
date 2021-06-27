#include "Arduino.h"
#include "HardwareSerial.h"

#include "src\include\multiprecision.h"


/*
0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.

1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected sum (257 bits).
0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe 00000000 00000000 00000000 00000000 00000000 00000000 00000000 00000001 is the largest expected value (512 bits): (ffffffff ... )^2.

0 ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a + b) % p is a D5 negative condition! It works as intended.
0 ffffffff ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a * b) % p is a D5 negative condition! It works as intended.
*/

/*
uint32_t a[8];
uint32_t b[8];

uint32_t u[34]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
uint32_t v[17]; // v[0] is used for carry / borrow.

uint32_t carry; // carry is used for addition carries, subtraction borrows, multiplication carries, and division remainders.
static const constexpr uint32_t base = 0x00010000;

// Modulus variables:
static const constexpr char d = 0x02;
static const constexpr uint32_t p2[16] = {0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffda}; // p*2.
unsigned short m;
static const constexpr unsigned short n = 16;
uint32_t qHat;
uint32_t rHat;
uint32_t c; // Conditional multiplier used in place of conditional branches to aid in constant-time.

// Multiplication variables:
uint32_t w[32];

// Subtraction variables:
static const constexpr uint32_t p[16] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed}; // (2^255) - 19.

unsigned long timeStamp;
unsigned long duration;


void base32_16() {
	u[1] = 0x00000000;
	v[0] = 0x00000000;

	for(unsigned short i = 0; i < 8; i += 1) {
		u[(i*2) + 2] = a[i] >> 16;
		u[(i*2) + 3] = a[i] & 0x0000ffff;

		v[(i*2) + 1] = b[i] >> 16;
		v[(i*2) + 2] = b[i] & 0x0000ffff;
	}
}


void base16Mod() {
// ---------- D1 ----------
	carry = 0x00000000;

	for(unsigned short i = (m + n); i > 0; i -= 1) {
		u[i] += (u[i] + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	u[0] = 0x00000000;

// ---------- D2 & D7 ----------
	for(unsigned short i = 0; i < (m + 1); i += 1) {

// ---------- D3 ----------
		qHat = ((base*u[i]) + u[i + 1])/p2[0];
		rHat = ((base*u[i]) + u[i + 1]) % p2[0];

		c = ((qHat == base) || ((qHat*p2[1]) > ((base*rHat) + u[i + 2])));
		qHat -= c;
		rHat += (c*p2[0]);

		c &= (rHat < base);

		c &= ((qHat == base) || ((qHat*p2[1]) > ((base*rHat) + u[i + 2])));
		qHat -= c;
		rHat += (c*p2[0]);

		if(rHat < base) {
			// Log an error here.
			// Wait until new scalar is chosen.
		}

// ---------- D4 ----------
		carry = 0x00000000;

		for(unsigned short j = 15; j < 16; j -= 1) {
			v[j + 1] = (qHat*p2[j]) + carry;
			carry = v[j + 1]/base;
			v[j + 1] %= base;
		}

		v[0] = carry;

		carry = 0x00000000;

		for(unsigned short j = (m + n); j > (m - 1); j -= 1) {
			u[j - (m - i)] -= (v[j - m] + carry);
			carry = (u[j - (m - i)] & base)/base;
			u[j - (m - i)] = (u[j - (m - i)] & 0x0001ffff) % base;
		}

// ---------- D5 ----------
		c = (carry != 0x00000000);

// ---------- D6 ----------
		carry = 0x00000000;

		for(unsigned short j = (m + n); j > m; j -= 1) {
			u[j - (m - i)] += (c*(p2[(j - m) - 1] + carry));
			carry = u[j - (m - i)]/base;
			u[j - (m - i)] %= base;
		}

		u[i] += carry;
		u[i] %= base;
	}

// ---------- D8 ----------
	carry = 0x00000000;

	for(unsigned short i = (m + 1); i < (m + 17); i += 1) {
		u[i] += (carry*base);
		carry = u[i] % d;
		u[i] /= d;
	}

	if(m > 1) {
		for(unsigned short i = 2; i < 18; i += 1) {
			u[i] = u[i + (m - 1)];
		}
	}
}


void base16Add() { // Might be able to optimize by combining some steps?
	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	m = 1;
	base16Mod();
}


void base16Mul() { // Might be able to optimize by using the Karatsuba method, or some other method. Maybe within the modulus operation as well?
	for(unsigned short i = 31; i > 15; i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = 16; j > 0; j -= 1) {
		carry = 0x00000000;

		for(unsigned short i = 17; i > 1; i -= 1) {
			w[(i + j) - 2] += ((u[i]*v[j]) + carry);
			carry = w[(i + j) - 2]/base;
			w[(i + j) - 2] %= base;
		}

		w[j - 1] = carry;
	}

	for(unsigned short i = 2; i < 34; i += 1) {
		u[i] = w[i - 2];
	}

	m = 17;
	base16Mod();
}


void base16Sub() { // Might be able to optimize by removing base16Mod().
	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] -= (v[i - 1] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}

	u[1] &= 0x00000001;

	for(unsigned short j = 0; j < 3; j += 1) {
		carry = 0x00000000;

		for(unsigned short i = 17; i > 1; i -= 1) {
			u[i] += ((u[1]*p[i - 2]) + carry);
			carry = u[i]/base;
			u[i] %= base;
		}

		u[1] -= carry;
	}

	m = 1;
	base16Mod();
}


void base16_32() {
	for(unsigned short i = 0; i < 8; i += 1) {
		a[i] = (u[(i*2) + 2] << 16) | u[(i*2) + 3];
	}
}
*/


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	multiPrecisionArithmetic math;

	uint32_t a[8];
	uint32_t b[8];
	uint32_t c[8];
	uint32_t am[math.getMultiLength()];
	uint32_t bm[math.getMultiLength()];
	uint32_t cm[math.getMultiLength()];

	unsigned long timeStamp;
	unsigned long duration;

	for(unsigned short t1 = 0; t1 < 3; t1 += 1) {
		duration = 0;

		for(unsigned short t2 = 0; t2 < 500; t2 += 1) {
//			a[0] = 0xabababab; a[1] = 0xabababab; a[2] = 0xabababab; a[3] = 0xabababab; a[4] = 0xabababab; a[5] = 0xabababab; a[6] = 0xabababab; a[7] = 0xabababab;
//			b[0] = 0xabababab; b[1] = 0xabababab; b[2] = 0xabababab; b[3] = 0xabababab; b[4] = 0xabababab; b[5] = 0xabababab; b[6] = 0xabababab; b[7] = 0xabababab;
//		             57575757           57575757           57575757           57575757           57575757           57575757           57575757           5757577C = (a + b) % p.
//		             731ECA76 21CD7924  D07C27D3 7F2AD682  2DD98530 DC8833DF  8B36E28E 39E5913C  0256AAFF 53A7FC50  A4F94DA1 F64A9EF3  479BF044 98ED4195  EA3E92E7 3B8FE439 = (a * b).
//		             18E8B888           5827F7C7           97673706           D6A67646           15E5B585           5524F4C4           94643403           D3A375A7 = (a * b) % p.

//			a[0] = 0x7fffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0xffffffff; a[6] = 0xffffffff; a[7] = 0xffffffed;
//			b[0] = 0x7fffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xffffffff; b[4] = 0xffffffff; b[5] = 0xffffffff; b[6] = 0xffffffff; b[7] = 0xffffffed;
//			a[0] = 0x00000000; a[1] = 0x00000000; a[2] = 0x00000000; a[3] = 0x00000000; a[4] = 0x00000000; a[5] = 0x00000000; a[6] = 0x00000000; a[7] = 0x00000000;
//			b[0] = 0x00000000; b[1] = 0x00000000; b[2] = 0x00000000; b[3] = 0x00000000; b[4] = 0x00000000; b[5] = 0x00000000; b[6] = 0x00000000; b[7] = 0x00000000;

//			a[0] = 0xffffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0xffffffff; a[6] = 0xffffffff; a[7] = 0xffffffff;
//			b[0] = 0xffffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xffffffff; b[4] = 0xffffffff; b[5] = 0xffffffff; b[6] = 0xffffffff; b[7] = 0xffffffff;
//			b[0] = 0xffffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xffffffff; b[4] = 0xffffffff; b[5] = 0xffffffff; b[6] = 0xffffffff; b[7] = 0xffffffd9;

//			a[0] = 0xffffffff; a[1] = 0xffffffff; a[2] = 0x00000000; a[3] = 0x00000000; a[4] = 0x00000000; a[5] = 0x00000000; a[6] = 0x00000000; a[7] = 0x00000000;
//			b[0] = 0xffffffff; b[1] = 0xffffffff; b[2] = 0x00000000; b[3] = 0x00000000; b[4] = 0x00000000; b[5] = 0x00000000; b[6] = 0x00000000; b[7] = 0x00000000;

			math.base32_16(am, a);
			math.base32_16(bm, b);

			timeStamp = micros();
//			math.base16Add(cm, am, bm);
//			math.base16Mul(cm, am, bm);
//			math.base16Sub(cm, am, bm);
			duration += (micros() - timeStamp);

			math.base16_32(c, cm);
		}

		Serial.print("micros: ");
		Serial.print(duration);
		Serial.println('\n');

	//	Serial.print(u[1], HEX);
		Serial.print(' ');
		for(unsigned short i = 0; i < 8; i += 1) {
			Serial.print(c[i], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');
	}
}


void loop() {}