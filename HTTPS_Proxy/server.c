/******************************************************************************
 *
 *      server.c
 *
 *      Isabel Muste (imuste01)
 *      11/12/2024
 *      
 *      CS 112 Final Project
 * 
 *      ...
 *      
 *
 *****************************************************************************/

#include <math.h> 
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <limits.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>



typedef struct 
{
        char *serverIP;
        int serverPort;
        struct sockaddr_in serverAddr;

        int serverSD;
        char *fileContents;
        int fileSize;

        int clientSD;
        char *clientRequest;
        int bytesReceived;

} info;


void getFileContents(info *theInfo, char *fileName);
void setupConnection(info *theInfo);
void readFromProxy(info *theInfo);
void writeToProxy(info *theInfo);


/*
 * name:      main
 * purpose:   opens the commands file and initializes a new cache instance
 * arguments: argc, argv
 * returns:   exit success
 * effects:   none
 */
int main(int argc, char *argv[])
{       
        info *theInfo = malloc(sizeof(info));
        theInfo->serverIP = malloc(20);

        getFileContents(theInfo, argv[1]);
        printf("\nFILE CONTENTS: \n\n%s\n\n", theInfo->fileContents);

        // get command line arguments for IP and port
        if (argc == 2) {
                strncpy(theInfo->serverIP, "10.4.2.18", 16);
                theInfo->serverPort = 9099;
        }
        else if (argc == 3) {
                strncpy(theInfo->serverIP, "10.4.2.18", 16);
                theInfo->serverPort = atoi(argv[1]);
        }
        else {
                memcpy(theInfo->serverIP, argv[1], strlen(argv[2]));
                theInfo->serverPort = atoi(argv[2]);
        }

        setupConnection(theInfo);
        readFromProxy(theInfo);
        writeToProxy(theInfo);


        //close necessary connections
        shutdown(theInfo->clientSD, SHUT_RDWR);
        close(theInfo->clientSD);
        free(theInfo->clientRequest);
        close(theInfo->serverSD);

        return EXIT_SUCCESS;
}


void setupConnection(info *theInfo)
{
        struct sockaddr_in serverAddress;
        socklen_t serverAddressLength;

        int ogSD = socket(AF_INET, SOCK_STREAM, 0);
        assert(ogSD != -1);

        //ensure to close the socket and port after termination
        int opt = 1;
        setsockopt(ogSD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        //setup to bind the socket to port 9099 and any IP address
        //this is the setup specifying the server info
        memset(&serverAddress, 0, sizeof(struct sockaddr_in));
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        serverAddress.sin_port = htons(9098);
        int returnVal = bind(ogSD, (struct sockaddr *)&serverAddress, 
                sizeof(struct sockaddr_in));
        assert(returnVal != -1);

        theInfo->serverSD = ogSD;

        //listen for incoming connections
        returnVal = listen(theInfo->serverSD, 500);
        assert(returnVal != -1);

        //block until a connection request is seen
        serverAddressLength = sizeof(serverAddress);
        
        int newSD = accept(ogSD, (struct sockaddr *)&serverAddress, 
                &serverAddressLength);
        assert(newSD != -1);

        theInfo->clientSD = newSD;
}


void readFromProxy(info *theInfo)
{
        //read any information from the socket
        char *buffer = malloc(10485760);
        int totalBytesRead = 0;
        int bufferSize = 10485760;
        int returnVal = 1;
        while (returnVal != 0) {
                returnVal = read(theInfo->clientSD, buffer + totalBytesRead, 
                        bufferSize - totalBytesRead);
                totalBytesRead += returnVal;
                assert(returnVal != -1);
        }
        buffer[totalBytesRead] = '\0';

        theInfo->clientRequest = buffer;
}


void writeToProxy(info *theInfo)
{
        //write into the socket
        int returnVal = write(theInfo->clientSD, theInfo->fileContents, 
                theInfo->fileSize + 1);
        assert(returnVal != -1);
        shutdown(theInfo->clientSD, SHUT_WR);

}


void getFileContents(info *theInfo, char *fileName)
{
        char *file = fileName;
        int fd = open(file, O_RDONLY);
        assert(fd != -1);

        struct stat st;
        stat(file, &st);
        size_t fileSize = st.st_size;

        char *fileContents = malloc(fileSize + 1);
        assert(fileContents != NULL);

        int bytesRead = read(fd, fileContents, fileSize);
        assert(bytesRead == fileSize);

        fileContents[fileSize] = '\0';

        theInfo->fileContents = fileContents;
        theInfo->fileSize = fileSize;

        close(fd);
}