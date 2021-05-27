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

	unsigned long long messageCount = 0; // Used to increment a nonce for each new message sent.

	if(setupEncryption()) { // Preferably only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().
		
		messageCount = cipher.getNonceCounter(); // getNonceCounter() will typically need to be called before every encryptMessage() in order to get the nonce needed to decrypt the message. It does not need to be secret.
		cipher.encryptMessage(message, MESSAGE_BYTES);
		printMessage(message, MESSAGE_BYTES);

		cipher.decryptMessage(message, MESSAGE_BYTES, messageCount);
		printMessage(message, MESSAGE_BYTES);
	}
}


void loop() {}