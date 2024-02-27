#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PORT 8000
#define MAX 128

int clientIdle(int sockfd) {
    char buff[MAX];
    
    while(1) {
        memset(buff, 0, sizeof(buff));

        // write message into buffer
        printf("Message: ");
        int n = 0;
        while ((buff[n++] = getchar()) != '\n');
        
        // copy message from buffer and send to server
        send(sockfd, buff, sizeof(buff), 0);

        // clear buffer to receive message from server
        memset(buff, 0, sizeof(buff));

        // read message from server into buff
        recv(sockfd, buff, sizeof(buff), 0);
        printf("Server: %s", buff);

        // if msg contains "exit" then client exit
        if ((strncmp(buff, "exit", 4)) == 0) {
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
    serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");

    if (connect(sockfd, (struct sockaddr*)&serverAddr, sizeof(serverAddr)) != 0) {
        perror("Connection with the server FAILED!\n");
    } else {
        printf("Connected to the server [%d]...\n", sockfd);
    }

    if(clientIdle(sockfd) != 0) {
        perror("An Error occured\n");
    }

    close(sockfd);

}
