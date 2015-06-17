//
// *****************************************************************************
// 
// Author:    Erik Ratcliffe
// Date:      June 5, 2015
// Project:   Program 4 - OTP
// Filename:  otp_enc_d.c
// Class:     CS 344 (Spring 2015)
//
//
// Overview:
//    Server/client simulation of the classic "One-time Pad" encryption scheme.
//
//    This file contains code that is specific to the encoding server.
//
// *****************************************************************************
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <wait.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "otp.h"


#define SVR_TYPE 1   // 1 = encryption, 0 = decryption


int main(int argc, char **argv)
{
    int   sock, cli;               // Socket descriptors for parent and child
    int   optval;                  // Holds option values from setsockopt()
    long  serverType;              // Type of server (see defined SVR_TYPE above)
    long  actualRecv;              // Total chars from a long string transfer
    long  inFileSize, keyFileSize; // Input and key file sizes
    char  *inContent, *keyContent; // Read content of input and key files
    pid_t pid;                     // Process ID
    socklen_t myCliLen;            // Holds size of client socket info
    struct sockaddr_in myServ, myCli; // Info describing client and server sockets

    // If we did not get two items on our command line, vital information
    // is missing. Display a usage message and exit.
    //
    if(argc < 2)
    {
        fprintf(stderr, "Usage: %s [port]\n", argv[0]);
        exit(1);
    }

    // Set up the socket: AF_INET for Internet domain, SOCK_STREAM for
    // TCP, 0 for default Internet protocol. If this was a UDP socket,
    // SOCK_STREAM would be SOCK_DGRAM instead.
    //
    if((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    { 
        perror("Socket failed");
        exit(1);
    }

    // This is rumored to help with the "address in use" errors that
    // sometimes pop up when trying to start a server on a busy socket. I
    // have found it works sometimes but not always. It is possible my
    // syntax is incorrect but based on everything I have read it is not.
    // I am leaving it here for future experimentation.
    //
    if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1)
    {
        perror("Setsockopt failed");
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
    // port, and indicate that connections from any address on our network
    // are okay (INADDR_ANY).
    //
    myServ.sin_family = AF_INET;
    myServ.sin_port = htons(atoi(argv[1])); // host to network endian conversion
    myServ.sin_addr.s_addr = htonl(INADDR_ANY);

    // Associate the address assocated with myServ with the socket for
    // this server.
    //
    if(bind(sock, (struct sockaddr *)&myServ, sizeof(myServ)) == -1)
    {
        perror("Bind failed");
        exit(1);
    }

    // Listen for connections. Allow up to 5 connections.
    //
    if(listen(sock, 5) == -1)
    {
        perror("Listen failed");
        exit(1);
    }

    while(1)
    {
       myCliLen = sizeof(myCli); // Grab the size of the myCli sockaddr_in struct

       // Wait for a client connection. If one is requested, accept it and 
       // return a socket descriptor for the new connection. If it fails,
       // exit with an error. 
       //
       // If more client connections are requested, the while loop should
       // catch them with subseqent calls to accept() (up to 5)..
       //
       cli = accept(sock, (struct sockaddr *)&myCli, &myCliLen);
       if(cli == -1)
       {
           perror("Accept failed");
           exit(1);
       }

       // Fork the server.
       //
       pid = fork();

       if(pid < 0)      // Error
       {
           close(cli);
           cli = -1;

           perror("Fork failed");
       }
       
       if(pid == 0)    // Child process
       {
           // Close the server socket connection. We don't need it.
           //
           close(sock);
           sock = -1;

           // Send the server type to the client. If the server and client
           // are not matched (encoding server -> encoding client, for
           // example), the client will close the connection.
           //
           serverType = SVR_TYPE;
           sendNum(&cli, &serverType);

           // Get the input file size from the client.
           //
           inFileSize = recvNum(&cli);

           // Send acknowledgement of receiving input file size
           //
           sendStr(&cli, "I got your input file size");
           
           // Get the key file size from the client.
           //
           keyFileSize = recvNum(&cli);
 
           // Send acknowledgement of receiving key file size
           //
           sendStr(&cli, "I got your key file size");

           // Create a properly sized buffer to hold the input file
           // content
           //
           inContent = malloc(sizeof(char) * (inFileSize + 1));

           // Set the first character to a null terminator, just to be
           // sure we are starting at the beginning.
           //
           inContent[0] = '\0';
      
           // Get the input file content.
           //
           actualRecv = recvStream(&cli, inContent, inFileSize);

           // Add a null terminator
           //
           inContent[actualRecv] = '\0';

           // Send acknowledgement of receiving input file
           //
           sendStr(&cli, "I got your input file");

           // Create a properly sized buffer to hold the key file
           // content
           //
           keyContent = malloc(sizeof(char) * (keyFileSize + 1));

           // Set the first character to a null terminator just to be sure
           // we're starting at the beginning.
           //
           keyContent[0] = '\0';

           // Get the key file content.
           //
           actualRecv = recvStream(&cli, keyContent, keyFileSize);

           // Add a null terminator
           //
           keyContent[actualRecv] = '\0';

           // Encode the characters from the input file using the content
           // from the key file. The input file content is updated
           // in-place (so inContent contains the input file going in and
           // the encoded file coming out).
           // 
           encodeChars(inContent, keyContent);

           // Send encoded input file back to the client
           //
           sendStr(&cli, inContent);

           // Free the inContent buffer
           //
           free(inContent);
           inContent = 0;
 
           // Free the keyContent buffer
           //
           free(keyContent);
           keyContent = 0;
 
           // Close the client 
           //
           close(cli);
           cli = -1;

           exit(0);
       }
       else             // Parent
       {
           // Close the client socket
           //
           close(cli);
           cli = -1;

           // Reap the closed server zombie
           //
           wait(NULL);
       }
    }

    // Close the server socket
    //
    close(sock);
    sock = -1;

    return 0;
}
