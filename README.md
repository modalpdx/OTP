# OTP
School project that simulates a "One Time Pad" encryption client and server.

How One Time Pad encryption works:

In a One Time Pad encryption exchange, the sender and receiver have
identical pads containing pages filled with random letters. The sender
will use the top page of his/her pad to encrypt a message, send the
message to the receiver, then destroy the page from the pad that was used
to encrypt the message. The receiver uses his/her pad's top page
(identical to the one the sender just destroyed) to decrypt the message
and then destroys the top page of his/her pad. Then a response is sent
using the same process, but this time the receiver becomes the sender and
the former sender becomes a receiver. At all times, the pages on the pad
must be destroyed in sets for the pads to remain in sync.

The encryption process takes the numeric value of each character in the
message and adds it to the character in the same position on the
page from the pad (the one with random characters on it), then the modulus
is calculated from the final sum divided by the number of possible
characters in the cipher. The letter represented by the numeric value of
the modulus is the letter that goes into the encrypted message (so, if the
modulus is 4, the letter would be E, assuming we're starting at 0).

Example from Wikipedia that converts "HELLO" into "EQNVZ" 
(https://en.wikipedia.org/wiki/One-time_pad):

```
      H       E       L       L       O  message
   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) message
+ 23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key
= 30      16      13      21      25     message + key
=  4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) message + key (mod 26)
      E       Q       N       V       Z  → ciphertext
```

Decryption is almost the reverse of this process, but not quite. Instead
of adding the numeric values of positionally-identical characters, the
character from the key is subtracted. If the result is negative, add the
number of possible characters in the character set to bring everything
back into positive number territory. The modulus of that number and the
total number of possible characters is calculated, and the letter
represented by the numeric value of that modulus should be the decrypted
character.

Example from Wikipedia that converts "EQNVZ" back to "HELLO":

```
       E       Q       N       V       Z  ciphertext
    4 (E)  16 (Q)  13 (N)  21 (V)  25 (Z) ciphertext
-  23 (X)  12 (M)   2 (C)  10 (K)  11 (L) key
= -19       4      11      11      14     ciphertext – key
=   7 (H)   4 (E)  11 (L)  11 (L)  14 (O) ciphertext – key (mod 26)
       H       E       L       L       O  → message
```

How these programs work:

There are 5 programs in the set:

- keygen: creates a "page" of random characters. You specify the number of
  random characters to generate on the command line.

- otp_enc_d, otp_dec_d: server programs that encrypt and decrypt,
  respectively.

- otp_enc, otp_dec: client programs that send messages for encryption or
  receive messages that have been decrypted, respectively.

You first create a key "page" using keygen. Due to the nature of OTP
encryption (it's all position-based) the size of the key "page" must be
greater than the size of the message to encrypt. Then the otp_enc client
program is then used to send the key "page" and the message to the
otp_enc_d server for encryption. The otp_enc_d server sends back the
encrypted message which is then output to stdout (redirect to a file to
capture the content). The otp_dec client program can then be used to send
the same key "page" and the encrypted message to the otp_dec_d server for
decryption. The otp_dec_d server sends back the decrypted message which is
then output to stdout (redirect to a file to capture the content).

Build:

Download everyting and run 'make'. Command line help is offered by running
a program without passing any command line arguments (they all require
arguments, even the two servers).

Colophon:

This suite of programs was written with standards in mind but was only
developed and tested on Linux and Mac OS. Mostly Linux.

GNU Make and GCC are required. The programs are 100% C, no C++ or other C
variants.

Disclaimer:

This is code from a school project. It satisfies assignment requirements
but is nowhere near as "scrubbed" as a released software suite should be.
Security is not addressed, only functionality and minimal input
validation. If you use this code for anything other than satisfying your
curiosity, please keep the following in mind:

- there is no warranty of any kind (you use the code as-is)
- there is no support offered, please do not ask
- there is no guarantee it'll work, although it's not complex so it should
  work
- please do not take credit for code you did not write, especially if you
  are a student


