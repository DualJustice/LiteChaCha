#include "Arduino.h"
#include "HardwareSerial.h"

#include "stdint.h"


// fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe0000000000000000000000000000000000000000000000000000000000000001 is the largest expected (HEX) value. (0xffffffff ...)^2

// 1 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff fffffffe is the largest expected (HEX) sum (257 bits).
// 0 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffed is p (255 bits, because 7fffffff HEX is 01111111111111111111111111111111 BIN).
// 0 ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffda is p*2.

// 2 7fffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffff ffffffeb is the largest expected sum + p.

// 80000000 = 10000000000000000000000000000000
// 7fffffff = 01111111111111111111111111111111

// p (BIN):
// 111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111111101101
// two's complement of p (BIN):
// 000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000010011


uint32_t p[8];
uint32_t np[8];

uint32_t a[8];
uint32_t b[8];

uint32_t temp;
char carry;
uint32_t shiftTemp;
char shiftCarry;
char shiftCount;


void add() { // Will want to check all functions for constant time!
	carry = 0x00;

	for(unsigned short i = 7; i < 8; i -= 1) {
		temp = b[i];
		a[i] += (b[i] + carry);

		if(((carry == 0x00) && (a[i] < temp)) || ((carry == 0x01) && (a[i] <= temp))) { // Carry check necessary for all 0's case and all f's case.
			carry = 0x01;
		} else {
			carry = 0x00;
		}
	}
}


bool greaterThan() {
	if(a[0] > p[0]) {
		return true;
	}
	for(unsigned short i = 1; i < 7; i += 1) {
		if(a[i] != p[i]) {
			return false;
		}
	}
	if(a[7] > p[7]) {
		return true;
	}

	return false;
}


void rShift() {
	shiftCount = 0x00;
	shiftTemp = 0x00000000;

	if(carry == 0x01) {
		shiftCount = 0x02;
	} else if((a[0] & 0x80000000) == 0x80000000) {
		shiftCount = 0x01;
	} else {
		shiftCount = 0x00;
	}

	for(char i = shiftCount; i > 0x00; i -= 0x01) {
		shiftTemp |= (a[7] & 0x00000001) << 31;
		shiftTemp >>= 1;

		for(unsigned short i = 7; i > 0; i -= 1) {
			shiftCarry = a[i - 1] & 0x00000001;
			a[i] = (a[i] >> 1) | (shiftCarry << 31);
		}
		a[0] = (a[0] >> 1) | (carry << 31);
		carry >>= 1;
	}

	shiftTemp <<= 1;
}


void nAdd() {
	carry = 0x00;

	for(unsigned short i = 7; i < 8; i -= 1) {
		temp = np[i];
		a[i] += (np[i] + carry);

		if(((carry == 0x00) && (a[i] < temp)) || ((carry == 0x01) && (a[i] <= temp))) { // Carry check necessary for all 0's case and all f's case.
			carry = 0x01;
		} else {
			carry = 0x00;
		}
	}
}


void lShift() {
	for(unsigned short i = 0; i < 7; i += 1) {
		shiftCarry = (a[i + 1] & 0x80000000) >> 31;
		a[i] = (a[i] << 1) | shiftCarry;
	}
	a[0] = (a[0] << 1) | (shiftTemp >> 31);
	shiftTemp <<= 1;
}


bool equalTo() {
	for(unsigned short i = 0; i < 8; i += 1) {
		if(a[i] != p[i]) {
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

	p[0] = 0x7fffffff; p[1] = 0xffffffff; p[2] = 0xffffffff; p[3] = 0xffffffff; p[4] = 0xffffffff; p[5] = 0xffffffff; p[6] = 0xffffffff; p[7] = 0xffffffed;
	np[0] = 0x00000000; np[1] = 0x00000000; np[2] = 0x00000000; np[3] = 0x00000000; np[4] = 0x00000000; np[5] = 0x00000000; np[6] = 0x00000000; np[7] = 0x00000013;

	unsigned long timestamp = 0;


	timestamp = micros();
	for(unsigned short t = 0; t < 500; t += 1) {
//		a[0] = 0x08080808; a[1] = 0x00000000; a[2] = 0x00000000; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0x00000000; a[6] = 0x00000000; a[7] = 0xffffffff;
//		b[0] = 0x09090910; b[1] = 0x00000000; b[2] = 0x00000000; b[3] = 0x00000000; b[4] = 0xffffffff; b[5] = 0x00000000; b[6] = 0xffffffff; b[7] = 0xffffffff;

		a[0] = 0x7fffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff; a[4] = 0xffffffff; a[5] = 0xffffffff; a[6] = 0xffffffff; a[7] = 0xffffffed;
		b[0] = 0x7fffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xffffffff; b[4] = 0xffffffff; b[5] = 0xffffffff; b[6] = 0xffffffff; b[7] = 0xffffffed;

		add(); // Adds a and b.
		if(carry == 0x01 || greaterThan()) { // a > p?
			rShift(); // Shifts a to correct index for binary long division.

			if(shiftCount == 0x00) {
				for(unsigned short i = 0; i < 8; i += 1) {
					a[i] -= p[i];
				}
			} else if(greaterThan()) { // Check if a > p. If so, subtract p. If not shift and nadd np.
				for(unsigned short i = 0; i < 8; i += 1) {
					a[i] -= p[i];
				}
				lShift();
				if(greaterThan()) {
					for(unsigned short i = 0; i < 8; i += 1) {
						a[i] -= p[i];
					}
				}

//				nAdd();

//				if(shiftCount == 0x02) {
//					lShift();
//					nAdd();
//				}
			} else {
				lShift();
				nAdd();
			}
			if(shiftCount == 0x02) {
				lShift();
				if(greaterThan()) {
					for(unsigned short i = 0; i < 8; i += 1) {
						a[i] -= p[i];
					}
				}
			}

//			a[0] &= 0x7fffffff;

		} else if(equalTo()) {
			for(unsigned short i = 0; i < 8; i += 1) {
				a[i] = 0x00000000;
			}
		}

/*		if((carry != 0x00) || isGreaterThan(A, p)) {
			Do mod:


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
		}*/
	}
	timestamp = micros() - timestamp;


	Serial.print("micros: ");
	Serial.print(timestamp);
	Serial.println('\n');

	Serial.print(carry, HEX);
	Serial.print(' ');
	for(unsigned short i = 0; i < 8; i += 1) {
		Serial.print(a[i], HEX);
		Serial.print(' ');
	}
	Serial.println('\n');
}


void loop() {}