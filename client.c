#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8000
#define MAX 128

int client_send(long sockfd) {
    char msg[MAX];
    memset(msg, 0, MAX);

    while(fgets(msg, MAX, stdin) != NULL) {
        if (strncmp(msg, "/exit", 5) == 0 || strncmp(msg, "/quit", 5) == 0) {
            printf("Bye!\n");
            return 1;
        }
        
        // copy msg from buffer and send to server
        send(sockfd, msg, sizeof(msg), 0);
    }

    return 0;
}

void * client_receive(void *sockfd) {
    long socketfd = (long) sockfd;
    char buff[MAX];

    while(1) {
        memset(buff, 0, sizeof(buff));

        // read message from server into buff
        int response = recv(socketfd, buff, sizeof(buff), 0);
        if (response == 0) {
            printf("Peer [%ld] disconnected...", socketfd);
            break;
        }
        
        printf("server: %s", buff);
        fflush(stdout);

        // if msg contains "exit" then client exit
        if ((strncmp(buff, "/exit", 4)) == 0 || (strncmp(buff, "/quit", 4)) == 0) {
            printf("Bye!\n");
            pthread_exit(NULL);
        }
    }
}

int main() {
    long sockfd;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == -1) {
        perror("Socket creation FAILED!");
    } else {
        printf("Socket [%ld] succesfully created!\n", sockfd);
    }

    struct sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        perror("Connection with the server FAILED!\n");
    } else {
        printf("Connected to the server [%ld]...\n", sockfd);
    }

    pthread_t thread;
    pthread_create(&thread, NULL, client_receive, (void *)sockfd);

    client_send(sockfd);
    
    pthread_cancel(thread);
    close(sockfd);

    return 0;
}
