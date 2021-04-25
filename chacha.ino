#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/chacha.h"


static const unsigned short BAUD_RATE = 9600;
static ChaChaEncryption& cipher = *new ChaChaEncryption();


void printEndState(uint32_t* endState) {
	Serial.print("endState: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(endState[i], HEX);
	}

	Serial.println();
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	cipher.encryptMessage();
	printEndState(cipher.getEndState());
}

void loop() {}