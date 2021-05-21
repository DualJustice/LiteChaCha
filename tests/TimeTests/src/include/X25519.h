#ifndef X25519_H
#define X25519_H

#include <stdint.h>

//#include "BigNumber.h"


//https://datatracker.ietf.org/doc/html/rfc7748
//https://cr.yp.to/ecdh/curve25519-20060209.pdf


class X25519KeyManagement {
private:
	//BigNumber p = "57896044618658097711785492504343953926634992332820282019728792003956564819949"; // (2^255) - 19
public:
	X25519KeyManagement();
	~X25519KeyManagement();

	//void startBigNum();
};


X25519KeyManagement::X25519KeyManagement() {

}


X25519KeyManagement::~X25519KeyManagement() {

}


/*
void X25519KeyManagement::startBigNum() {
	BigNumber::begin();

	BigNumber p = "57896044618658097711785492504343953926634992332820282019728792003956564819949";
}
*?


/*
Here, the "bits" parameter should be set to 255 for X25519.

	def decodeLittleEndian(b, bits):
		return sum([b[i] << 8*i for i in range((bits+7)/8)])

	def decodeUCoordinate(u, bits):
		u_list = [ord(b) for b in u]
		# Ignore any unused bits.
		if bits % 8:
			u_list[-1] &= (1<<(bits%8))-1
		return decodeLittleEndian(u_list, bits)

	def encodeUCoordinate(u, bits):
		u = u % p
		return ''.join([chr((u >> 8*i) & 0xff) for i in range((bits+7)/8)])


Scalars are assumed to be randomly generated bytes.  For X25519, in
order to decode 32 random bytes as an integer scalar, set the three
least significant bits of the first byte and the most significant bit
of the last to zero, set the second most significant bit of the last
byte to 1 and, finally, decode as little-endian.  This means that the
resulting integer is of the form 2^254 plus eight times a value
between 0 and 2^251 - 1 (inclusive).

def decodeScalar25519(k):
	k_list = [ord(b) for b in k]
	k_list[0] &= 248
	k_list[31] &= 127
	k_list[31] |= 64
	return decodeLittleEndian(k_list, 255)


To implement the X25519(k, u) function (where k is
the scalar and u is the u-coordinate), first decode k and u and then
perform the following procedure, which is taken from [curve25519] and
based on formulas from [montgomery].  All calculations are performed
in GF(p), i.e., they are performed modulo p.  The constant a24 is
(486662 - 2) / 4 = 121665 for curve25519/X25519.

	x_1 = u
	x_2 = 1
	z_2 = 0
	x_3 = u
	z_3 = 1
	swap = 0

	For t = bits-1 down to 0:
		k_t = (k >> t) & 1
		swap ^= k_t
		// Conditional swap; see text below.
		(x_2, x_3) = cswap(swap, x_2, x_3)
		(z_2, z_3) = cswap(swap, z_2, z_3)
		swap = k_t

		A = x_2 + z_2
		AA = A^2
		B = x_2 - z_2
		BB = B^2
		E = AA - BB
		C = x_3 + z_3
		D = x_3 - z_3
		DA = D * A
		CB = C * B
		x_3 = (DA + CB)^2
		z_3 = x_1 * (DA - CB)^2
		x_2 = AA * BB
		z_2 = E * (AA + a24 * E)

	// Conditional swap; see text below.
	(x_2, x_3) = cswap(swap, x_2, x_3)
	(z_2, z_3) = cswap(swap, z_2, z_3)
	Return x_2 * (z_2^(p - 2))

	(Note that these formulas are slightly different from Montgomery's
	original paper.  Implementations are free to use any correct
	formulas.)

	Finally, encode the resulting value as 32 or 56 bytes in little-
	endian order.  For X25519, the unused, most significant bit MUST be
	zero.

	The cswap function SHOULD be implemented in constant time (i.e.,
	independent of the swap argument).  For example, this can be done as
	follows:

	cswap(swap, x_2, x_3):
		dummy = mask(swap) AND (x_2 XOR x_3)
		x_2 = x_2 XOR dummy
		x_3 = x_3 XOR dummy
		Return (x_2, x_3)

	Where mask(swap) is the all-1 or all-0 word of the same length as x_2
	and x_3, computed, e.g., as mask(swap) = 0 - swap.
*/




#endif