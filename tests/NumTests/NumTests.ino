#include "Arduino.h"
#include "HardwareSerial.h"

//#include "src\include\multiprecision25519.h"
//#include "src\include\multiprecision1305.h"
#include "src\include\multiprecision252ed.h"


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	MultiPrecisionArithmetic252ed order;

	uint32_t a[32] = {0x00008000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
	uint32_t b[16] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};

	order.base16Mod(b, a);

	Serial.print("b:");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(' ');
		Serial.print(b[i], HEX);
	}
	Serial.println('\n');
}


/*
0 ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a + b) % p is a D5 negative condition! It works as intended.
0 ffffffff ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a * b) % p is a D5 negative condition! It works as intended.
*/
/*
---------- What to expect ----------
The largest expected block: 	0001 ffff ffff ffff ffff ffff ffff ffff ffff
The largest expected a:			0003 ffff ffff ffff ffff ffff ffff ffff fffa
The largest expected sum:		0005 ffff ffff ffff ffff ffff ffff ffff fff9
Above sum*d:					c005 ffff ffff ffff ffff ffff ffff ffff 1ff9

The largest expected product:	0003 ffff ffff ffff ffff ffff ffff ffff fff6 0000 0000 0000 0000 0000 0000 0000 0006
Above product*d:				8003 ffff ffff ffff ffff ffff ffff fffe bff6 0000 0000 0000 0000 0000 0000 0000 c006
*/
/*
void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	MultiPrecisionArithmetic1305 math;

	uint32_t a[4];
	uint32_t b[4];
	uint32_t aMulti[9];
	uint32_t bMulti[9];

	unsigned long timeStamp;
	unsigned long duration;

//	for(unsigned short t1 = 0; t1 < 3; t1 += 1) {
		duration = 0;

//		for(unsigned short t2 = 0; t2 < 500; t2 += 1) {

//			a[0] = 0xabababab; a[1] = 0xabababab; a[2] = 0xabababab; a[3] = 0xabababab; a[4] = 0xabababab; a[5] = 0xabababab; a[6] = 0xabababab; a[7] = 0xabababab;
//			b[0] = 0xabababab; b[1] = 0xabababab; b[2] = 0xabababab; b[3] = 0xabababab; b[4] = 0xabababab; b[5] = 0xabababab; b[6] = 0xabababab; b[7] = 0xabababab;
//		             57575757           57575757           57575757           57575757           57575757           57575757           57575757           5757577C = (a + b) % p.
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


			a[0] = 0xffffffff; a[1] = 0xffffffff; a[2] = 0xffffffff; a[3] = 0xffffffff;
			b[0] = 0xffffffff; b[1] = 0xffffffff; b[2] = 0xffffffff; b[3] = 0xfffffffa;
//					1 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFC
//					1 FFFF FFFF FFFF FFFF FFFF FFFF FFFF FFFC

			math.base32_16(aMulti, a);
			math.base32_16(bMulti, b);

			aMulti[0] = 0x00000001;
			bMulti[0] = 0x00000003;

			timeStamp = micros();
//			math.base16Add(aMulti, aMulti, bMulti);
			math.base16Mul(aMulti, aMulti, bMulti);
//			math.base16AddNoMod(aMulti, aMulti, bMulti);
			duration += (micros() - timeStamp);

			Serial.print("aMulti: ");
			for(unsigned short i = 0; i < 9; i += 1) {
				Serial.print(aMulti[i], HEX);
				Serial.print(' ');
			}
			Serial.println('\n');

			math.base16_32(a, aMulti);
//		}

		Serial.print("micros: ");
		Serial.print(duration);
		Serial.println('\n');

		Serial.print("a: ");
		Serial.print(aMulti[0], HEX);
		Serial.print(' ');
		for(unsigned short i = 0; i < 4; i += 1) {
			Serial.print(a[i], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');
//	}
}
*/

void loop() {}