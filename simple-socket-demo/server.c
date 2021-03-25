#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/signal.h>
#include <sys/file.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>
#include "intbuf.h"

int fd;
socklen_t struct_len = sizeof(struct sockaddr_in);
struct sockaddr_in server_addr;
pthread_t accept_threads[8];

#define MAXWAITSEC 10
struct THREADTIMER {
    pthread_t *thread;
    time_t touch;
    int accept_fd;
    ssize_t numbytes;
    char *data;
};
typedef struct THREADTIMER THREADTIMER;

#define DATASIZE 64

#define showUsage(program) printf("Usage: %s [-d] listen_port try_times\n\t-d: As daemon\n", program)

void acceptClient();
void acceptTimer(void *p);
int bindServer(uint16_t port, u_int try_times);
int checkBuffer(THREADTIMER *timer);
void handleAccept(void *accept_fd_p);
void handle_pipe(int signo);
void handle_quit(int signo);
int listenSocket(u_int try_times);
int sendData(int accept_fd, void *data, size_t length);

int bindServer(uint16_t port, u_int try_times) {
    int fail_count = 0;
    int result = -1;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    bzero(&(server_addr.sin_zero), 8);

    fd = socket(AF_INET, SOCK_STREAM, 0);
    while(!~(result = bind(fd, (struct sockaddr *)&server_addr, struct_len)) && fail_count++ < try_times) sleep(1);
    if(!~result && fail_count >= try_times) {
        puts("Bind server failure!");
        return 0;
    } else{
        puts("Bind server success!");
        return 1;
    }
}

int listenSocket(u_int try_times) {
    int fail_count = 0;
    int result = -1;
    while(!~(result = listen(fd, 10)) && fail_count++ < try_times) sleep(1);
    if(!~result && fail_count >= try_times) {
        puts("Listen failed!");
        return 0;
    } else{
        puts("Listening....");
        return 1;
    }
}

int sendData(int accept_fd, void *data, size_t length) {
    if(!~send(accept_fd, data, length, 0)) {
        puts("Send data error");
        return 0;
    } else {
        //puts("Send data succeeded");
        return 1;
    }
}

int checkBuffer(THREADTIMER *timer) {
    INTBUF num;
    memcpy(num.data, timer->data, INTBUFSZ);
    printf("Get number: %d\n", num.number);
    if(num.number > 0) {
        num.number += random1_500();
        if(num.number > 100) {
            sendData(timer->accept_fd, num.data, INTBUFSZ);
            printf("Send number: %d\n", num.number);
        }
    }
    return ~(num.number);
}

void handle_quit(int signo) {
    printf("Handle sig %d\n", signo);
    pthread_exit(NULL);
}

#define timerPointerOf(x) ((THREADTIMER*)(x))
#define touchTimer(x) timerPointerOf(x)->touch = time(NULL)

void acceptTimer(void *p) {
    THREADTIMER *timer = timerPointerOf(p);
    signal(SIGQUIT, handle_pipe);
    signal(SIGPIPE, handle_pipe);
    while(*timer->thread && !pthread_kill(*timer->thread, 0)) {
        sleep(MAXWAITSEC);
        puts("Check accept status");
        if(time(NULL) - timer->touch > MAXWAITSEC) {
            pthread_kill(*timer->thread, SIGQUIT);
            close(timer->accept_fd);
            if(timer->data) free(timer->data);
            *timer->thread = 0;
        }
    }
    free(p);
}

void handle_pipe(int signo) {
    puts("Pipe error");
}

void handleAccept(void *p) {
    int accept_fd = timerPointerOf(p)->accept_fd;
    if(accept_fd > 0) {
        puts("Connected to the client.");
        signal(SIGQUIT, handle_quit);
        signal(SIGPIPE, handle_pipe);
        pthread_t thread;
        if (pthread_create(&thread, NULL, (void *)&acceptTimer, p)) puts("Error creating timer thread");
        else puts("Creating timer thread succeeded");
        sendData(accept_fd, "Welcome to simple demo server.", 31);
    
        char *buff = calloc(BUFSIZ, sizeof(char));
        if(buff) {
            timerPointerOf(p)->data = buff;
            while(*timerPointerOf(p)->thread && (timerPointerOf(p)->numbytes = recv(accept_fd, buff, BUFSIZ, 0)) > 0) {
                touchTimer(p);
                buff[timerPointerOf(p)->numbytes] = 0;
                printf("Get %zd bytes.\n", timerPointerOf(p)->numbytes);
                puts("Check buffer");
                if(!checkBuffer(timerPointerOf(p))) break;
            }
            printf("Recv %zd bytes\n", timerPointerOf(p)->numbytes);
        } else puts("Error allocating buffer");
        close(accept_fd);
    } else puts("Error accepting client");
}

void acceptClient() {
    srand(time(NULL));
    while(1) {
        puts("Ready for accept, waitting...");
        int p = 0;
        while(p < 8 && accept_threads[p] && !pthread_kill(accept_threads[p], 0)) p++;
        if(p < 8) {
            printf("Run on thread No.%d\n", p);
            THREADTIMER *timer = malloc(sizeof(THREADTIMER));
            if(timer) {
                struct sockaddr_in client_addr;
                timer->accept_fd = accept(fd, (struct sockaddr *)&client_addr, &struct_len);
                timer->thread = &accept_threads[p];
                timer->touch = time(NULL);
                timer->data = NULL;
                if (pthread_create(timer->thread, NULL, (void *)&handleAccept, timer)) puts("Error creating thread");
                else puts("Creating thread succeeded");
            } else puts("Allocate timer error");
        } else {
            puts("Max thread cnt exceeded");
            sleep(1);
        }
    }
}

int main(int argc, char *argv[]) {
    if(argc != 3 && argc != 4) showUsage(argv[0]);
    else {
        int port = 0;
        int as_daemon = !strcmp("-d", argv[1]);
        sscanf(argv[as_daemon?2:1], "%d", &port);
        if(port > 0 && port < 65536) {
            int times = 0;
            sscanf(argv[as_daemon?3:2], "%d", &times);
            if(times > 0) {
                if(!as_daemon || (as_daemon && (daemon(1, 1) >= 0))) {
                        signal(SIGQUIT, handle_pipe);
                        signal(SIGPIPE, handle_pipe);
                        if(bindServer(port, times)) if(listenSocket(times)) acceptClient();
                } else puts("Start daemon error");
            } else printf("Error times: %d\n", times);
        } else printf("Error port: %d\n", port);
    }
    close(fd);
    exit(EXIT_FAILURE);
}