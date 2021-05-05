#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


/*
EXAMPLE OF LAST TEST VECTOR IN TestVectors.txt:

1. Change the message below to:
{"'Twas brillig, and the slithy toves.Did gyre and gimble in the wabe:.All mimsy were the borogoves,.And the mome raths outgrabe."}

2. Change the MESSAGE_BYTES below to 127

3. Change the startBlock below to 42

4. Change the nonceIncrement below to 2;

5. Run the program on an Arduino.

6. For the private key, input:
1c9240a5eb55d38af333888604f6b5f0473917c1402b80099dca5cbc207075c0

7. For your unique nonce, input:
00000000

8. For the other user's unique nonce, input any four-byte, HEX value that is differnt than your unique nonce. Such as:
00000001

9. Validate that the ciphertexts match.
*/


static ChaChaEncryption& cipher = *new ChaChaEncryption();


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	// User inputs:
	unsigned long long MESSAGE_BYTES = 0;
	char message[MESSAGE_BYTES] = {""};
	unsigned long startBlock = 0;
	unsigned short nonceIncrement = 0;

	if(setupEncryption()) { // Preferably only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().

		for(unsigned short i = 0; i < nonceIncrement; i += 1) {
			cipher.incrementNonceCounter();
		}

		cipher.encryptMessage(message, MESSAGE_BYTES, startBlock); // startBlock is an optional parameter. Its default value is 0.
		printMessage(message, MESSAGE_BYTES);
	}
}


void loop() {}