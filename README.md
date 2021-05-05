# LiteChaCha

### Version 0.1a

## An Implementation Of The ChaCha20 Stream-Cipher, Made For [***glEEmail***](https://github.com/Matt-and-Gib/gleemail).

* LiteChaCha is a basic implementation of the ChaCha20 stream-cipher, written for Arduino. I am not an authority with regards to cryptography. I would strongly advise you use a mature and well-tested library such as OpenSSL, LibSodium, or the Crypto library for Arduino if possible, particularly if the application is of any importance.

  * It should be noted that there is currently no one-time authenticator (such as Poly1305) or key management included with LiteChaCha. These are expected to be added in a later version. Because LiteChaCha does not yet use any form of message authentication it is **NOT SAFE TO USE** in any network application other than purely hobbyist applications.

* LiteChaCha was constructed using the method laid out in [Daniel J. Bernstein's paper](https://cr.yp.to/chacha/chacha-20080128.pdf) on the cipher, as well as in the [IRTF's document](https://tools.ietf.org/html/rfc8439) on ChaCha20. It was tested against the test vectors provided [here](https://tools.ietf.org/html/rfc8439#appendix-A.1) in the IRTF document.

* The test vectors used to validate LiteChaCha are provided in a text file included in this repository. If you would like to test LiteChaCha yourself, there is a `ManualTestVectorsValidation.ino` example file included. It should have enough information to help you get started.

* LiteChaCha was made to be run on the Arduino Nano 33 IoT. It may work on other microcontrollers, but it should be noted that the Arduino Nano 33 IoT uses a 32Bit processor. Microcontrollers that are unable to handle 32 Bits or more at a time will run far less efficiently due to the fact that the ChaCha20 steam-cipher primarily handles data in 32 Bit words.

* LiteChaCha is in the public domain for all to use freely.

## How to use for a network application:

1. Include the chacha.h & tempfuncs.h header files in your project of choice.

2. An example chacha.ino file is included to show you the ropes.

3. When `setupEncryption()` is called, the user will be suggested a unique nonce and key for that session. These values are generated using the `rand()` and `srand(micros())` functions.

   * The **key** used for that session **must match** for both users.

   * The **nonce** used for that session **must *not* match** for both users.

   * Because no key management is currently included with LiteChaCha, these values must be manually shared between users.

   * It is absolutely vital that **no two messages, whether from the same user or two different users, ever be sent using the same key & nonce pair**! The nonce used in LiteChaCha includes a counter that increments with every message sent to help mitigate this. But, it is initialized at zero at the start of every session. As such:

     * Every time a new session is set up, a **new** key, nonce (for each user), or both **must** be used.

## Future plans:

* Message authentication (HMAC, Poly1305, etc.).
* Storage of information such as keys used, nonces used, and number of messages sent, to ensure safety.
* Automatic key management / exchange.
