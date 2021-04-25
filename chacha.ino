#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/chacha.h"


static const unsigned short BAUD_RATE = 9600;
static ChaChaEncryption& cipher = *new ChaChaEncryption();


void printKeyStream(uint32_t* keyStream) {
	Serial.print("keyStream: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(keyStream[i], HEX);
	}

	Serial.println();
}


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	cipher.encryptMessage();
	//cipher.constructStartState();
	//cipher.createKeyStream();
	printKeyStream(cipher.getKeyStream());
}

void loop() {}