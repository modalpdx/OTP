//
// *****************************************************************************
// 
// Author:    Erik Ratcliffe
// Date:      June 5, 2015
// Project:   Program 4 - OTP
// Filename:  keygen.c
// Class:     CS 344 (Spring 2015)
//
//
// Overview:
//    Server/client simulation of the classic "One-time Pad" encryption scheme.
//
//    This file contains code that is specific to the key generation utility.
//
// *****************************************************************************
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "otp.h"

int main(int argc, char **argv)
{
    // If we did not get a length for the key, display usage and exit.
    //
    if(argc == 1)
    {
        printf("Usage: %s length_of_key\n", argv[0]);
        exit(1);
    }

    // Allowed characters include capital A-Z and space.
    //
    char allowedChars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    int keySize = 0;  // Holds the user-specified length of the key
    int seed;         // Seeds the randomizer
    int idx;          // Loop index

    keySize = atoi(argv[1]);  // Grab the key length (convert to an int)

    seed = time(NULL);        // Seed the randomizer
    srand(seed);              // Ditto

    for(idx = 0; idx < keySize; idx++)
    {
        // Grab a random index from allowedChars and print to stdout. Do this
        // until we have reached the desired key length.
        //
        printf("%c", allowedChars[rand() % (int)strlen(allowedChars)]);
    }

    // Add a trailing newline.
    //
    printf("\n");

    return 0;
}
