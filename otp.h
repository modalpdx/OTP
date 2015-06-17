//
// *****************************************************************************
// 
// Author:    Erik Ratcliffe
// Date:      June 5, 2015
// Project:   Program 4 - OTP
// Filename:  otp.h
// Class:     CS 344 (Spring 2015)
//
//
// Overview:
//    Server/client simulation of the classic "One-time Pad" encryption scheme.
//
//    This file contains variable definitions and function prototypes.
//
// *****************************************************************************
//

#ifndef OTP_H
#define OTP_H


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>


#define MAX_MSG 4196 // power of 2, speeds things up a smidge


// *****************************************************************************
// 
// int verifyInput(char *str)
//
//    Entry:   char *str
//                String containing input file.
//
//    Exit:    Returns status of verification: 1 = yes, 0 = no
//
//    Purpose: Verifies that the input contains no invalid character.
//
// *****************************************************************************
//
int verifyInput(char *str);


// *****************************************************************************
// 
// void sendNum(int *sock, long *num)
//
//    Entry:   int *sock
//                Socket for the current network connection
//             long *num
//                Number to send across the connection
//
//    Exit:    None.
//
//    Purpose: Translates a number from host to network byte order and sends
//    it across a network connection.
//
// *****************************************************************************
//
void sendNum(int *sock, long *num);


// *****************************************************************************
// 
// void sendStr(int *sock, char *str)
//
//    Entry:   int *sock
//                Socket for the current network connection
//             char *str
//                String to send across the connection
//
//    Exit:    None.
//
//    Purpose: Send a string across a network connection.
//
// *****************************************************************************
//
void sendStr(int *sock, char *str);


// *****************************************************************************
// 
// int recvNum(int *sock)
//
//    Entry:   int *sock
//                Socket for the current network connection
//
//    Exit:    Integer translated from network to host byte order.
//
//    Purpose: Receive a number from across a network connection.
//
// *****************************************************************************
//
int recvNum(int *sock);


// *****************************************************************************
// 
// void recvStr(int *sock, char *str)
//
//    Entry:   int *sock
//                Socket for the current network connection
//             char *str
//                Short string to receive from across the connection (see
//                recvStream() for receiving long strings).
//
//    Exit:    None.
//
//    Purpose: Receive a short string from across a network connection.
//
// *****************************************************************************
//
void recvStr(int *sock, char *str);


// *****************************************************************************
// 
// int recvStream(int *sock, char *str, long maxChars)
//
//    Entry:   int *sock
//                Socket for the current network connection
//             char *str
//                Long string to receive from across the connection (see
//                recvStr() for receiving short strings).
//             long maxChars
//                Long integer containing the  number of characters expected
//                to arrive in the string.
//
//    Exit:    An integer containing the actual number of characters received.
//
//    Purpose: Receive a long string from across a network connection.
//
// *****************************************************************************
//
int recvStream(int *sock, char *str, long maxChars);


// *****************************************************************************
// 
// int strIdx(char *inString, char ch)
//
//    Entry:   char *inString
//                String to be searched (haystack).
//             char ch
//                Character to find (needle).
//
//    Exit:    An integer containing the index of the character found in the
//             string (indexed to 0)
//
//    Purpose: Search a string for a character, return the character's index
//    in the string.
//
// *****************************************************************************
//
int strIdx(char *inString, char ch);


// *****************************************************************************
// 
// void encodeChars(char *inputChars, char *keyChars)
//
//    Entry:   char *inputChars
//                String to be encoded.
//             char *keyChars
//                Randomized "key" file to use for the encoding.
//
//    Exit:    The inputChars string is updated in-place to include the
//             encoded characters.
//
//    Purpose: Encode an input string using a simulation of the classic
//    "One-time Pad" encryption scheme.
//
// *****************************************************************************
//
void encodeChars(char *inputChars, char *keyChars);


// *****************************************************************************
// 
// void decodeChars(char *inputChars, char *keyChars)
//
//    Entry:   char *inputChars
//                String to be decoded.
//             char *keyChars
//                Randomized "key" file to use for the decoding.
//
//    Exit:    The inputChars string is updated in-place to include the
//             decoded characters.
//
//    Purpose: Decode an input string using a simulation of the classic
//    "One-time Pad" encryption scheme.
//
// *****************************************************************************
//
void decodeChars(char *inputChars, char *keyChars);


#endif
