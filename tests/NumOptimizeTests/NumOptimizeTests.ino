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
- Validate optimized versions. [DONE]
	- Ed25519 [DONE]
	- X25519 [DONE]
	- poly [DONE]

*/


/*
---------- RUN-TIME RESULTS: ----------
	UNOPTIMIZED TEST VECTOR 1:
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


	OPTIMIZED TEST VECTOR 1:
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


void stringToHex(char* out, char* s, unsigned short length = 32) {
	for(unsigned short i = 0; i < length; i += 1) {
		if(48 <= s[i*2] && s[i*2] <= 57) {
			s[i*2] -= 48;
		} else if(65 <= s[i*2] && s[i*2] <= 70) {
			s[i*2] -= 55;
		} else {
			s[i*2] -= 87;
		}
		out[i] = s[i*2];
		out[i] <<= 4;

		if(48 <= s[(i*2) + 1] && s[(i*2) + 1] <= 57) {
			s[(i*2) + 1] -= 48;
		} else if(65 <= s[(i*2) + 1] && s[(i*2) + 1] <= 70) {
			s[(i*2) + 1] -= 55;
		} else {
			s[(i*2) + 1] -= 87;
		}
		out[i] |= s[(i*2) + 1];
	}
}


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

/*
//	X25519 Test Vectors:
	char n[32];
	char xp[32];
	char nBuffer[65] = {"751fb4308655b476b6789b7325f9ea8cddd16a58533ff6d9e60009464a5f9d94"};
	char xpBuffer[65] = {"0be7c1f5aad87d7e448662673298a443478b859745179eaf564c79c0ef6eee25"};

	stringToHex(n, nBuffer, 32);
	stringToHex(xp, xpBuffer, 32);

	timestamp = micros();
		ecdhe.curve25519(n, xp);
	duration = micros() - timestamp;

	Serial.print("X25519 run-time: ");
	Serial.println(duration);
	Serial.print("xp: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		if(xp[i] > 0x0f) {
			Serial.print(xp[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(xp[i], HEX);
		}
	}
	Serial.println('\n');
*/
/*
//	Ed25519 Test Vectors:
	char hancock[64];
	char EdPubKey[32];
//	char EdPrivKeyBuffer[65] = {"833fe62409237b9d62ec77587520911e9a759cec1d19755b7da901b96dca3d42"}; // Used for non-FB Research Test Vectors
	char EdPrivKey[32];
	unsigned short EdMessageLength = 32;
	char EdMessageBuffer[(EdMessageLength*2) + 1] = {"39a591f5321bbe07fd5a23dc2f39d025d74526615746727ceefd6e82ae65c06f"};
	char EdMessage[EdMessageLength];
	bool EdValid;
//	char EdPubKeyBuffer[65] = {"ecffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffff"}; // Used for FB Research Test Vectors
//	char EdSigBuffer[129] = {"a9d55260f765261eb9b84e106f665e00b867287a761990d7135963ee0a7d59dca5bb704786be79fc476f91d3f3f89b03984d8068dcf1bb7dfc6637b45450ac04"}; // Used for FB Research Test Vectors

	stringToHex(EdPrivKey, EdPrivKeyBuffer); // Used for non-FB Research Test Vectors
	stringToHex(EdMessage, EdMessageBuffer, EdMessageLength); // Used for both types of Test Vectors

	timestamp = micros();
		john.sign(hancock, EdPubKey, EdPrivKey, EdMessage, true, EdMessageLength); // Used for non-FB Research Test Vectors
	duration = micros() - timestamp;

	Serial.print("Ed25519 sign run-time: ");
	Serial.println(duration);
	Serial.print("Public Key: ");
	for(unsigned short i = 0; i < 32; i += 1) {
		if(EdPubKey[i] > 0x0f) {
			Serial.print(EdPubKey[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(EdPubKey[i], HEX);
		}
	}
	Serial.println();
	Serial.print("Signature: ");
	for(unsigned short i = 0; i < 64; i += 1) {
		if(hancock[i] > 0x0f) {
			Serial.print(hancock[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(hancock[i], HEX);
		}
	}
	Serial.println();

//	stringToHex(EdPubKey, EdPubKeyBuffer); // Used for FB Research Test Vectors
//	stringToHex(hancock, EdSigBuffer, 64); // Used for FB Research Test Vectors

	timestamp = micros();
		EdValid = john.verify(EdPubKey, EdMessage, hancock, EdMessageLength); // Used for both types of Test Vectors
	duration = micros() - timestamp;

	Serial.print("Ed25519 verify run-time: ");
	Serial.println(duration);
	Serial.print("Valid? ");
	Serial.println(EdValid);

	hancock[63] += 0x01; // Used for non-FB Research Test Vectors
	EdValid = john.verify(EdPubKey, EdMessage, hancock, EdMessageLength); // Used for non-FB Research Test Vectors

	Serial.print("Invalid? ");
	Serial.println(EdValid);
	Serial.println();
*/
/*
//	poly1305 Test Vector 1:
	char tag[16];
	uint32_t polyKey[8] = {0x01000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000};
	unsigned short polyMessageLength = 256;
	char polyMessageBuffer[polyMessageLength + 1] = {0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff};
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
		if(tag[i] > 0x0f) {
			Serial.print(tag[i], HEX);
		} else {
			Serial.print('0');
			Serial.print(tag[i], HEX);
		}
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
*/
}


void loop() {}