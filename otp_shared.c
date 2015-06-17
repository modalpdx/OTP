//
// *****************************************************************************
// 
// Author:    Erik Ratcliffe
// Date:      June 5, 2015
// Project:   Program 4 - OTP
// Filename:  otp_shared.c
// Class:     CS 344 (Spring 2015)
//
//
// Overview:
//    Server/client simulation of the classic "One-time Pad" encryption scheme.
//
//    This file contains functions that are shared between the various
//    client and server programs.
//
// *****************************************************************************
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include "otp.h"


// *****************************************************************************
// 
// int verifyInput(char *str)
//
// Purpose: Verifies that the input contains no invalid character.
//
// *****************************************************************************
//
int verifyInput(char *str)
{
    long strLen = (long)strlen(str); // Length of input string
    long idx;                        // Loop index

    for(idx = 0; idx < strLen; idx++)
    {
        // Use ASCII codes to check if the characters in the input string
        // are valid. A-Z is ASCII 65-90. We allow spaces as well, which
        // is ASCII 32.
        //
        if((str[idx] > 90) || ((str[idx] < 65) && (str[idx] != 32)))
        {
            return 0; // Not validated!
        }
    }

    return 1; // Validated!
}


// *****************************************************************************
// 
// void sendNum(int *sock, long *num)
//
// Purpose: Translates a number from host to network byte order and sends
// it across a network connection.
//
// *****************************************************************************
//
void sendNum(int *sock, long *num)
{
    long numToSend;  // Number to send
    int  numSent;    // Characters transferred

    // First, convert the number we're sending from host to network byte
    // order.
    //
    numToSend = htonl(*num);

    // Send the number, report the number of characters transferred. If
    // -1, exit with an error.
    //
    if((numSent = send(*sock, &numToSend, sizeof(numToSend), 0)) == -1)
    {
        perror("client send failed");
        exit(1);
    }
}


// *****************************************************************************
// 
// void sendStr(int *sock, char *str)
//
// Purpose: Send a string across a network connection.
//
// *****************************************************************************
//
void sendStr(int *sock, char *str)
{
    long len;     // Holds the length of the input string
    int  numSent; // Characters transferred

    // Get the input string length.
    //
    len = strlen(str);

    // Send the string, report the number of characters transferred. If
    // -1, exit with an error.
    //
    if((numSent = send(*sock, str, len, 0)) == -1)
    {
        perror("client send failed");
        exit(1);
    }
}


// *****************************************************************************
// 
// int recvNum(int *sock)
//
// Purpose: Receive a number from across a network connection.
//
// *****************************************************************************
//
int recvNum(int *sock)
{
    long outNum;  // Number to return
    long inNum;   // Number received 
    int  numRecv; // Characters transferred

    // Read the number, report the number of characters transferred. If
    // -1, exit with an error.
    //
    if((numRecv = recv(*sock, &inNum, sizeof(inNum), 0)) == -1)
    {
        perror("server recv failed (client type)");
        exit(1);
    }
    else if(numRecv == 0)
    {
        perror("socket closed during recv");
        exit(1);
    }

    outNum = ntohl(inNum); // Convert the number from network to host byte order

    return outNum;         // Return the received, converted number
}


// *****************************************************************************
// 
// void recvStr(int *sock, char *str)
//
// Purpose: Receive a short string from across a network connection.
//
// *****************************************************************************
//
void recvStr(int *sock, char *str)
{
    long len;      // Holds the length of the input string
    int  numRecv;  // Characters transferred

    len = MAX_MSG; // Set string length to our MAX_MSG

    // Read the string, report the number of characters transferred. If
    // -1, exit with an error.
    //
    if((numRecv = recv(*sock, str, len, 0)) == -1)
    {
        perror("client recv failed");
        exit(1);
    }
}


