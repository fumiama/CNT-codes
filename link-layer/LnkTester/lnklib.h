#pragma once
//本文件对应lnklib.c，拒绝cpp从我做起
#include "function.h"
#define FRAME_HEAD_LEN (8+8+8+8)
//帧头+尾长度，依次为 帧头8 SRC8 DST8 ACK8 (DATA) CRC16 帧尾8
#define FRAME_INFO_LEN (8+8+8+8+16+8)
//最大数据长度
#define MAX_DATA_LEN 512

#define IS_ACK_FRAME 0x01
#define IS_ERR_FRAME 0x02
#define IS_DAT_FRAME 0x04
static U8 prev_send_buf[MAX_BUFFER_SIZE];		//存储前一个帧以备重传
static size_t prev_send_bufsz;								//前一个帧长

void save_send_buf(U8* buf, size_t size);
int unpack_frame(U8* buf, size_t len, int is_bit_arr);
//封装帧，返回封装好的帧的指针
U8* pack_frame(U8* buf, size_t len, int is_bit_arr);
void go_next_frame();
void check_recv(U8* buf, size_t len, int is_bit_arr);