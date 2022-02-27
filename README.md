# LiteChaCha

## A Cipher Suite Made for Arduino.

---

### LiteChaCha is a basic, readable, header-only implementation of a cipher suite written for Arduino. It uses X25519 to perform ecdhe, Ed25519 for digital signatures, ChaCha20 for bulk encryption and decryption, and Poly1305 for message authentication.

---

## Important Info:

  * I am not an authority with regards to cryptography. I would strongly advise you use a mature and well-tested library such as OpenSSL, LibSodium, or the Crypto library for Arduino if possible, particularly if the application is of any importance.

  * LiteChaCha is not guaranteed to be performant, and is very likely susceptible to side-channel attacks. It is also not guaranteed to be updated. Frankly, it was made for fun, so **USE AT YOUR OWN PERIL**!

  * LiteChaCha was made to be run on the Arduino Nano 33 IoT. It may work on other microcontrollers, but has not been validated to run on anything else. LiteChaCha is not guaranteed to run on anything other than the Arduino Nano 33 IoT.

  * LiteChaCha is structured to be used in a communication application. It is possible to use LiteChaCha to encrypt and decrypt stored data, but you will need to implement it yourself. The tools necessary to do so can be found inside of `src/chacha.h`. It is recommended that anyone trying to use LiteChaCha in a storage application read the relevant sources below to better understand both how to use `chacha.h` as well as its limitations.

---

## How To Use:

1. All necessary header files are within the src folder.

2. ```
   #include "src/keyinfrastructure.h"
   #include "src/authenticatedencrypt.h"
   #include "src/errorflags.h"
   ```

3. All functions are handled within `KeyManagement` and `CipherManagement`.

4. The example file, `Overview.ino`, is included to show you the ropes.

   * The random ID that is generated each session **must *not* match** between peers. As such, it is recommended that you use the `IDUnique()` function as it is used in `Overview.ino`.

     * The ID does not need to be random, nor does it need to be generated each session. If it is stored, it need only be generated once, and confirmed to be different from all other peers' IDs.

	 * Note that in a communication application these IDs must not match, but in a storage application they will match.

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

8. The implementation of RNG that LiteChaCha uses to generate a random ID, private ephemeral key, and private DSA key **requires there be a floating analog pin!** By default, pin A0 is used. The pin that is read from can be changed in `rng.h`.

9. Whichever method is chosen to send encrypted data, it must be able to successfully send null characters (`0x00`). Alternatively, the encrypted data must be altered to exclude any null characters. It was found during testing that some Arduino libraries which aid in the sending and receiving of data may handle null characters differentially, and any users of LiteChaCha should be aware of this.

---

## When Errors Arise:

### There are a few places within LiteChaCha where things can go awry due to the known limitations of the algorithms used within. A summary of these errors, as well as how to deal with them, can be found here.

* These errors should effectively never occur under normal use cases. That being said, proper implementations of LiteChaCha should account for them all the same.

* This section refers to an *Establish Connection Block*. This can be found within `Overview.ino`.

* Note that each error shown below is represented as a single bit in a byte. If you are confused about how errors are handled within LiteChaCha, consider looking through `errorflags.h` to gain a better understanding.

| Error Name:                  | Error Bit:   | Where It Can Occur:                                       | What To Do:   |
| ---------------------------- | ------------ | --------------------------------------------------------- | ------------- |
| CURVE25519_ALL_ZEROS_CASE    | `0b00000001` | `KeyManagement::initialize()`, `createSessionKey()`       | See 1. Below. |
| MPA25519_MATH_ERROR          | `0b00000010` | `KeyManagement::initialize()`, `createSessionKey()`       | See 1. Below. |
| MPA252ED_MATH_ERROR          | `0b00000100` | `KeyManagement::initialize()`, `signatureValid()`         | See 2. Below. |
| CHACHA_BLOCK_COUNT_OVERFLOW  | `0b00001000` | `encryptAndTagMessage()`, `decryptAuthenticatedMessage()` | See 3. Below. |
| USER_NONCE_OVERFLOW_IMMINENT | `0b00010000` | `encryptAndTagMessage()`                                  | See 4. Below. |
| PEER_NONCE_OVERFLOW_IMMINENT | `0b00100000` | `decryptAuthenticatedMessage()`                           | See 5. Below. |
| POLY_BLOCK_COUNT_OVERFLOW    | `0b01000000` | `encryptAndTagMessage()`, `messageAuthentic()`            | See 6. Below. |
| MPA1305_MATH_ERROR           | `0b10000000` | `encryptAndTagMessage()`, `messageAuthentic()`            | See 7. Below. |

1. Both parties restart the *Establish Connection Block*. Note that new DSA key pairs are not necessary.

2. Both parties restart the *Establish Connection Block*. To guarantee success, generate new DSA key pairs as well.

3. If encrypting, discard the encrypted message. The message will need to be broken up into smaller messages. If decrypting, drop your connection. Your peer is using unsafe practices.

4. Send the current encrypted message. Both parties must restart the *Establish Connection Block* **before** encrypting and sending the next message. Note that new DSA key pairs are not necessary.

5. Both parties restart the *Establish Connection Block* after you process the current message. Note that new DSA key pairs are not necessary.

6. If tagging, discard the encrypted message and its tag. The message will need to be broken up into smaller messages. If authenticating, drop your connection. Your peer is using unsafe practices.

7. Discard the current encrypted message and its tag before sending or processing them further. Both parties restart the *Establish Connection Block*. Note that new DSA key pairs are not necessary.

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

  * All tests use outdated versions of LiteChaCha. They are functionally similar to the latest version, but include code which makes it possible to run manual tests.

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

  * Gives a quickly digestible summary of non-canonical signatures.

### chacha.h:

* [rfc8439](https://datatracker.ietf.org/doc/html/rfc8439). Written by Yoav Nir and Adam Langley.

* [ChaCha, a variant of Salsa20](https://cr.yp.to/chacha/chacha-20080128.pdf). Written by Daniel J. Bernstein.

* [Section 3.2 of rfc5116](https://datatracker.ietf.org/doc/html/rfc5116#section-3.2). Written by David A. McGrew.

  * Used as a template for the nonce construction.

### poly1305.h:

* [rfc8439](https://datatracker.ietf.org/doc/html/rfc8439). Written by Yoav Nir and Adam Langley.

* [The Poly1305-AES message-authentication code](https://cr.yp.to/mac/poly1305-20050329.pdf). Written by Daniel J. Bernstein.

---

## Potential Future Plans:

1. AEAD construction to allow for the authentication of unencrypted data.

   * Currently, if any unencrypted data necessary for decryption is changed in-transit while the accompanying ciphertext is left unchanged the MAC will show the message to be authentic. Thus the ciphertext will be incorrectly decrypted.

2. General optimizations, particularly to the implementation of X25519 and multiple-precision arithmetic.
