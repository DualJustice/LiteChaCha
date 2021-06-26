#ifndef MULTIPRECISION_H
#define MULTIPRECISION_H

#include <stdint.h>


class multiPrecisionArithmetic {
private:
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

	void prepareIn(uint32_t*, uint32_t*);

	void base16Mod();

	void prepareOut(uint32_t*);
public:
	multiPrecisionArithmetic();
	~multiPrecisionArithmetic();

	static const constexpr unsigned short getMultiLength() {return n;}

	void base32_16(uint32_t*, uint32_t*);

	void base16Add(uint32_t*, uint32_t*, uint32_t*);
	void base16Mul(uint32_t*, uint32_t*, uint32_t*);
	void base16Sub(uint32_t*, uint32_t*, uint32_t*);

	void base16_32(uint32_t*, uint32_t*);
};


multiPrecisionArithmetic::multiPrecisionArithmetic() {

}


multiPrecisionArithmetic::~multiPrecisionArithmetic() {

}

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
*/

void multiPrecisionArithmetic::base32_16(uint32_t* out, uint32_t* a) {
	for(unsigned short i = 0; i < 8; i += 1) {
		out[i*2] = a[i] >> 16;
		out[(i*2) + 1] = a[i] & 0x0000ffff;
	}
}


void multiPrecisionArithmetic::prepareIn(uint32_t* a, uint32_t* b) {
	u[1] = 0x00000000;
	v[0] = 0x00000000;

	for(unsigned short i = 0; i < 16; i += 1) {
		u[i + 2] = a[i];
		v[i + 1] = b[i];
	}
}


void multiPrecisionArithmetic::base16Mod() {
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


void multiPrecisionArithmetic::prepareOut(uint32_t* out) {
	for(unsigned short i = 0; i < 16; i += 1) {
		out[i] = u[i + 2];
	}
}


void multiPrecisionArithmetic::base16Add(uint32_t* out, uint32_t* a, uint32_t* b) { // Might be able to optimize by combining some steps?
	prepareIn(a, b);

	carry = 0x00000000;

	for(unsigned short i = 17; i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	m = 1;
	base16Mod();

	prepareOut(out);
}


void multiPrecisionArithmetic::base16Mul(uint32_t* out, uint32_t* a, uint32_t* b) { // Might be able to optimize by using the Karatsuba method, or some other method. Maybe within the modulus operation as well?
	prepareIn(a, b);

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

	prepareOut(out);
}


void multiPrecisionArithmetic::base16Sub(uint32_t* out, uint32_t* a, uint32_t* b) { // Might be able to optimize by removing base16Mod().
	prepareIn(a, b);

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

	prepareOut(out);
}


void multiPrecisionArithmetic::base16_32(uint32_t* out, uint32_t* a) {
	for(unsigned short i = 0; i < 8; i += 1) {
		out[i] = (a[i*2] << 16) | a[(i*2) + 1];
	}
}

#endif