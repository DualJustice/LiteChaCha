# LiteChaCha

## A Cipher Suite Made for Arduino.

---

### LiteChaCha is a basic, readable, header-only implementation of a cipher suite written for Arduino. It uses X25519 to perform ecdhe, Ed25519 for digital signatures, ChaCha20 for bulk encryption and decryption, and Poly1305 for message authentication.

---

## Important Info:

  * I am not an authority with regards to cryptography. I would strongly advise you use a mature and well-tested library such as OpenSSL, LibSodium, or the Crypto library for Arduino if possible, particularly if the application is of any importance.

  * Lite ChaCha is not guaranteed to be performant, and is very likely susceptible to side-channel attacks. It is also not guaranteed to be updated. Frankly, it was made for fun, so **USE AT YOUR OWN PERIL**!

  * LiteChaCha was made to be run on the Arduino Nano 33 IoT. It may work on other microcontrollers, but has not been validated to run on anything else. LiteChaCha is not guaranteed to run on anything other than the Arduino Nano 33 IoT.

  * LiteChaCha is in the public domain for all to use freely.

---

## How To Use:

1. All necessary header files are within the src\include folder.

2. ```
   #include "src/include/keyinfrastructure.h"
   #include "src/include/authenticatedencrypt.h"
   ```

3. All functions are handled within `KeyManagement` and `CipherManagement`.

4. The example file, `litechacha.ino`, is included to show you the ropes.

   * The random ID that is generated each session **must *not* match** between peers. As such, it is recommended that you use the `IDUnique()` function as it is used in `litechacha.h`.

     * The ID does not need to be random, nor does it need to be generated each session. If it is stored, it need only be generated once, and confirmed to be different from all other peers' IDs.

   * It is absolutely vital that **no two messages, whether from the same user or two peers, ever be sent using the same ephemeral key and nonce pair**!

     * Every time a new session is set up, a *new* shared private session key, ID (for each user), or both *must* be used. It is recommended that a new shared private session key be used for each session.

5. In order to fulfill the requirements in step 4 above, and thus to use litechacha safely, it is recommended that users have a way to store and compare keys between sessions.

6. `pki.initialize()`:

   * If `generateNewDSAKeys` is `true`, which it is by default, a new, random `userDSAPrivateKey` and corresponding `userDSAPubKey` will be output for the user to store if desired.

   * If `generateNewDSAKeys` is `false`, the user will need to input a previously stored `userDSAPrivateKey` and `userDSAPubKey` pair.

   * DSA key pairs can be used multiple times without compromising security, but should be refreshed on a regular schedule.

   * A new, random `userEphemeralPubKey` and `userID` is generated with each call.

7. An important note:

   * Your DSA public key is the final determiner of authenticity, and is what signifies to others that you are in fact who you say you are.

   * Typically your DSA public key would be validated using a certificate, or some other similar mechanism, but this is out of the scope of this project.

   * To truly ensure a secure connection, DSA public keys must be validated via some secure, out-of-band method.

8. The implementation of RNG that LiteChaCha uses to generate a random ID, private ephemeral key, and private DSA key requires there be a floating analog pin! By default, pin A0 is used. The pin that is read from can be changed in `rng.h`.

---

## "In-Depth" Implementation Info:

### rng.h:

* `rng.h` generates random bytes using `rand()` and `srand()`, which is seeded with a randomly generated number.

  * The seed is created by concatenating 32 random bits into a single value. Each bit is generated by masking out all but the last bit of a value created by calling `analogRead()` on a floating pin. Effectively, each bit is determined by whether a random number is even or odd. This method was chosen as it was conceptually simple, quick to run, and found to be adequately random after some admittedly cursory tests.

* It is likely that `rng.h` is barely sufficient, if at all, as it was not tested rigorously. Users are encouraged to implement their own version, or perform tests to determine if `rng.h` is acceptable.

### multiprecision25519.h, multiprecision252ed.h, and multiprecision1305.h:

* These were created to do multiple-precision arithmetic on 32 and 18-byte numbers. They are specialized for use in X25519, Ed25519, and Poly1305, but it would be relatively trivial to make a generalized version for use with any sized numbers.

* Modulus, addition, multiplication, and subtraction are all near constant-time. That being said, there are differences in run time depending on inputs, which is unacceptable for security reasons. If you are planning on using `multiprecision25519.h`, `multiprecision252ed.h`, or `multiprecision1305.h` yourself, I would advise you do some tests of your own to determine if the risks posed by side-channel attacks are not too great. As it is, this is not a safe implementation of multiple-precision arithmetic!

