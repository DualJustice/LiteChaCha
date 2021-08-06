#ifndef MULTIPRECISION1305_H
#define MULTIPRECISION1305_H

#include <stdint.h>


class MultiPrecisionArithmetic1305 {
private:
// ---------- Modulus Variables ----------
	static const constexpr uint32_t d = 0x00002001; // 8,193 is the smallest value which satisfies D1.
	unsigned short m;
	static const constexpr unsigned short n = 9;
	const uint32_t pd[n] = {0x00008003, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x00005ffb}; // p*d with p = (2^130) - 5.
	uint32_t qHat;
	uint32_t rHat;
	uint32_t c; // Conditional multiplier used in place of conditional branches to aid in constant-time.

// ---------- Multiplication Variables ----------
	uint32_t w[n*2];

// ---------- General Variables ----------
	uint32_t u[(n*2) + 2]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
	uint32_t v[n + 1]; // v[0] is used for carry / borrow.

	uint32_t carry; // carry is used for addition carries, multiplication carries, and division remainders.
	static const constexpr uint32_t base = 0x00010000;

	void prepareIn(uint32_t*, uint32_t*);

	void base16Mod();

	void prepareOut(uint32_t*);
public:
	MultiPrecisionArithmetic1305();
	~MultiPrecisionArithmetic1305();

	void base32_16(uint32_t*, uint32_t*);

	void base16Add(uint32_t*, uint32_t*, uint32_t*, bool);
	void base16Mul(uint32_t*, uint32_t*, uint32_t*);
};


MultiPrecisionArithmetic1305::MultiPrecisionArithmetic1305() {

}


MultiPrecisionArithmetic1305::~MultiPrecisionArithmetic1305() {

}


void MultiPrecisionArithmetic1305::base32_16(uint32_t* out, uint32_t* a) {
	for(unsigned short i = 0; i < (n/2); i += 1) {
		out[(i*2) + 1] = a[i] >> 16;
		out[(i*2) + 2] = a[i] & 0x0000ffff;
	}

	out[0] = 0x00000000;
}


void MultiPrecisionArithmetic1305::prepareIn(uint32_t* a, uint32_t* b) {
	u[1] = 0x00000000;
	v[0] = 0x00000000;

	for(unsigned short i = 0; i < n; i += 1) {
		u[i + 2] = a[i];
		v[i + 1] = b[i];
	}
}


void MultiPrecisionArithmetic1305::base16Mod() {
// ---------- D1 ----------
	carry = 0x00000000;

	for(unsigned short i = (m + n); i > 0; i -= 1) {
		u[i] = (d*u[i]) + carry;
		carry = u[i]/base;
		u[i] %= base;
	}

	u[1] = carry;

	u[0] = 0x00000000;

// ---------- D2 & D7 ----------
	for(unsigned short i = 0; i < (m + 1); i += 1) {

// ---------- D3 ----------
		qHat = ((base*u[i]) + u[i + 1])/pd[0];
		rHat = ((base*u[i]) + u[i + 1]) % pd[0];

		c = ((qHat == base) || ((qHat*pd[1]) > ((base*rHat) + u[i + 2])));
		qHat -= c;
		rHat += (c*pd[0]);

		c &= (rHat < base);

		c &= ((qHat == base) || ((qHat*pd[1]) > ((base*rHat) + u[i + 2])));
		qHat -= c;
		rHat += (c*pd[0]);

		if(rHat < base) {
			// Log an error here.
			// Wait until new scalar is chosen.
		}

// ---------- D4 ----------
		carry = 0x00000000;

		for(unsigned short j = (n - 1); j < n; j -= 1) {
			v[j + 1] = (qHat*pd[j]) + carry;
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
			u[j - (m - i)] += (c*(pd[(j - m) - 1] + carry));
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
		for(unsigned short i = 2; i < (n + 2); i += 1) {
			u[i] = u[i + (m - 1)];
		}
	}
}


void MultiPrecisionArithmetic1305::prepareOut(uint32_t* out) {
	for(unsigned short i = 0; i < n; i += 1) {
		out[i] = u[i + 2];
	}
}


void MultiPrecisionArithmetic1305::base16Add(uint32_t* out, uint32_t* a, uint32_t* b, bool mod = true) { // Might be able to optimize by combining some steps.
	prepareIn(a, b);

	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	if(mod) {
		m = 1;
		base16Mod();
	}

	prepareOut(out);
}


void MultiPrecisionArithmetic1305::base16Mul(uint32_t* out, uint32_t* a, uint32_t* b) { // Might be able to optimize by using the Karatsuba method, or some other method. Maybe within the modulus operation as well.
	prepareIn(a, b);

	for(unsigned short i = ((n*2) - 1); i > (n - 1); i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = n; j > 0; j -= 1) {
		carry = 0x00000000;

		for(unsigned short i = (n + 1); i > 1; i -= 1) {
			w[(i + j) - 2] += ((u[i]*v[j]) + carry);
			carry = w[(i + j) - 2]/base;
			w[(i + j) - 2] %= base;
		}

		w[j - 1] = carry;
	}

	for(unsigned short i = 2; i < ((n*2) + 2); i += 1) {
		u[i] = w[i - 2];
	}

	m = n + 1;
	base16Mod();

	prepareOut(out);
}

#endif