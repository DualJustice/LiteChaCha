#include "Arduino.h"
#include "HardwareSerial.h"

/*
	#include "src\include\X25519.h"
	#include "src\include\Ed25519.h"
	#include "src\include\poly1305.h"
*/

	#include "src\include\optiX25519.h"
	#include "src\include\optiEd25519.h"
	#include "src\include\optipoly1305.h"


#include <stdint.h>


/*
---------- NOTES: ----------
TO DO:
- Integrate optimized multi libraries. [DONE]
- Get run-times for ecdhe, john, and mac. [DONE]
- Get run-times for optimized versions of above objects. [DONE]
- Validate optimized versions. []

*/


/*
---------- RESULTS: ----------
	UNOPTIMIZED:
X25519 run-time: 2148959
xp: C3DA55379DE9C6908E94EA4DF28D84F32ECCF3491C71F754B475577A28552

Ed25519 sign run-time: 7127771
Public Key: D75A98182B1AB7D54BFED3C96473AEE172F3DAA62325AF21A68F77511A
Signature: E556430C360AC729086E2CC806E828A84877F1EB8E5D974D873E06522491555FB8821590A33BACC61E39701CF9B46BD25BF5F0595BBE24655141438E7A10B
Ed25519 verify run-time: 5586044
Valid? 1

poly1305 tag run-time: 1623
Tag: A861DC1305136C6C22B8BAFC127A9
poly1305 authenticate run-time: 1626
Authentic? 1


	OPTIMIZED:
X25519 run-time: 1439470
xp: C3DA55379DE9C6908E94EA4DF28D84F32ECCF3491C71F754B475577A28552

Ed25519 sign run-time: 4728533
Public Key: D75A98182B1AB7D54BFED3C96473AEE172F3DAA62325AF21A68F77511A
Signature: E556430C360AC729086E2CC806E828A84877F1EB8E5D974D873E06522491555FB8821590A33BACC61E39701CF9B46BD25BF5F0595BBE24655141438E7A10B
Ed25519 verify run-time: 3736375
Valid? 1

poly1305 tag run-time: 1228
Tag: A861DC1305136C6C22B8BAFC127A9
poly1305 authenticate run-time: 1211
Authentic? 1


	APPROXIMATE DIFFERENCE:
optiX25519 = (0.67)X25519

optiEdSign = (0.66)EdSign
optiEdVerify = (0.67)EdVerify

optipolyTag = (0.76)polyTag [This is a maximum value, and will only decrease with larger messages.]
optipolyAuth = (0.74)polyAuth [This is a maximum value, and will only decrease with larger messages.]

*/


void setup() {
	Serial.begin(9600);
	while(!Serial) {
		delay(250);
	}

	X25519KeyExchange ecdhe;
	Ed25519SignatureAlgorithm john;
	Poly1305MAC mac;

	unsigned long timestamp = 0;
	unsigned long duration = 0;


//	X25519 Test Vector 1:
	char n[32] = {0xa5, 0x46, 0xe3, 0x6b, 0xf0, 0x52, 0x7c, 0x9d, 0x3b, 0x16, 0x15, 0x4b, 0x82, 0x46, 0x5e, 0xdd, 0x62, 0x14, 0x4c, 0x0a, 0xc1, 0xfc, 0x5a, 0x18, 0x50, 0x6a, 0x22, 0x44, 0xba, 0x44, 0x9a, 0xc4};
	char xp[32] = {0xe6, 0xdb, 0x68, 0x67, 0x58, 0x30, 0x30, 0xdb, 0x35, 0x94, 0xc1, 0xa4, 0x24, 0xb1, 0x5f, 0x7c, 0x72, 0x66, 0x24, 0xec, 0x26, 0xb3, 0x35, 0x3b, 0x10, 0xa9, 0x03, 0xa6, 0xd0, 0xab, 0x1c, 0x4c};

	timestamp = micros();
		ecdhe.curve25519(n, xp);
	duration = micros() - timestamp;

	Serial.print("X25519 run-time: ");
	Serial.println(duration);
	Serial.print("xp: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(xp[i], HEX);
	}
	Serial.println('\n');


//	Ed25519 Test Vector 1:
	char hancock[64];
	char EdPubKey[32];
	char EdPrivKey[32] = {0x9d, 0x61, 0xb1, 0x9d, 0xef, 0xfd, 0x5a, 0x60, 0xba, 0x84, 0x4a, 0xf4, 0x92, 0xec, 0x2c, 0xc4, 0x44, 0x49, 0xc5, 0x69, 0x7b, 0x32, 0x69, 0x19, 0x70, 0x3b, 0xac, 0x03, 0x1c, 0xae, 0x7f, 0x60};
	unsigned short EdMessageLength = 0;
	char EdMessageBuffer[EdMessageLength + 1] = {'\n'};
	char EdMessage[EdMessageLength];
	bool EdValid;

	for(unsigned short i = 0; i < EdMessageLength; i += 1) {
		EdMessage[i] = EdMessageBuffer[i];
	}

	timestamp = micros();
		john.sign(hancock, EdPubKey, EdPrivKey, EdMessage, true, EdMessageLength);
	duration = micros() - timestamp;

	Serial.print("Ed25519 sign run-time: ");
	Serial.println(duration);
	Serial.print("Public Key: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		Serial.print(EdPubKey[i], HEX);
	}
	Serial.println();
	Serial.print("Signature: ");
	for(unsigned short i = 0; i < 64; i += 1) {
		Serial.print(hancock[i], HEX);
	}
	Serial.println();

	timestamp = micros();
		EdValid = john.verify(EdPubKey, EdMessage, hancock, EdMessageLength);
	duration = micros() - timestamp;

	Serial.print("Ed25519 verify run-time: ");
	Serial.println(duration);
	Serial.print("Valid? ");
	Serial.println(EdValid);
	Serial.println();


//	poly1305 Test Vector 1:
	char tag[16];
	uint32_t polyKey[8] = {0x85d6be78, 0x57556d33, 0x7f4452fe, 0x42d506a8, 0x0103808a, 0xfb0db2fd, 0x4abff6af, 0x4149f51b};
	unsigned short polyMessageLength = 34;
	char polyMessageBuffer[polyMessageLength + 1] = {"Cryptographic Forum Research Group"};
	char polyMessage[polyMessageLength];
	bool polyValid;

	for(unsigned short i = 0; i < polyMessageLength; i += 1) {
		polyMessage[i] = polyMessageBuffer[i];
	}

	timestamp = micros();
		mac.createTag(tag, polyKey, polyMessage, polyMessageLength);
	duration = micros() - timestamp;

	Serial.print("poly1305 tag run-time: ");
	Serial.println(duration);
	Serial.print("Tag: ");
	for(unsigned short i = 0; i < 16; i += 1) {
		Serial.print(tag[i], HEX);
	}
	Serial.println();

	timestamp = micros();
		polyValid = mac.authenticateTag(tag, polyKey, polyMessage, polyMessageLength);
	duration = micros() - timestamp;

	Serial.print("poly1305 authenticate run-time: ");
	Serial.println(duration);
	Serial.print("Authentic? ");
	Serial.println(polyValid);
	Serial.println();
}


void loop() {}