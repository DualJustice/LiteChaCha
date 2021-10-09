#ifndef MULTIPRECISION252ED_H
#define MULTIPRECISION252ED_H

#include <stdint.h>


class MultiPrecisionArithmetic252ed {
private:
// ---------- Modulus Variables ----------
	static const constexpr uint32_t d = 0x00000008; // 8 is the smallest value which satisfies D1.
	unsigned short m;
	static const constexpr unsigned short n = 16;
	const uint32_t p[n] = {0x00001000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x000014de, 0x0000f9de, 0x0000a2f7, 0x00009cd6, 0x00005812, 0x0000631a, 0x00005cf5, 0x0000d3ed}; // (2^252) + 27742317777372353535851937790883648493.
	const uint32_t pd[n] = {0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000a6f7, 0x0000cef5, 0x000017bc, 0x0000e6b2, 0x0000c093, 0x000018d2, 0x0000e7ae, 0x00009f68}; // p*d
	uint32_t qHat;
	uint32_t rHat;
	uint32_t c; // Conditional multiplier used in place of conditional branches to aid in constant-time.
	uint32_t s; // Switch used on the conditional multiplier.

// ---------- Multiplication Variables ----------
	uint32_t w[n*2];

// ---------- General Variables ----------
	uint32_t u[(n*2) + 2]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
	uint32_t v[n + 1]; // v[0] is used for carry / borrow.

	uint32_t carry; // carry is used for addition carries, multiplication carries, and division remainders.
	static const constexpr uint32_t base = 0x00010000;

	void prepareDoubleIn(const uint32_t*);
	void prepareIn(const uint32_t*, const uint32_t*);

	void base16ModInternal();

	void quickAMod(); // Used after addition, and assumes that both addends are less than p.

	void prepareOut(uint32_t*);
public:
	MultiPrecisionArithmetic252ed();
	~MultiPrecisionArithmetic252ed();

	void base16Mod(uint32_t*, const uint32_t*);
	void base16Add(uint32_t*, const uint32_t*, const uint32_t*);
	void base16Mul(uint32_t*, const uint32_t*, const uint32_t*);
};


MultiPrecisionArithmetic252ed::MultiPrecisionArithmetic252ed() {

}


MultiPrecisionArithmetic252ed::~MultiPrecisionArithmetic252ed() {

}


void MultiPrecisionArithmetic252ed::prepareDoubleIn(const uint32_t* a) {
	u[1] = 0x00000000;

	for(unsigned short i = 0; i < (2*n); i += 1) {
		u[i + 2] = a[i];
	}
}


void MultiPrecisionArithmetic252ed::base16ModInternal() {
// ---------- D1 ----------
	carry = 0x00000000;

	for(unsigned short i = (m + n); i > 0; i -= 1) {
		u[i] = (d*u[i]) + carry;
		carry = u[i]/base;
		u[i] %= base;
	}

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


void MultiPrecisionArithmetic252ed::prepareOut(uint32_t* out) {
	for(unsigned short i = 0; i < n; i += 1) {
		out[i] = u[i + 2];
	}
}


void MultiPrecisionArithmetic252ed::prepareIn(const uint32_t* a, const uint32_t* b) {
	u[1] = 0x00000000;
	v[0] = 0x00000000;

	for(unsigned short i = 0; i < n; i += 1) {
		u[i + 2] = a[i];
		v[i + 1] = b[i];
	}
}


void MultiPrecisionArithmetic252ed::quickAMod() {
	c = 0x00000000;
	s = 0x00000001;

	for(unsigned short i = 2; i < (n + 2); i += 1) {
		c |= (s*(u[i] > p[i - 2]));
		s &= (!(u[i] < p[i - 2]));
	}

	c |= s;

	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 1; i -= 1) {
		u[i] -= (c*(p[i - 2] + carry));
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}
}


void MultiPrecisionArithmetic252ed::base16Mod(uint32_t* out, const uint32_t* a) {
	prepareDoubleIn(a);

	m = n + 1;
	base16ModInternal();

	prepareOut(out);
}


void MultiPrecisionArithmetic252ed::base16Add(uint32_t* out, const uint32_t* a, const uint32_t* b) {
	prepareIn(a, b);

	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	quickAMod();

	prepareOut(out);
}


void MultiPrecisionArithmetic252ed::base16Mul(uint32_t* out, const uint32_t* a, const uint32_t* b) { // Might be able to optimize by using the Karatsuba method, or some other method. Maybe within the modulus operation as well.
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
	base16ModInternal();

	prepareOut(out);
}

#endif