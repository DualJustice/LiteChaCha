#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


static ChaChaEncryption& cipher = *new ChaChaEncryption();


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	// Input message and the number of bytes in message:
	unsigned long long MESSAGE_BYTES = 0;
	char message[MESSAGE_BYTES] = {""};
	unsigned long startBlock = 0;

	if(setupEncryption()) { // Preferably only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().
		cipher.encryptMessage(message, MESSAGE_BYTES, startBlock = 0);
		printEncryptedMessage(message, MESSAGE_BYTES);
	}
}


void loop() {}