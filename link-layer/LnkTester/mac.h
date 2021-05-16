#pragma once
#ifndef _MAC_H_
#define _MAC_H_
#include "lnklib.h"

#define MAC_ADDR_SZ 255
static U8 local_addr_src[8];	//±¾½»»»»úmac

int send_to_dst_and_wait(U8* dst, U8* data, int len);
void set_local_addr(int addr);
void flood(U8* frame_body, int* len, int exclude);
void broadcast(U8* buf, int* len, int is_notificaton);
int is_broadcast_frame(U8* frame_body);
int is_notification(U8* frame_body);
void on_recv_notification(U8* frame_body, int len, int ifNo);
void notify_my_existance();
void set_ttl(U8* frame_body);
void decrease_ttl(U8* frame_body);
int get_ttl(U8* frame_body);
int send_from_me(U8* frame_body);
int send_to_me(U8* frame_body);
#endif