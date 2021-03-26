#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/timeb.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include "intbuf.h"

int sockfd, send_cnt;
char buf[BUFSIZ];
char bufr[BUFSIZ];
struct sockaddr_in their_addr;
pthread_t thread;
unsigned long long start_time;

long long getSysTime() {
    struct timeb t;
    ftime(&t);
    return 1000 * t.time + t.millitm;
}

void sendNumber() {
    INTBUF num;
    num.number = random1_500();
    send(sockfd, num.data, INTBUFSZ, 0);
    printf("Send: %d\n", num.number);
    send_cnt++;
}

void sendTerminate() {
    INTBUF num;
    num.number = -1;
    send(sockfd, num.data, INTBUFSZ, 0);
    printf("Send: %d\n", num.number);
}

void getMessage(void *p) {
    int c = 0;
    while((c = recv(sockfd, bufr, BUFSIZ, 0)) > 0 && send_cnt < 20) {
        printf("Recv %d bytes of ", c);
        INTBUF num;
        memcpy(num.data, bufr, INTBUFSZ);
        printf("number: %d ", num.number);
        printf("Time now: %llu\n", getSysTime() - start_time);
        if(num.number > 100) sendNumber();
    }
}

int main(int argc,char *argv[]) {   //usage: ./client host port
    ssize_t numbytes;
    start_time = getSysTime();
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
        while(c-- && send_cnt < 20) {
            sleep(1);
            if(send_cnt < 20) sendNumber();
        }
        sendTerminate();
    } else perror("Create msg thread failed");
    close(sockfd);
    return 0;
}
