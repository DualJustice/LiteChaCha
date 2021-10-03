#ifndef MULTIPRECISION25519_H
#define MULTIPRECISION25519_H

#include <stdint.h>


class MultiPrecisionArithmetic25519 {
private:
// ---------- Modulus Variables ----------
	static const constexpr uint32_t d = 0x00000002; // 2 is the simplest value which satisfies D1.
	unsigned short m;
	static const constexpr unsigned short n = 16;
	const uint32_t pd[n] = {0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffda}; // p*d.
	uint32_t qHat;
	uint32_t rHat;
	uint32_t c; // Conditional multiplier used in place of conditional branches to aid in constant-time.
	uint32_t s; // Switch used on the conditional multiplier.

// ---------- Barrett Variables ----------
	const uint32_t mew[n + 1] = {0x00000002, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000004c}; // (base^(2*n))/p
	uint32_t q[n + 1];

// ---------- Multiplication Variables ----------
	uint32_t w[(n*2) + 2]; // w[32] & w[33] are used in the Barrett reduction.

// ---------- Subtraction Variables ----------
	const uint32_t p[n] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed}; // (2^255) - 19.

// ---------- General Variables ----------
	uint32_t u[(n*2) + 2]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
	uint32_t v[n + 1]; // v[0] is typically used for carry / borrow.

	uint32_t carry; // carry is used for addition carries, subtraction borrows, multiplication carries, and division remainders.
	static const constexpr uint32_t base = 0x00010000;

	void prepareIn(const uint32_t*, const uint32_t*);

	void base16Mod();

	void quickAMod(); // Used after addition, and assumes that both addends are less than p.
	void quickSMod(); // Used after subtraction, and assumes that both the subtrahend and minuend are less than p.

	void barrettReduce(); // Used after multiplication, and assumes that both the multiplicand and multiplier are less than p.

	void prepareOut(uint32_t*);
	void prepareMulOut(uint32_t*); // Used after multiplication and the Barrett reduction.
public:
	MultiPrecisionArithmetic25519();
	~MultiPrecisionArithmetic25519();

	void base32_16(uint32_t*, const uint32_t*);

	void base16Add(uint32_t*, const uint32_t*, const uint32_t*);
	void base16Mul(uint32_t*, const uint32_t*, const uint32_t*);
	void base16Sub(uint32_t*, const uint32_t*, const uint32_t*);

	void base16_32(uint32_t*, const uint32_t*);
};


MultiPrecisionArithmetic25519::MultiPrecisionArithmetic25519() {

}


MultiPrecisionArithmetic25519::~MultiPrecisionArithmetic25519() {

}


void MultiPrecisionArithmetic25519::base16Mod() {
// ---------- D1 ----------
	carry = 0x00000000;

	for(unsigned short i = (m + n); i > 0; i -= 1) { // Could bit shift all words all words one two the left, with carry.
		u[i] += (u[i] + carry);
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


void MultiPrecisionArithmetic25519::base32_16(uint32_t* out, const uint32_t* a) {
	for(unsigned short i = 0; i < (n/2); i += 1) {
		out[i*2] = a[i] >> 16;
		out[(i*2) + 1] = a[i] & 0x0000ffff;
	}
}


void MultiPrecisionArithmetic25519::prepareIn(const uint32_t* a, const uint32_t* b) {
	u[1] = 0x00000000;
	v[0] = 0x00000000;

	for(unsigned short i = 0; i < n; i += 1) {
		u[i + 2] = a[i];
		v[i + 1] = b[i];
	}
}


void MultiPrecisionArithmetic25519::quickAMod() {
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


void MultiPrecisionArithmetic25519::prepareOut(uint32_t* out) {
	for(unsigned short i = 0; i < n; i += 1) {
		out[i] = u[i + 2];
	}
}


void MultiPrecisionArithmetic25519::barrettReduce() {
// ---------- 1 ----------
	for(unsigned short i = 0; i < (n + 1); i += 1) {
		v[i] = u[i + 2]; // v is storing q1.
	}

	for(unsigned short i = ((n*2) + 1); i > n; i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = n; j < (n + 1); j -= 1) {
		carry = 0x00000000;

		for(unsigned short i = n; i < (n + 1); i -= 1) {
			w[(i + j) + 1] += ((v[i]*mew[j]) + carry);
			carry = w[(i + j) + 1]/base;
			w[(i + j) + 1] %= base;
		}

		w[j] = carry;
	} // w is storing q2.

	for(unsigned short i = 0; i < (n + 1); i += 1) {
		q[i] = w[i]; // q is storing q3.
	}

// ---------- 2 ----------
	for(unsigned short i = 0; i < (n + 1); i += 1) {
		v[i] = u[i + (n + 1)]; // v is storing r1.
	}

	for(unsigned short i = (n*2); i > (n - 1); i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = (n - 1); j < n; j -= 1) {
		carry = 0x00000000;

		for(unsigned short i = n; i < (n + 1); i -= 1) {
			w[(i + j) + 1] += ((q[i]*p[j]) + carry);
			carry = w[(i + j) + 1]/base;
			w[(i + j) + 1] %= base;
		}

		w[j] = carry;
	} // w is storing q3*m.

	carry = 0x00000000;

	for(unsigned short i = (n + 2); i > 1; i -= 1) {
		u[i] = v[i - 2] - (w[i + (n - 2)] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}

	u[1] = carry; // u is storing r.

// ---------- 3 ----------
//	u[1] -= u[1]; Unnecessary step.

// ---------- 4 ----------
	for(unsigned short j = 0; j < 2; j += 1) {
		c = 0x00000000;
		s = 0x00000001;

		c |= (u[2] > 0x00000000);

		for(unsigned short i = 3; i < (n + 3); i += 1) {
			c |= (s*(u[i] > p[i - 3]));
			s &= (!(u[i] < p[i - 3]));
		}

		c |= s;

		carry = 0x00000000;

		for(unsigned short i = (n + 2); i > 2; i -= 1) {
			u[i] -= (c*(p[i - 3] + carry));
			carry = (u[i] & base)/base;
			u[i] = (u[i] & 0x0001ffff) % base;
		}

		u[2] -= carry;
	}
}


void MultiPrecisionArithmetic25519::prepareMulOut(uint32_t* out) {
	for(unsigned short i = 0; i < n; i += 1) {
		out[i] = u[i + 3];
	}
}


void MultiPrecisionArithmetic25519::quickSMod() {
	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 1; i -= 1) {
		u[i] += ((u[1]*p[i - 2]) + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	u[1] -= carry;
}


void MultiPrecisionArithmetic25519::base16Add(uint32_t* out, const uint32_t* a, const uint32_t* b) {
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


void MultiPrecisionArithmetic25519::base16Mul(uint32_t* out, const uint32_t* a, const uint32_t* b) { // Might be able to optimize by using the Karatsuba method, or some other method. Maybe within the modulus operation as well.
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

	barrettReduce();

	prepareMulOut(out);
}


void MultiPrecisionArithmetic25519::base16Sub(uint32_t* out, const uint32_t* a, const uint32_t* b) {
	prepareIn(a, b);

	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 0; i -= 1) {
		u[i] -= (v[i - 1] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}

	u[1] &= 0x00000001;

	quickSMod();

	prepareOut(out);
}


void MultiPrecisionArithmetic25519::base16_32(uint32_t* out, const uint32_t* a) {
	for(unsigned short i = 0; i < (n/2); i += 1) {
		out[i] = (a[i*2] << 16) | a[(i*2) + 1];
	}
}

#endif