// *****************************************************************************
// 
// int recvStream(int *sock, char *str, long maxChars)
//
// Purpose: Receive a long string from across a network connection.
//
// *****************************************************************************
//
int recvStream(int *sock, char *str, long maxChars)
{
    int  numRecv    = 0;  // Characters transferred per recv() call
    long actualRecv = 0;  // Actual accumulated characters transferred
    char buf[MAX_MSG];    // Temp. buffer used by recv()

    // Start at the beginning of outStr
    //
    str[0] = '\0';

    // Keep looping until the entire input file is received
    //
    while(actualRecv < maxChars)
    {
       // Read the string, report the number of characters transferred. If
       // -1, exit with an error. If 0, we're done. Otherwise, concatenate
       // the contents of buf to the final string and add the number of
       // characters transferred to the running count (actualRecv).
       //
       if((numRecv = recv(*sock, buf, sizeof(buf), 0)) == -1)
       {
           perror("server recv failed (message)");
           exit(1);
       }
       else if(numRecv == 0)
       {
           perror("socket closed during recv (cli)");
           exit(1);
       }
       else 
       {
           // At one point, I was using strcat(). After multiple overruns,
           // I realized that strncat() was the way to go. That had me
           // stumped for about half a day.
           //
           strncat(str, buf, numRecv);
           actualRecv += numRecv;
       }
    }

    return actualRecv; // Return the actual number of characters received
}


// *****************************************************************************
// 
// int strIdx(char *inString, char ch)
//
// Purpose: Search a string for a character, return the character's index
// in the string.
//
// *****************************************************************************
//
int strIdx(char *inString, char ch)
{
    char *baseAddr;  // base address of the string
    char *chAddr;    // address of the character in the string

    // strchr() returns a pointer to the position of a character in a
    // string. To get the numeric index of the character, subtract the
    // base pointer address of the string from the address value returned
    // by strchr(). Not too tricky.
    //
    // So, start by grabbing the base address of the string.
    //
    baseAddr = &inString[0];

    // ...then, get the address of the character.
    //
    chAddr = strchr(inString, ch);

    // ...finally, subtract the character address from the base address
    // and return the final result, which will be the index of the
    // character in the string.
    //
    return chAddr - baseAddr;
}


// *****************************************************************************
// 
// void encodeChars(char *inputChars, char *keyChars)
//
// Purpose: Encode an input string using a simulation of the classic
// "One-time Pad" encryption scheme.
//
// *****************************************************************************
//
void encodeChars(char *inputChars, char *keyChars)
{
    char allowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Valid input characters
    int  inputCh;           // Character to encode
    int  keyCh;             // Associated character from the key
    int  sum;               // Sum of inputCh and keyCh (1st pt of OTP)
    int  mod;               // Modulus of sum and # of allowed chars (2nd pt of OTP)
    long inputCharsLen;     // Length of input character string
    long idx = 0;           // Loop index

    inputCharsLen = strlen(inputChars); // Grab the length of the input string

    while(idx < inputCharsLen)
    {
        inputCh = strIdx(allowedChars, inputChars[idx]); // Char from input string
        keyCh = strIdx(allowedChars, keyChars[idx]);

        // Add the numeric value of the two characters together...
        //
        sum = inputCh + keyCh;  

        // ...then mod with the total number of allowed characters.
        //
        mod = sum % strlen(allowedChars); 

        inputChars[idx] = allowedChars[mod]; // Replace input char with encoded char
        idx++;
    }
}


// *****************************************************************************
// 
// void decodeChars(char *inputChars, char *keyChars)
//
// Purpose: Decode an input string using a simulation of the classic
// "One-time Pad" encryption scheme.
//
// *****************************************************************************
//
void decodeChars(char *inputChars, char *keyChars)
{
    char allowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // Valid input characters
    int  inputCh;           // Character to encode
    int  keyCh;             // Associated character from the key
    int  diff;              // Diff of inputCh and keyCh (1st pt of OTP)
    int  mod;               // Modulus of sum and # of allowed chars (2nd pt of OTP)
    long inputCharsLen;     // Length of input character string
    long idx = 0;           // Loop index

    inputCharsLen = strlen(inputChars); // Grab the length of the input string

    while(idx < inputCharsLen)
    {
        inputCh = strIdx(allowedChars, inputChars[idx]); // Char from input string
        keyCh = strIdx(allowedChars, keyChars[idx]);     // Matching char from key

        // Subtract the numeric value of the key char from the input char.
        //
        diff = inputCh - keyCh;

        // If the result of the subtraction is a negative number, add the
        // total number of allowed characters to bring it back into
        // positive territory.
        //
        if(diff < 0)
        {
            diff += strlen(allowedChars);
        }

        // Mod with the total number of allowed characters.
        //
        mod = diff % strlen(allowedChars);

        inputChars[idx] = allowedChars[mod]; // Replace input char with encoded char
        idx++;
    }
}




