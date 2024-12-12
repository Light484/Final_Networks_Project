/******************************************************************************
 *
 *      client.c
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
#include <netdb.h>
#include <arpa/inet.h>



typedef struct 
{
        char *serverIP;
        int serverPort;
        struct sockaddr_in serverAddr;

        int clientSD;
        char *fileContents;
        int fileSize;
        int bytesReceived;

        char *serverResponse;

} info;



void getFileContents(info *theInfo, char *fileName);
void setupConnection(info *theInfo);
void writeToProxy(info *theInfo);
void readFromProxy(info *theInfo);



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
        char *fileName = malloc(20);

        // get command line arguments for IP and port
        if (argc == 2) {
                strncpy(theInfo->serverIP, "10.4.2.18", 16);
                theInfo->serverPort = 9099;
                memcpy(fileName, argv[1], strlen(argv[1]) + 1);
        }
        else if (argc == 3) {
                strncpy(theInfo->serverIP, "10.4.2.18", 16);
                theInfo->serverPort = atoi(argv[1]);
                memcpy(fileName, argv[2], strlen(argv[2]) + 1);
        }
        else {
                memcpy(theInfo->serverIP, argv[1], strlen(argv[1]));
                theInfo->serverPort = atoi(argv[2]);
                memcpy(fileName, argv[3], strlen(argv[3]) + 1);
        }

        getFileContents(theInfo, fileName);
        printf("\nFILE CONTENTS: \n\n%s\n\n", theInfo->fileContents);

        setupConnection(theInfo);

        //write the file to the socket
        writeToProxy(theInfo);

        //read any information from the socket
        readFromProxy(theInfo);

        //server response in client
        printf("%s", theInfo->serverResponse);

        //close the socket
        shutdown(theInfo->clientSD, SHUT_RDWR);
        close(theInfo->clientSD);
        free(theInfo->fileContents);
        free(theInfo->serverResponse);
        
        return EXIT_SUCCESS;
}


void setupConnection(info *theInfo)
{
        struct sockaddr_in clientAddress, proxyAddress;
        int clientSD = socket(AF_INET, SOCK_STREAM, 0);
        assert(clientSD != -1);

        //ensure to close the socket and port after termination
        int opt = 1;
        setsockopt(clientSD, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

        //Client setup to bind the socket to port 9099 and any IP address
        //this is the setup specifying the client info
        // memset(&clientAddress, 0, sizeof(struct sockaddr_in));
        // clientAddress.sin_family = AF_INET;
        // clientAddress.sin_addr.s_addr = htonl(INADDR_ANY);
        // clientAddress.sin_port = htons(9095);
        // int returnVal = bind(clientSD, (struct sockaddr *)&clientAddress, 
        //         sizeof(clientAddress));
        // assert(returnVal != -1);

        //Client setup to connect the socket to the proxy and its IP address
        //this is the setup specifying the proxy info
        memset(&proxyAddress, 0, sizeof(struct sockaddr_in));
        proxyAddress.sin_family = AF_INET;
        proxyAddress.sin_addr.s_addr = inet_addr(theInfo->serverIP);
        printf("Server address: %s\n", theInfo->serverIP);
        printf("Server port: %d\n", theInfo->serverPort);
        proxyAddress.sin_port = htons(theInfo->serverPort);
        int returnVal = connect(clientSD, (struct sockaddr *)&proxyAddress, 
                sizeof(proxyAddress));
        assert(returnVal != -1);

        theInfo->clientSD = clientSD;
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



void writeToProxy(info *theInfo)
{
        int returnVal = write(theInfo->clientSD, theInfo->fileContents, 
                theInfo->fileSize + 1);
        assert(returnVal != -1);
}


void readFromProxy(info *theInfo)
{
        char *buffer = malloc(10485760);
        int totalBytesRead = 0;
        int bufferSize = 10485760;
        int returnVal = 1;
        // while (returnVal != 0) {
                returnVal = read(theInfo->clientSD, buffer + totalBytesRead, 
                        bufferSize - totalBytesRead);
                totalBytesRead += returnVal;
                assert(returnVal != -1);
        // }
        buffer[totalBytesRead] = '\0';

        theInfo->serverResponse = buffer;
}