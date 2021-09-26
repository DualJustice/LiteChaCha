#include "Arduino.h"
#include "HardwareSerial.h"

#include "src\include\multiprecision25519.h"
#include "src\include\multiprecision1305.h"
#include "src\include\multiprecision252ed.h"

#include <stdint.h>


unsigned short INT_LENGTH_MULTI = 16;
unsigned short DUB_LENGTH_MULTI = 2*INT_LENGTH_MULTI;


/*
RESULTS:


*/


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	MultiPrecisionArithmetic25519 math;
	MultiPrecisionArithmetic252ed order;
	MultiPrecisionArithmetic1305 poly;

	unsigned long timestamp = 0;
	unsigned long duration = 0;

	uint32_t a[INT_LENGTH_MULTI] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffec};
	uint32_t b[INT_LENGTH_MULTI] = {0x00007fff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffec};

	uint32_t d[DUB_LENGTH_MULTI] = {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff, 0x0000ffff};

	uint32_t r[INT_LENGTH_MULTI];

/*
	timestamp = micros();
	order.base16Mod(r, d);
	duration = micros() - timestamp;

	Serial.print("Mod: ");
	Serial.println(duration);
*/

	timestamp = micros();
	math.base16Add(r, a, b);
	duration = micros() - timestamp;

	Serial.print("Add: ");
	Serial.println(duration);

	Serial.print("r:");
	for(unsigned short i = 0; i < INT_LENGTH_MULTI; i += 1) {
		Serial.print(' ');
		Serial.print(r[i], HEX);
	}
	Serial.println();


	timestamp = micros();
	math.base16Sub(r, a, b);
	duration = micros() - timestamp;

	Serial.print("Sub: ");
	Serial.println(duration);


	timestamp = micros();
	math.base16Mul(r, a, b);
	duration = micros() - timestamp;

	Serial.print("Mul: ");
	Serial.println(duration);
}


void loop() {}