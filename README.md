# LiteChaCha

## A Cipher Suite Made for Arduino.

---

### LiteChaCha is a basic, readable implementation of a cipher suite written for Arduino. It uses X25519 to perform ecdhe, ChaCha20 for bulk encryption and decryption, and Poly1305 for message authentication.

---

## Important Info:

  * I am not an authority with regards to cryptography. I would strongly advise you use a mature and well-tested library such as OpenSSL, LibSodium, or the Crypto library for Arduino if possible, particularly if the application is of any importance.

  * Lite ChaCha is not guaranteed to be performant, and is very likely susceptible to side-channel attacks. It is also not guaranteed to be updated. Frankly, it was made for fun, so **USE AT YOUR OWN PERIL**!

  * LiteChaCha was made to be run on the Arduino Nano 33 IoT. It may work on other microcontrollers, but it should be noted that the Arduino Nano 33 IoT uses a 32-Bit processor. Microcontrollers that are unable to handle at least 32 Bits are very likely to run far less efficiently due to the fact that LiteChaCha primarily handles data in 32 Bit words.

  * LiteChaCha is in the public domain for all to use freely.

---

## How To Use For A Peer-To-Peer Communication Application:

1. All necessary header files are within the src\include folder.

2. Include keyinfrastructure.h & authenticatedencrypt.h in your project.

3. All functions are handled in the KeyManagement & CipherManagement objects.

4. The example file, litechacha.ino, is included to show you the ropes.

   * The random ID that is generated each session **must *not* match** between peers. As such, it is recommended that you use the `IDUnique()` function as it is used in litechacha.h.

     * The ID does not need to be random, nor does it need to be generated each session. If it is stored, it need only be generated once, and confirmed to be different from all other peers' IDs.

   * It is absolutely vital that **no two messages, whether from the same user or two peers, ever be sent using the same key & nonce pair**!

     * Every time a new session is set up, a *new* shared private session key, ID (for each user), or both *must* be used. It is recommended that an unused shared private session key be used for each session.

---

## "In-Depth" Implementation Info:

### rng.h:

* The pin used for rng.h **must be floating**!

* rng.h generates random bytes using `rand()` & `srand()`. `srand()` is seeded by repeatedly calling `analogRead()` and `micros()`.

* It is hoped that sufficient entropy will be obtained by adding, XORing, rotating, and multiplying the values created by the above calls.

* rng.h is almost certainly insufficient as a random number generator! Users are encouraged to implement their own version.

### multiprecision25519.h & multiprecision1305.h:

* These were created to do multiple-precision arithmetic on 32 and 18-byte numbers respectively. They are specialized for use in X25519 & Poly1305, but it would be relatively trivial to make a generalized version for use with any sized numbers.

* Modulus, addition, multiplication, and subtraction are near constant-time. That being said, there are differences in run time depending on inputs, which is unacceptable for security reasons. If you are planning on using multiprecision25519.h or multiprecision1305.h yourself, I would advise you do some tests of your own to determine if the risks posed by side-channel attacks are not too great. As it is, this is not a safe implementation of multiple-precision arithmetic!

---

## Testing:

* All functions of LiteChaCha were tested against relevant test vectors. A text file including most of the test vectors used is provided in this repository.

* If you would like to test the ChaCha20 implementation yourself, there is a ManualTestVectorsValidation.ino example file included. It should have enough information to help you get started.

* If you would like to test the X25519 or Poly1305 implementations yourself, there is a TimeTests.ino example file included. TimeTests.ino can also be used to find the approximate runtimes of all of the components of LiteChaCha.

---

## Sources:

### multiprecision1305.h & multiprecision25519.h:

* The Art of Computer Programming, Vol. 2, Sec. 4.3.1, Algorithms A, S, M, and D. Written by Donald Knuth.

* [ResearchGate: High-speed Curve25519 on 8-bit, 16-bit, and 32-bit microcontrollers](https://www.researchgate.net/publication/277940984_High-speed_Curve25519_on_8-bit_16-bit_and_32-bit_microcontrollers). Written by Michael Düll, Björn Haase, Gesine Hinterwälder, Michael Hutter, Christof Paar, Ana Helena Sánchez, and Peter Schwabe.

  * Could be used for potential future optimizations.

### X25519.h:

* [rfc7748](https://datatracker.ietf.org/doc/html/rfc7748). Written by Adam Langley, Mike Hamburg, and Sean Turner.

* [Curve25519: new Diffie-Hellman speed records](https://cr.yp.to/ecdh/curve25519-20060209.pdf). Written by Daniel J. Bernstein.

* [rfc8031](https://datatracker.ietf.org/doc/html/rfc8031). Written by Yoav Nir and Simon Josefsson.

  * Used primarily for additional test vectors.

* [Efficient Constant Time Conditional Branching in the Montgomery Ladder](https://eprint.iacr.org/eprint-bin/getfile.pl?entry=2019/1410&version=20191205:080951&file=1410.pdf). Written by Kaushik Nath and Palash Sarkar.

  * Food for thought. Could be used for potential future optimizations.

### chacha.h:

* [rfc8439](https://datatracker.ietf.org/doc/html/rfc8439). Written by Yoav Nir and Adam Langley.

* [ChaCha, a variant of Salsa20](https://cr.yp.to/chacha/chacha-20080128.pdf). Written by Daniel J. Bernstein.

* [Section 3.2 of rfc5116](https://datatracker.ietf.org/doc/html/rfc5116#section-3.2). Written by David A. McGrew.

  * Used as a template for the nonce construction.

### poly1305.h:

* [rfc8439](https://datatracker.ietf.org/doc/html/rfc8439). Written by Yoav Nir and Adam Langley.

* [The Poly1305-AES message-authentication code](https://cr.yp.to/mac/poly1305-20050329.pdf). Written by Daniel J. Bernstein.

---

## Future plans:

1. Digital Signature Authentication using RSA or ECDSA.

2. Optimize multi-precision implementations.

3. Optimize X25519 implementation.