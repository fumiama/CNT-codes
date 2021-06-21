#pragma once
#ifndef _APP_H_
#define _APP_H_
#include <stdint.h>
#include "function.h"

#define MAX_PACKAGE_LEN 1024

enum TCP_TYPE {DATA, ACK_DAT, ACT_SEND, ACK_SEND, ACT_RECV, ACK_RECV};
struct TCP {
	uint8_t dst, src;
	TCP_TYPE	 type;
	int len;
	U8 data[];
};
typedef struct TCP TCP;

void init_app_window();
int send_to_ip(uint8_t ip, char* path);
void recv_to(char* path);
void recv_buf(U8* buf, int len);
void set_progress(int progress, int is_send);
void action_recv(uint8_t peer_ip);
void set_ip(uint8_t ip);
void release_wait();
void ack_send();
void send_type_package(uint8_t ip, TCP_TYPE type);

#endif