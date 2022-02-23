#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


/*
EXAMPLE OF RFC8439 TEST VECTOR #3 WITH CIPHERTEXT IN TestVectors.txt:
---------------------------------------------------------------------

Note: If no ciphertext is involved with a test vector, comment out the message, MESSAGE_BYTES, cipher.encryptMessage, all printMessages, and cipher.decryptMessage.
Then uncomment cipher.generateEndState and printLastEndState. All other steps should be done in a similar fashion to test vectors which use ciphertexts.

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

8. For the other user's unique nonce, input the same nonce used as your unique nonce (normally this would need to be a different value than your unique nonce):
00000000

9. Validate that the ciphertexts match.
*/


void setup() {
	ChaChaEncryption cipher;

	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	// User inputs:
	unsigned long long MESSAGE_BYTES = 127;
	char message[MESSAGE_BYTES] = {"'Twas brillig, and the slithy toves.Did gyre and gimble in the wabe:.All mimsy were the borogoves,.And the mome raths outgrabe."};
	unsigned long startBlock = 42;
	unsigned short nonceIncrement = 2;

	unsigned long long messageCount = 0; // Used to increment a nonce for each new message sent.

	if(setupEncryption()) { // Preferably only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().

		for(unsigned short i = 0; i < nonceIncrement; i += 1) {
			cipher.incrementNonceCounter();
		}

		messageCount = cipher.getNonceCounter(); // getNonceCounter() will typically need to be called before every encryptMessage() in order to get the nonce needed to decrypt the message. It does not need to be secret.
		cipher.encryptMessage(message, MESSAGE_BYTES, startBlock); // startBlock is an optional parameter. Its default value is 0.
		printMessage(message, MESSAGE_BYTES);
//		printLastEndState(cipher.generateEndState(startBlock));

		cipher.decryptMessage(message, MESSAGE_BYTES, messageCount, startBlock);
		printMessage(message, MESSAGE_BYTES);
	}
}


void loop() {}