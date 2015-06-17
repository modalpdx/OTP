//
// *****************************************************************************
// 
// Author:    Erik Ratcliffe
// Date:      June 5, 2015
// Project:   Program 4 - OTP
// Filename:  otp_dec.c
// Class:     CS 344 (Spring 2015)
//
//
// Overview:
//    Server/client simulation of the classic "One-time Pad" encryption scheme.
//
//    This file contains code that is specific to the decoding client.
//
// *****************************************************************************
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "otp.h"


#define CLI_TYPE 0   // 1 = encode, 0 = decode


int main(int argc, char **argv)
{
    int    sock;                    // Socket descriptor
    long   inFileSize, keyFileSize; // Input and key file sizes
    int    inFp, keyFp;             // Input and key file descriptors
    int    inChars, keyChars;       // Number of input and key file chars read
    long   actualRecv;              // Total chars from a long string transfer
    int    serverType;              // Type of server (1 = encode, 0 = decode)
    char   *inContent, *keyContent; // Read content of input and key files
    char   buf[MAX_MSG];            // Buffer used to transfer strings/numbers
    struct sockaddr_in myServ;      // Information describing socket
    struct hostent *server;         // Information describing the connected server
    struct stat inFile, keyFile;    // File information for input and key files

    // If we did not get four items on our command line, vital information
    // is missing. Display a usage message and exit.
    //
    if(argc < 4)
    {
        fprintf(stderr, "Usage: %s [input file] [key file] [port]\n", argv[0]);
        exit(1);
    }

    // Get file size info for input and key files. 
    //
    stat(argv[1], &inFile);
    stat(argv[2], &keyFile);
    inFileSize = inFile.st_size;
    keyFileSize = keyFile.st_size;
 
    // Verify that the key file is larger than the input file
    //
    if(keyFileSize < inFileSize)
    {
        fprintf(stderr, "ERROR: key file is too short.\n");
        exit(1);
    }

    // Open the input file
    //
    inFp = open(argv[1], O_RDONLY);
    if(inFp == -1)
    {
        perror("Error opening input file");
        exit(1);
    }

    // Create a properly sized buffer to hold the content, both incoming
    // and outgoing.
    //
    inContent = malloc(sizeof(char) * inFileSize);
   
    // Read in the contents of the input file
    //
    inChars = read(inFp, inContent, inFileSize);
    if(inChars == -1)
    {
        perror("Error reading input file");
        exit(1);
    }

    // Replace the trailing newline in the captured input file content with 
    // a null terminator and close the input file.
    //
    inContent[inFileSize - 1] = '\0';
    close(inFp);

    // Verify the input file (should only contain A-Z and spaces). Exit
    // with an error if this is not the case.
    //
    if(!verifyInput(inContent))
    {
        fprintf(stderr, "ERROR: %s contains invalid characters (only A-Z and spaces allowed)\n", argv[1]);
        exit(1);
    }

    // Open the key file
    //
    keyFp = open(argv[2], O_RDONLY);
    if(keyFp == -1)
    {
        perror("Error opening key file");
        exit(1);
    }

    // Create a properly sized buffer to hold the content 
    //
    keyContent = malloc(sizeof(char) * keyFileSize);

    // Read in the contents of the key file
    //
    keyChars = read(keyFp, keyContent, keyFileSize);
    if(keyChars == -1)
    {
        perror("Error reading key file");
        exit(1);
    }

    // Replace the trailing newline in the captured key file content with 
    // a null terminator and close the key file.
    //
    keyContent[keyFileSize - 1] = '\0';
    close(keyFp);

    // Set up the socket: AF_INET for Internet domain, SOCK_STREAM for
    // TCP, 0 for default Internet protocol. If this was a UDP socket,
    // SOCK_STREAM would be SOCK_DGRAM instead.
    //
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { 
        perror("Socket failed");
        exit(1);
    }

    // We're assuming localhost for all server/client connection with this
    // project. The gethostbyname() function returns a hostent structure
    // with information about the server we are connecting to (i.e.
    // hostname, alias list, address list, etc). 
    //
    if((server = gethostbyname("localhost")) == NULL)
    {
        perror("gethostbyname failed");
        exit(1);
    }

    // Initialize socket structure with zeroes (once handled by bzero()
    // which has been deprecated).
    // 
    memset((char *)&myServ, 0, sizeof(myServ));

    // Use the server hostent information to help populate the sockaddr_in
    // struct that holds server connection data. Set the domain to
    // Internet (AF_INET), convert the port passed on the command line
    // from host to network byte ordering and assign to the sockaddr_in
    // port, and copy in the address of the server.
    //
    myServ.sin_family = AF_INET;
    myServ.sin_port = htons(atoi(argv[3])); // host to network endian conversion
    memcpy(&myServ.sin_addr, server->h_addr_list[0], server->h_length);

    // Connect to the server. Cast the sockaddr_in struct to (sockaddr *)
    // (required).
    //
    if(connect(sock, (struct sockaddr *)&myServ, sizeof(myServ)) == -1)
    {
        perror("connect failed");
        exit(1);
    }
    
    //
    // We're connected. Do something.
    //

    // Start by identifying which server we connected to: 1 = encode, 0 = decode
    //
    // If we are not connected to an appropriate server, exit with an
    // error.
    //
    serverType = recvNum(&sock);
    if(serverType != CLI_TYPE)
    {
        fprintf(stderr, "ERROR: %s cannot find %s_d.\n", argv[0], argv[0]);

        // Close the connection
        //
        close(sock);

        // Free the input file string pointer
        //
        free(inContent);
        inContent = 0;

        // Free the key file string pointer
        //
        free(keyContent);
        keyContent = 0;

        exit(1);
    }

    // Send the input file size. First, adjust the size to accommodate for
    // the removed newline.
    //
    inFileSize -= 1;
    sendNum(&sock, &inFileSize);
    
    // Read the input file acknowledgement.
    //
    memset((char *)&buf, '\0', sizeof(buf));
    recvStr(&sock, buf);

    // Send the key file size. First, adjust the size to accommodate for
    // the removed newline.
    //
    keyFileSize -= 1;
    sendNum(&sock, &keyFileSize);
    
    // Read the key file acknowledgement.
    //
    memset((char *)&buf, '\0', sizeof(buf));
    recvStr(&sock, buf);

    // Send the input file
    //
    sendStr(&sock, inContent);

    // Read the input file acknowledgement.
    //
    memset((char *)&buf, '\0', sizeof(buf));
    recvStr(&sock, buf);

    // Send the key file
    //
    sendStr(&sock, keyContent);

    // Receive the content of the input file back from the server
    //
    actualRecv = recvStream(&sock, inContent, inFileSize);

    // Add a null terminator
    //
    inContent[actualRecv] = '\0';

    // Print the contents of the decoded file to stdout. Add a trailing
    // newline.
    //
    printf("%s\n", inContent);

    // Close the connection
    //
    close(sock);

    // Free the input file string pointer
    //
    free(inContent);
    inContent = 0;

    // Free the key file string pointer
    //
    free(keyContent);
    keyContent = 0;

    return 0;
}
