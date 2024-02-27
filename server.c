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

int server_send(int connfd) {
    char msg[MAX];
    memset(msg, 0, MAX);

    printf(">>> ");

    // copy server message in the buffer
    int n = 0;
    while((msg[n++] = getchar()) != '\n');

    // and set the buffer to the client
    send(connfd, msg, sizeof(msg), 0);

    // if msg contails "exit" then server exit
    if (strncmp("/exit", msg, 4) == 0 || strncmp("/quit", msg, 4) == 0) {
        printf("Bye!\n");
        return 1;
    }

    return 0;
}

int server_receive(int connfd) {
    char buff[MAX];
    memset(buff, 0, MAX);

    // read the message from the client and copy it into buff
    recv(connfd, buff, sizeof(buff), 0);

    // print buffer which contains the clients contents
    printf("Client: %s", buff);

    return 0;
}

int server_idle(int connfd) {
    while(1) {
        server_receive(connfd);
        int serverRet = server_send(connfd);
        if (serverRet) break;
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

    if(server_idle(connfd) != 0) {
        perror("An Error occured!\n");
    }

    close(sockfd);
    close(connfd);
    
    return 0;
}
