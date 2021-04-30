# LiteChaCha

### Version 0.1a

## An Implementation Of The ChaCha20 Stream-Cipher, Made For [***glEEmail***](https://github.com/Matt-and-Gib/gleemail).

* LiteChaCha is a basic implementation of the ChaCha20 stream-cipher, written for Arduino. I am not an authority with regards to cryptography. I would advise you use a mature and well-tested library such as OpenSSL, LibSodium, or the Crypto library for Arduino if possible, particularly if the application is of any importance.

  * It should be noted that there is currently no one-time authenticator (such as Poly1305) or key management included with LiteChaCha. These are expected to be added in a later version.

* LiteChaCha was constructed using the method laid out in [Daniel J. Bernstein's paper](https://cr.yp.to/chacha/chacha-20080128.pdf) on the cipher, as well as in the [IRTF's document](https://tools.ietf.org/html/rfc8439) on ChaCha20. It was tested against the test vectors provided in [here](https://tools.ietf.org/html/rfc8439#appendix-A.1) in the previous document.

* The test vectors used to validate LiteChaCha are provided in a text file included in this repository. If you would like to test this implementation yourself, here are a few useful **rules of thumb**:

   1. Keep Little-endian in mind. When converting a stream of bytes into a word, the order matters. For example:

      *Key*:  
      `000  00 ff 00 00 00 00 00 00 00 00 00 00 00 00 00 00`  
      `016  00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00`

      *will equal*:  
      `0x0000ff00, 0x00000000, ...`

   2. Little-endian order is important for the block counter as well. For example:

      Block counter = `0` *equals* `0x00000000`  
      Block counter = `2` *equals* `0x00000002`

* LiteChaCha was made to be run on the Arduino Nano 33 IoT. It may work on other microcontrollers, but it should be noted that the processor running this program must be Little-endian in order to execute properly. It should also be noted that the Arduino Nano 33 IoT uses a 32Bit microcontroller. Processors with a smaller bus width than 32 Bits will run far less efficiently due to the fact that the ChaCha20 steam-cipher primarily handles data in 32 Bit words.

* LiteChaCha is in the public domain for all to use freely.

## How to use:

1. Include the chacha.h header file in your project of choice.

2. An example .ino file is included to show you the ropes.

3. When `setupEncryption()` is called, the user will be suggested a unique nonce and key for that session. These values are generated using the `rand()` and `srand(micros())` functions.

   * The **key** used for that session **must match** for both users.
   * The **nonce** used for that session **must *not* match** for both users.
   * Because no key management is currently included with LiteChaCha, these values must manually be shared between users.
   * Every time a new session is set up, a **new** key, nonce (for both users), or both **must** be used.

## Setup:

1. To Do.

## Future plans:

* Automatic key management / exchange.
* Message authentication (HMAC, Poly1305, etc.).
* Storage of information such as keys used, nonces used, and number of messages sent, to ensure safety.