#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/chacha.h"

void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	constructStartState();
	chacha();
	printKeyStream();
}

void loop() {}