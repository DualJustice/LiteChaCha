#include "Arduino.h"
#include "HardwareSerial.h"

#include "src\include\multiprecision.h"


/*
0 ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a + b) % p is a D5 negative condition! It works as intended.
0 ffffffff ffffffff 00000000 00000000 00000000 00000000 00000000 00000000 = a & b: (a * b) % p is a D5 negative condition! It works as intended.
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

		Serial.print(' ');
		for(unsigned short i = 0; i < 8; i += 1) {
			Serial.print(c[i], HEX);
			Serial.print(' ');
		}
		Serial.println('\n');
	}
}


void loop() {}