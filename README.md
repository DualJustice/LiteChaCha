# LiteChaCha

### Version 0.1a

## An Implementation Of The ChaCha20 Stream-Cipher, Made For [***glEEmail***](https://github.com/Matt-and-Gib/gleemail).

* LiteChaCha is a basic, readable implementation of a cryptographic protocol, written for Arduino. It uses the ChaCha20 stream-cipher, the X25519 ecdhe, and a multiple-precision arithmetic library. I am not an authority with regards to cryptography. I would strongly advise you use a mature and well-tested library such as OpenSSL, LibSodium, or the Crypto library for Arduino if possible, particularly if the application is of any importance.

  * Lite ChaCha is not guaranteed to be performant, and is very likely susceptible to side-channel attacks. It is also not guaranteed to be updated. Frankly, it was made for fun, so **USE AT YOUR OWN PERIL**!

  * It should be noted that there is currently no one-time authenticator (such as Poly1305) or key management included with LiteChaCha. These are expected to be added in a later version. Because LiteChaCha does not yet use any form of message authentication it is **NOT SAFE TO USE** in any network application other than purely hobbyist applications.

* multiprecision.h is a library created to do multiple-precision arithmetic on 32-byte numbers, specifically for ECDHE using X25519.

  * The algorithms used for modulus, addition, multiplication, and subtraction below were derived from [The Art Of Computer Programming, Vol. 2](https://doc.lagout.org/science/0_Computer%20Science/2_Algorithms/The%20Art%20of%20Computer%20Programming%20%28vol.%202_%20Seminumerical%20Algorithms%29%20%283rd%20ed.%29%20%5BKnuth%201997-11-14%5D.pdf), Sec. 4.3.1, Algorithms D, A, M, and S respectively.

  * Modulus, addition, multiplication, and subtraction are near constant-time. There are differences in run time depending on inputs, which is unacceptable for security reasons. If you are planning on using multiprecision.h yourself for public-key encryption, I would advise you do some tests of your own to determine if the risks posed by side-channel attacks are not too great.
  As it is, this is not a safe implementation of multiple-precision arithmetic for public-key encryption!

* chacha.h is a library created to run the ChaCha20 stream-cipher.

  * The ChaCha20 stream-cipher was constructed using the method laid out in [Daniel J. Bernstein's paper](https://cr.yp.to/chacha/chacha-20080128.pdf) on the cipher, as well as in the [IRTF's document](https://tools.ietf.org/html/rfc8439) on ChaCha20. It was tested against the test vectors provided [here](https://tools.ietf.org/html/rfc8439#appendix-A.1) in the IRTF document.

  * The test vectors used to validate ChaCha20 are provided in a text file included in this repository. If you would like to test LiteChaCha yourself, there is a `ManualTestVectorsValidation.ino` example file included. It should have enough information to help you get started.

   * Also included is a `TimeTests.ino` file which will help provide the encryption and decryption time of any message.

* LiteChaCha was made to be run on the Arduino Nano 33 IoT. It may work on other microcontrollers, but it should be noted that the Arduino Nano 33 IoT uses a 32Bit processor. Microcontrollers that are unable to handle 32 Bits or more at a time are very likely to run far less efficiently due to the fact that LiteChaCha primarily handles data in 32 Bit words.

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

* Automatic key management / exchange (X25519).
* Message authentication (Poly1305).
* Storage of information such as keys used, nonces used, and number of messages sent, to ensure safety.
  * Encryption of above storage.