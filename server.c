#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/ip.h>
#include <pthread.h>

#define PORT 8000
#define MAX_PENDING 5
#define MAX 128

int server_send(long connfd) {
    char msg[MAX];
    memset(msg, 0, MAX);

    while(fgets(msg, MAX, stdin) != NULL) {
        // if msg contails "exit" then server exit
        if (strncmp("/exit", msg, 5) == 0 || strncmp("/quit", msg, 5) == 0) {
            printf("Bye!\n");
            return 1;
        }

        // and set the buffer to the client
        send(connfd, msg, sizeof(msg), 0);
    }

    return 0;
}

void * server_receive(void * connfd) {
    char buff[MAX];
    long socketfd = (long) connfd;
   
    while(1) {
        memset(buff, 0, MAX);

        // read the message from the client and copy it into buff
        int response = recv(socketfd, buff, sizeof(buff), 0);
        if (response) {
            // print buffer which contains the clients contents
            printf("client: %s", buff);
            fflush(stdout);
        }
    }

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
    long connfd;
    connfd = accept(sockfd, (struct sockaddr*)&clientAddr, &clientLen);
    if (connfd == -1) {
        perror("Server accept FAILED!");
    } else {
        printf("Server [%d] accepted the client [%ld]\n", sockfd, connfd);
    }

    pthread_t thread;
    pthread_create(&thread, NULL, server_receive, (void *)connfd);

    server_send(connfd);

    pthread_cancel(thread);
    close(sockfd);
    close(connfd);
    
    return 0;
}
