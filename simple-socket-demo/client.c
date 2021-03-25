#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "intbuf.h"

int sockfd;
char buf[BUFSIZ];
char bufr[BUFSIZ];
struct sockaddr_in their_addr;
pthread_t thread;

void sendNumber() {
    INTBUF num;
    num.number = random1_500();
    send(sockfd, num.data, INTBUFSZ, 0);
    printf("Send: %d\n", num.number);
}

void sendTerminate() {
    INTBUF num;
    num.number = -1;
    send(sockfd, num.data, INTBUFSZ, 0);
    printf("Send: %d\n", num.number);
}

void getMessage(void *p) {
    int c = 0;
    while((c = recv(sockfd, bufr, BUFSIZ, 0)) > 0) {
        printf("Recv %d bytes of ", c);
        INTBUF num;
        memcpy(num.data, bufr, INTBUFSZ);
        printf("number: %d\n", num.number);
        if(num.number > 100) sendNumber();
    }
}

int main(int argc,char *argv[]) {   //usage: ./client host port
    ssize_t numbytes;
    puts("break!");
    srand(time(NULL));
    while((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1);
    puts("Get sockfd");
    their_addr.sin_family = AF_INET;
    their_addr.sin_port = htons(atoi(argv[2]));
    their_addr.sin_addr.s_addr=inet_addr(argv[1]);
    bzero(&(their_addr.sin_zero), 8);
    
    while(connect(sockfd,(struct sockaddr*)&their_addr,sizeof(struct sockaddr)) == -1);
    puts("Connected to server");
    numbytes = recv(sockfd, buf, BUFSIZ,0);
    buf[numbytes]='\0';  
    puts(buf);
    if(!pthread_create(&thread, NULL, (void*)&getMessage, NULL)) {
        puts("Thread create succeeded");
        int c = 20;
        while(c--) {
            sendNumber();
            sleep(1);
        }
        sendTerminate();
    } else perror("Create msg thread failed");
    close(sockfd);
    return 0;
}