---

## Testing:

* All functions of LiteChaCha were tested against relevant test vectors. A text file including most of the test vectors used is provided in this repository.

* If you would like to test the ChaCha20 implementation yourself, there is a `ManualChaChaTests.ino` example file included. It should have enough information to help you get started.

* If you would like to test the X25519, Ed25519, or Poly1305 implementations yourself, there is a `ManualTests.ino` example file included. `ManualTests.ino` can also be used to find the approximate runtimes of these implementations.

* One note worth mentioning is that, whichever method is chosen to send data, it must be able to successfully send null characters (`0x00`). It was found during testing that some Arduino libraries which aid in the sending and receiving of data may handle null characters differentially, and any users of LiteChaCha should be aware of this.

---

## Sources:

### multiprecision25519.h, multiprecision252ed.h, and multiprecision1305.h:

* The Art of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithms A, S, M, and D. Written by Donald Knuth.

* Handbook of Applied Cryptography, Sec. 14.42. Written by A. Menezes, P. van Oorschot, and S. Vanstone.

* [ResearchGate: High-speed Curve25519 on 8-bit, 16-bit, and 32-bit microcontrollers](https://www.researchgate.net/publication/277940984_High-speed_Curve25519_on_8-bit_16-bit_and_32-bit_microcontrollers). Written by Michael Düll, Björn Haase, Gesine Hinterwälder, Michael Hutter, Christof Paar, Ana Helena Sánchez, and Peter Schwabe.

  * Could be used for potential future optimizations.

### X25519.h:

* [rfc7748](https://datatracker.ietf.org/doc/html/rfc7748). Written by Adam Langley, Mike Hamburg, and Sean Turner.

* [Curve25519: new Diffie-Hellman speed records](https://cr.yp.to/ecdh/curve25519-20060209.pdf). Written by Daniel J. Bernstein.

* [rfc8031](https://datatracker.ietf.org/doc/html/rfc8031). Written by Yoav Nir and Simon Josefsson.

  * Used primarily for additional test vectors.

* [Efficient Constant Time Conditional Branching in the Montgomery Ladder](https://eprint.iacr.org/eprint-bin/getfile.pl?entry=2019/1410&version=20191205:080951&file=1410.pdf). Written by Kaushik Nath and Palash Sarkar.

  * Food for thought. Could be used for potential future optimizations.

### SHA512.h:

* [rfc6234](https://datatracker.ietf.org/doc/html/rfc6234). Written by Donald Eastlake and Tony Hansen.

### Ed25519.h:

* [rfc8032](https://datatracker.ietf.org/doc/html/rfc8032). Written by Simon Josefsson and Ilari Liusvaara.

* [rfc7748](https://datatracker.ietf.org/doc/html/rfc7748). Written by Adam Langley, Mike Hamburg, and Sean Turner.

* [Taming the many EdDSAs](https://research.fb.com/wp-content/uploads/2020/10/Taming-the-many-EdDSAs.pdf). Written by Konstantinos Chalkias, François Garillot, and Valeria Nikolaenko.

* [It’s 255:19AM](https://hdevalence.ca/blog/2020-10-04-its-25519am). Written by Henry de Valence.

  * Gives a quickly digestable summary of non-canonical signatures.

### chacha.h:

* [rfc8439](https://datatracker.ietf.org/doc/html/rfc8439). Written by Yoav Nir and Adam Langley.

* [ChaCha, a variant of Salsa20](https://cr.yp.to/chacha/chacha-20080128.pdf). Written by Daniel J. Bernstein.

* [Section 3.2 of rfc5116](https://datatracker.ietf.org/doc/html/rfc5116#section-3.2). Written by David A. McGrew.

  * Used as a template for the nonce construction.

### poly1305.h:

* [rfc8439](https://datatracker.ietf.org/doc/html/rfc8439). Written by Yoav Nir and Adam Langley.

* [The Poly1305-AES message-authentication code](https://cr.yp.to/mac/poly1305-20050329.pdf). Written by Daniel J. Bernstein.

---

## Potential future plans:

1. AEAD construction to allow for the authentication of unencrypted data.

   * Currently, if any unencrypted data which is necessary for decryption is changed in-transit, but the accompanying ciphertext is unchanged, the MAC will show the message to be authentic. Thus, the ciphertext will be incorrectly decrypted.

2. Optimize X25519 implementation.
