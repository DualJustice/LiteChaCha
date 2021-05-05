#include "Arduino.h"
#include "HardwareSerial.h"

#include "src/include/tempfuncs.h"
#include "src/include/chacha.h"


static ChaChaEncryption& cipher = *new ChaChaEncryption();


/*
Key: 0000000000000000000000000000000000000000000000000000000000000001
Nonce of message of interest: 00000000 00000000 00000002 (3rd sent message)
initial block counter of message of interest: 1

Message of interest:
375 bytes -
"Any submission to the IETF intended by the Contributor for publication as all or part of an IETF Internet-Draft or RFC and any statement made within the context of an IETF activity is considered an "IETF Contribution". Such statements include oral statements in IETF sessions, as well as written and electronic communications made at any time or place, which are addressed to"

To account for initial block counter:
439 bytes -
"0000000000000000000000000000000000000000000000000000000000000000Any submission to the IETF intended by the Contributor for publication as all or part of an IETF Internet-Draft or RFC and any statement made within the context of an IETF activity is considered an "IETF Contribution". Such statements include oral statements in IETF sessions, as well as written and electronic communications made at any time or place, which are addressed to"
*/


void setup() {
	Serial.begin(BAUD_RATE);
	while(!Serial) {
		delay(250);
	}

	if(setupEncryption()) { // Preferably only run once per session.
		cipher.buildEncryption(userKeyHex, userFixedNonceHex, peerFixedNonceHex); // Preferably only run once per session. Always run immediately after setupEncryption().

		unsigned long long messageCount = 0;

		// Input message and the number of bytes in message:
		unsigned long long MESSAGE1_BYTES = 1;
		char message1[MESSAGE1_BYTES] = {"b"};
		unsigned long long MESSAGE2_BYTES = 2;
		char message2[MESSAGE2_BYTES] = {"cc"};
		unsigned long long MESSAGE3_BYTES = 439;
		char message3[MESSAGE3_BYTES] = {"0000000000000000000000000000000000000000000000000000000000000000Any submission to the IETF intended by the Contributor for publication as all or part of an IETF Internet-Draft or RFC and any statement made within the context of an IETF activity is considered an \"IETF Contribution\". Such statements include oral statements in IETF sessions, as well as written and electronic communications made at any time or place, which are addressed to"};

		cipher.encryptMessage(message1, MESSAGE1_BYTES);

		cipher.encryptMessage(message2, MESSAGE2_BYTES);

		messageCount = cipher.getNonceCounter();
		cipher.encryptMessage(message3, MESSAGE3_BYTES);
		printMessage(message3, MESSAGE3_BYTES);
		cipher.decryptMessage(message3, MESSAGE3_BYTES, messageCount);
		printMessage(message3, MESSAGE3_BYTES);
	}
}


void loop() {}