#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


static ChaChaEncryption& cipher = *new ChaChaEncryption();


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	// Input message and the number of bytes in message:
	unsigned int MESSAGE_BYTES = 0;
	char message[MESSAGE_BYTES] = {""};

	if(setupEncryption()) { // Preferably only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session.
		cipher.encryptMessage(message, MESSAGE_BYTES);
		printEncryptedMessage(message, MESSAGE_BYTES);
	}
}

void loop() {}