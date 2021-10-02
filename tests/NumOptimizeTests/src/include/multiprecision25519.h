#ifndef MULTIPRECISION25519_H
#define MULTIPRECISION25519_H

#include <stdint.h>

#include "Arduino.h" // DELETE ME!
#include "HardwareSerial.h" // DELETE ME!


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
	uint32_t w[(n*2) + 1]; // INCREASED W'S SIZE BY ONE FOR BARRETT REDUCTION!

// ---------- Subtraction Variables ----------
	const uint32_t p[n] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffed}; // (2^255) - 19.

// ---------- General Variables ----------
	uint32_t u[(n*2) + 2]; // u[0] is used for u[m + n], u[1] is used for carry / borrow.
	uint32_t v[n + 1]; // v[0] is typically used for carry / borrow.

	uint32_t carry; // carry is used for addition carries, subtraction borrows, multiplication carries, and division remainders.
	static const constexpr uint32_t base = 0x00010000;

	void prepareIn(const uint32_t*, const uint32_t*);

	void quickAMod(); // Used after addition, and assumes that both addends are less than p.
	void quickSMod(); // Used after subtraction, and assumes that both the subtrahend and minuend are less than p.
	void base16Mod();

	void barrettReduce(); // Used after multiplication, and assumes that both the multiplicand and multiplier are less than p.

	void prepareOut(uint32_t*);
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


void MultiPrecisionArithmetic25519::base16Mod() {
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


void MultiPrecisionArithmetic25519::barrettReduce() {
// ---------- 1 ---------- q = u >> 240. 240/16 = 15. 32 - 15 = 17. AKA, q[17 to 33] = u[2 to 18]. OR, v[0 to 16] = u[2 to 18].
	for(unsigned short i = 0; i < (n + 1); i += 1) {
		v[i] = u[i + 2]; // v is storing q1.
	}
//	q2 = q1*mew, q1 is 17 long. mew is 17 long. q must be 34 long. Gonna use j for mew and i for q.
/*
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
*/

	for(unsigned short i = (n*2); i > (n - 1); i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = n; j >= 0; j -= 1) {
		carry = 0x00000000;

		for(unsigned short i = n; i >= 0; i -= 1) {
			w[i + j] += ((v[i]*mew[j]) + carry);
			carry = w[i + j]/base;
			w[i + j] %= base;
		}
	} // w is storing q2.

//	q = q >> 272. 272/16 = 17. 34 - 17 = 17. AKA, q[17 to 33] = q[0 to 16]. More accurately, q3 = w[0 to 16].

	for(unsigned short i = 0; i < (n + 1); i += 1) {
		q[i] = w[i]; // q is storing q3.
	}

// ---------- 2 ---------- r1 = u % 2^272. Strip all but the lowest 272 bits of u. 272/16 = 17. r1 = u[17 to 33].
	for(unsigned short i = 0; i < (n + 1); i += 1) {
		v[i] = u[i + (n + 1)]; // v is storing r1.
	}

//	r2 = (q3*p) % 2^272. Use j for p, and i for q3.

	for(unsigned short i = ((n*2) - 1); i > (n - 2); i -= 1) {
		w[i] = 0x00000000;
	}

	for(unsigned short j = (n - 1); j >= 0; j -= 1) {
		carry = 0x00000000;

		for(unsigned short i = n; i >= 0; i -= 1) {
			w[i + j] += ((q3[i]*p[j]) + carry);
			carry = w[i + j]/base;
			w[i + j] %= base;
		}
	} // w is storing q3*m.

//	result = v - w[16 to 32]. Result can be negative!
/*
	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 0; i -= 1) {
		u[i] -= (v[i - 1] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}

	u[1] &= 0x00000001;
*/

	carry = 0x00000000;

	for(unsigned short i = (n + 2); i > 1; i -= 1) {
		u[i] = v[i - 2] - (w[i + 14] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}

	u[1] = carry; // u is storing r.

// ---------- 3 ----------
/*
	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 1; i -= 1) {
		u[i] += ((u[1]*p[i - 2]) + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	u[1] -= carry;
*/
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


void MultiPrecisionArithmetic25519::prepareOut(uint32_t* out) {
	for(unsigned short i = 0; i < n; i += 1) {
		out[i] = u[i + 2];
	}
}


/*
Given a + b = c:

amax =	 ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff
bmax =	 ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff
cmax = 1 ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff fffe

p	 =	 7fff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffed

cmax/p = 4.000...000127...

(((2^256) - 1)*2) - (4*((2^255) - 19)) == cmax - 4p = 74.

Therefore: You may need to quick subtract 0, 1p, 2p, 3p, or 4p.

0  : c < 1p
1p : 1p <= c < 2p
2p : 2p <= c < 3p
3p : 3p <= c < 4p
4p : 4p <= c

Instead of checking 5 conditions for every addition and subtraction, make a single preparatoryMod() function that acts on all values derived from outside inputs.

Then...

Assume 0 <= a & b < p

Given a + b = c:

amax =	 7fff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffec
bmax =	 7fff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffec
cmax =	 ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffd8

p	 =	 7fff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffff ffed

cmax - 1p < p !

Therefore: You may need to quick subtract 0 or 1p.

0  : c < 1p
1p : 1p <= c
*/


void MultiPrecisionArithmetic25519::base16Add(uint32_t* out, const uint32_t* a, const uint32_t* b) { // Might be able to optimize by combining some steps.
	prepareIn(a, b);

	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 0; i -= 1) {
		u[i] += (v[i - 1] + carry);
		carry = u[i]/base;
		u[i] %= base;
	}

	quickAMod();

/*
	m = 1;
	base16Mod();
*/

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

	m = n + 1;
	base16Mod();

	prepareOut(out);
}


void MultiPrecisionArithmetic25519::base16Sub(uint32_t* out, const uint32_t* a, const uint32_t* b) { // Might be able to optimize by removing base16Mod().
	prepareIn(a, b);

	carry = 0x00000000;

	for(unsigned short i = (n + 1); i > 0; i -= 1) {
		u[i] -= (v[i - 1] + carry);
		carry = (u[i] & base)/base;
		u[i] = (u[i] & 0x0001ffff) % base;
	}

	u[1] &= 0x00000001;

	quickSMod();

/*
	for(unsigned short j = 0; j < 3; j += 1) {
		carry = 0x00000000;

		for(unsigned short i = (n + 1); i > 1; i -= 1) {
			u[i] += ((u[1]*p[i - 2]) + carry);
			carry = u[i]/base;
			u[i] %= base;
		}

		u[1] -= carry;
	}

	m = 1;
	base16Mod();
*/

	prepareOut(out);
}


void MultiPrecisionArithmetic25519::base16_32(uint32_t* out, const uint32_t* a) {
	for(unsigned short i = 0; i < (n/2); i += 1) {
		out[i] = (a[i*2] << 16) | a[(i*2) + 1];
	}
}

#endif