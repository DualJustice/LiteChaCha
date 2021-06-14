#include "Arduino.h"
#include "HardwareSerial.h"

#include "stdint.h"


// fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe0000000000000000000000000000000000000000000000000000000000000001 is the largest expected (HEX) value. (0xffffffff ...)^2

// 1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected (HEX) sum (257 bits).
// 0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits, because 7fffffff HEX is 01111111111111111111111111111111 BIN).
// 0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.

// 80000000 = 10000000000000000000000000000000
// 7fffffff = 01111111111111111111111111111111


bool isGreaterThan(uint32_t* A, uint32_t* p) {
	if(A[0] > p[0]) {
		return true;
	}
	for(unsigned short i = 1; i < 7; i += 1) {
		if(A[i] != p[i]) {
			return false;
		}
	}
	if(A[7] > p[7]) {
		return true;
	}
	return false;
}


bool isEqualTo(uint32_t* A, uint32_t* p) {
	for(unsigned short i = 0; i < 8; i += 1) {
		if(A[i] != p[i]) {
			return false;
		}
	}
	return true;
}


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}


	uint32_t A[8];
	uint32_t B[8];
	uint32_t C[8];
	uint32_t p[8];

	uint32_t temp;
	char carry;

//	B[0] = 0x08080808; B[1] = 0x00000000; B[2] = 0x00000000; B[3] = 0xffffffff; B[4] = 0xffffffff; B[5] = 0x00000000; B[6] = 0x00000000; B[7] = 0xffffffff;
//	C[0] = 0x09090910; C[1] = 0x00000000; C[2] = 0x00000000; C[3] = 0x00000000; C[4] = 0xffffffff; C[5] = 0x00000000; C[6] = 0xffffffff; C[7] = 0xffffffff;

//	B[0] = 0x00000000; B[1] = 0x00000000; B[2] = 0x00000000; B[3] = 0x00000000; B[4] = 0x00000000; B[5] = 0x00000000; B[6] = 0x00000000; B[7] = 0x00000000;
//	C[0] = 0x7fffffff; C[1] = 0xffffffff; C[2] = 0xffffffff; C[3] = 0xffffffff; C[4] = 0xffffffff; C[5] = 0xffffffff; C[6] = 0xffffffff; C[7] = 0xffffffed;

//	B[0] = 0x00000000; B[1] = 0x00000000; B[2] = 0x00000000; B[3] = 0x00000000; B[4] = 0x00000000; B[5] = 0x00000000; B[6] = 0x00000000; B[7] = 0x00000001;
//	C[0] = 0xffffffff; C[1] = 0xffffffff; C[2] = 0xffffffff; C[3] = 0xffffffff; C[4] = 0xffffffff; C[5] = 0xffffffff; C[6] = 0xffffffff; C[7] = 0xffffffda;

	B[0] = 0x00000000; B[1] = 0x00000000; B[2] = 0x00000000; B[3] = 0x00000000; B[4] = 0x00000000; B[5] = 0x00000000; B[6] = 0x00000000; B[7] = 0x00000000;
//       0   7fffffff           ffffffff           ffffffff           ffffffff           ffffffff           ffffffff           ffffffff           ffffffed is p
	C[0] = 0x80000000; C[1] = 0x00000000; C[2] = 0x00000000; C[3] = 0x00000000; C[4] = 0x00000000; C[5] = 0x00000000; C[6] = 0x00000000; C[7] = 0x00000000;

// 0xed = 11101101
// 0x01 = 00000001

// 0xee = 11101110

	p[0] = 0x7fffffff; p[1] = 0xffffffff; p[2] = 0xffffffff; p[3] = 0xffffffff; p[4] = 0xffffffff; p[5] = 0xffffffff; p[6] = 0xffffffff; p[7] = 0xffffffed;

	unsigned long timestamp = 0;


