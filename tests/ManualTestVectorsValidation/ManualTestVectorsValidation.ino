#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


/*
EXAMPLE OF LAST TEST VECTOR IN TestVectors.txt:

1. Change the message below to:
{"'Twas brillig, and the slithy toves.Did gyre and gimble in the wabe:.All mimsy were the borogoves,.And the mome raths outgrabe."}

2. Change the MESSAGE_BYTES below to 127

3. In chacha.h (tests\src\include), change initialBlockCounter to {0x0000002a} (42 in HEX)

4. In chacha.h (tests\src\include), change nonce to {0x00000000, 0x00000000, 0x00000002}

5. Run the program on an Arduino.

6. For the private key, input:
1c9240a5eb55d38af333888604f6b5f0473917c1402b80099dca5cbc207075c0

7. For your unique nonce, input:
00000000

8. For the other user's unique nonce, input any four-byte, HEX value that is differnt than your unique nonce. Such as:
00000001

9. Validate that the ciphertexts match.

10. Change initialBlockCounter & nonce in chacha.h back to {0x00000000} & {0x00000000, 0x00000000, 0x00000000} respectively.
*/


static ChaChaEncryption& cipher = *new ChaChaEncryption();


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	// Input message and the number of bytes in message:
	unsigned int MESSAGE_BYTES = 127;
	char message[MESSAGE_BYTES] = {"'Twas brillig, and the slithy toves.Did gyre and gimble in the wabe:.All mimsy were the borogoves,.And the mome raths outgrabe."};

	if(setupEncryption()) { // Only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session.
		cipher.encryptMessage(message, MESSAGE_BYTES);

		//printLastEndState(cipher.getLastEndState());
		//printLastKeyStream(cipher.getLastKeyStream());
		//printLastCipherText(cipher.getLastCipherText());
		printEncryptedMessage(message, MESSAGE_BYTES);
	}
}

void loop() {}