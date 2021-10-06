#include "Arduino.h"
#include "HardwareSerial.h"


	#include "src\include\X25519.h"
	#include "src\include\Ed25519.h"
	#include "src\include\poly1305.h"

/*
	#include "src\include\optiX25519.h"
	#include "src\include\optiEd25519.h"
	#include "src\include\optipoly1305.h"
*/

#include <stdint.h>


/*
---------- NOTES: ----------
TO DO:
- Get run-times for ecdhe, john, and mac.
- Get run-times for optimized versions of above objects.
- Validate optimized versions.

*/


/*
---------- RESULTS: ----------


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
}


void loop() {}