// -------------------- A = (B + C) % p --------------------
	timestamp = micros();
	for(unsigned short t = 0; t < 500; t += 1) {
		A[0] = 0x00000000; A[1] = 0x00000000; A[2] = 0x00000000; A[3] = 0x00000000; A[4] = 0x00000000; A[5] = 0x00000000; A[6] = 0x00000000; A[7] = 0x00000000;

// -------------------- RELEVANT --------------------

		carry = 0x00;

		for(unsigned short i = 7; i < 8; i -= 1) {
			temp = B[i];
			A[i] = B[i] + C[i] + carry;

			if(((carry == 0x00) && (A[i] < temp)) || ((carry == 0x01) && (A[i] <= temp))) { // Carry check necessary for all 0's case and all f's case.
				carry = 0x01;
			} else {
				carry = 0x00;
			}
		}

		if((carry != 0x00) || isGreaterThan(A, p)) { // Everything below assumes that p includes a leading 0! I don't think that polynomial division can be used in place of normal modulus.
			// Do mod:
			if(carry == 0x01) {
				temp = (A[7] & 0x00000003);
				for(unsigned short i = 7; i > 0; i -= 1) {
					carry = A[i - 1] & 0x00000003; // This might not work?
					A[i] = (A[i] >> 2) | (carry << 30);
					A[i] ^= p[i];
				}
				A[0] = (A[0] >> 2) | 0x40000000;
				A[0] ^= p[0];

				for(unsigned short t = 0; t < 2; t += 1) {
					for(unsigned short i = 0; i < 7; i += 1) {
						carry = (A[i + 1] & 0x80000000) >> 31; // This might not work?
						A[i] = (A[i] << 1) | carry; // This might not work?
					}
					carry = (temp >> (1 - t)) & 0x00000001; // This might not work?
					A[7] = (A[7] << 1) | carry; // This might not work?

					if((A[0] & 0x80000000) == 0x80000000) {
						for(unsigned short i = 0; i < 8; i += 1) {
							A[i] ^= p[i];
						}
					}
				}
			} else if((A[0] & 0x80000000) == 0x80000000) {
				temp = (A[7] & 0x00000001);
				for(unsigned short i = 7; i > 0; i -= 1) {
					carry = A[i - 1] & 0x00000001; // This might not work?
					A[i] = (A[i] >> 1) | (carry << 31);
					A[i] ^= p[i];
				}
				A[0] = (A[0] >> 1);
				A[0] ^= p[0];

				for(unsigned short i = 0; i < 7; i += 1) {
					carry = (A[i + 1] & 0x80000000) >> 31; // This might not work?
					A[i] = (A[i] << 1) | carry; // This might not work?
				}
				A[7] = (A[7] << 1) | temp;

				if((A[0] & 0x80000000) == 0x80000000) {
					for(unsigned short i = 0; i < 8; i += 1) {
						A[i] ^= p[i];
					}
				}
			} else {
				for(unsigned short i = 0; i < 8; i += 1) {
					A[i] ^= p[i];
				}
			}
		} else if(isEqualTo(A, p)) {
			for(unsigned short i = 0; i < 8; i += 1) {
				A[i] = 0x00000000;
			}
		}
	}

// -------------------- RELEVANT --------------------
	timestamp = micros() - timestamp;

	Serial.print("micros: ");
	Serial.print(timestamp);
	Serial.println('\n');

	Serial.print(carry, HEX);
	Serial.print(' ');
	for(unsigned short i = 0; i < 8; i += 1) {
		Serial.print(A[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');
}


/*
#include "BigNumber.h"


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	BigNumber::begin();

	BigNumber a;
	BigNumber b;
	BigNumber c;
	BigNumber d;

	BigNumber p = "57896044618658097711785492504343953926634992332820282019728792003956564819949";
	BigNumber A24 = 121665;

	Serial.print("p: ");
	Serial.println(p);

	a = 1000;
	Serial.print("a (should be 1000): ");
	Serial.println(a);

	c = 1000;

	b = a + c;
	Serial.print("b (should be 2000): ");
	Serial.println(b);
	Serial.print("a (should be 1000): ");
	Serial.println(a);

	c = 3000;
	Serial.print("c (should be 3000): ");
	Serial.println(c);

	d = c/a;
	Serial.print("d (should be 3): ");
	Serial.println(d);
	Serial.print("c (should be 3000): ");
	Serial.println(c);
	Serial.print("a (should be 1000): ");
	Serial.println(a);

	b = b/a;
	Serial.print("b (should be 2): ");
	Serial.println(b);

	a = "123456789123456789123456789123456789123456789123456789123456789123456789123456789";
	Serial.print("a: ");
	Serial.println(a);

	b = a % p;
	Serial.println("b (should be 22421996477724801930119104195479351870985469883947857395004571021392927325521): ");
	Serial.print("             ");
	Serial.println(b);

	c = "123456789123456789123456789123456789123456789";
	d = c.pow(2);
	Serial.println("d (should be 15241578780673678546105778311537878076969977811918920046486820281054720515622620750190521)");
	Serial.print("             ");
	Serial.println(d);

	d = 2;
	a = ((c.pow(2))*d) % p;
	Serial.println("a (should be 34448047961358672903238494434736815569263625689538145824675882968395199193724): ");
	Serial.print("             ");
	Serial.println(a);

	a = (A24*a) % p;
	Serial.println("a (should be 27085274043245416359623012795841485351924544798296351024048183386182588322350): ");
	Serial.print("             ");
	Serial.println(a);

	p = 99;
	a = 30;
	b = 75;
	c = (a + b) % p;
	Serial.print("c (should be 6): ");
	Serial.println(c);

	b = 35;
	c = (a - b) % p;
	if(c < 0) {
		c = c + p;
	}
	Serial.print("c (should be 94): ");
	Serial.println(c);
}
*/


void loop() {}