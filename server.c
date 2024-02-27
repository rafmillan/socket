#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>

#define PORT 8000
#define MAX_PENDING 5
#define MAX 128

int idle(int connfd) {
    char buff[MAX];

    while(1) {
        memset(buff, 0, MAX);

        // read the message from the client and copy it into buff
        recv(connfd, buff, sizeof(buff), 0);

        // print buffer which contains the clients contents
        printf("Client: %s", buff);
        printf("Message: ");

        memset(buff, 0, MAX);
        int n = 0;

        // copy server message in the buffer
        while((buff[n++] = getchar()) != '\n');

        // and set the buffer to the client
        send(connfd, buff, sizeof(buff), 0);

        // if msg contails "exit" then server exit
        if (strncmp("exit", buff, 4) == 0) {
            printf("Bye!\n");
            break;
        }
    }

    return 0;
}

int main() {
    int sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation FAILED!");
    } else {
        printf("Socket [%d] succesfully created!\n", sockfd);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (bind(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        perror("Socket bind FAILED!\n");
    } else {
        printf("Socket [%d] succesfully binded!\n", sockfd);
    }
    
    if (listen(sockfd, MAX_PENDING) != 0) {
        perror("Socket listening FAILED!");
    } else {
        printf("Server [%d] listening...\n", sockfd);
    }

    struct sockaddr_in clientAddr;
    int clientLen = sizeof(clientAddr);
    int connfd;
    connfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
    if (connfd == -1) {
        perror("Server accept FAILED!");
    } else {
        printf("Server [%d] accepted the client [%d]\n", sockfd, connfd);
    }

    if(idle(connfd) != 0) {
        perror("An Error occured!\n");
    }

    close(sockfd);
    
    return 0;
}
