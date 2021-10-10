#include "Arduino.h"
#include "HardwareSerial.h"

#include "src\include\X25519.h"
#include "src\include\Ed25519.h"
#include "src\include\poly1305.h"

#include <stdint.h>


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
//	---------- X25519 Test Vectors: ----------
	char n[32];
	char xp[32];
	char nBuffer[65] = {"a546e36bf0527c9d3b16154b82465edd62144c0ac1fc5a18506a2244ba449ac4"};
	char xpBuffer[65] = {"e6db6867583030db3594c1a424b15f7c726624ec26b3353b10a903a6d0ab1c4c"};

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
//	---------- Ed25519 Test Vectors: ----------
	char hancock[64];
	char EdPubKey[32];
	char EdPrivKeyBuffer[65] = {"9d61b19deffd5a60ba844af492ec2cc44449c5697b326919703bac031cae7f60"}; // Used for non-FB Research Test Vectors
	char EdPrivKey[32];
	unsigned short EdMessageLength = 0; // Used for both types of Test Vectors
	char EdMessageBuffer[(EdMessageLength*2) + 1] = {""}; // Used for both types of Test Vectors
	char EdMessage[EdMessageLength];
	bool EdValid;
//	char EdPubKeyBuffer[65] = {"c7176a703d4dd84fba3c0b760d10670f2a2053fa2c39ccc64ec7fd7792ac03fa"}; // Used for FB Research Test Vectors
//	char EdSigBuffer[129] = {"c7176a703d4dd84fba3c0b760d10670f2a2053fa2c39ccc64ec7fd7792ac037a0000000000000000000000000000000000000000000000000000000000000000"}; // Used for FB Research Test Vectors

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
//	---------- poly1305 Test Vectors: ----------
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