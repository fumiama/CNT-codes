#pragma once
#ifndef _LNKLIB_H_
#define _LNKLIB_H_
//本文件对应lnklib.c，拒绝cpp从我做起
#include <process.h>
#include "function.h"
#include "crc16.h"
#define FRAME_HEAD_LEN (8+8+8+8)
//帧头+尾长度，依次为 帧头8 SRC8 DST8 ACK8 (DATA) CRC16 帧尾8
#define FRAME_INFO_LEN (8+8+8+8+16+8)
//最大数据长度
#define MAX_DATA_LEN_BIT 4096
//解封的返回消息
#define IS_ACK_FRAME 0x01
#define IS_ERR_FRAME 0x02
#define IS_DAT_FRAME 0x04
static U8* this_frame;
static size_t this_frame_size;
static U8 prev_send_buf[MAX_BUFFER_SIZE];		//存储前一个帧以备重传
static size_t prev_send_bufsz;								//前一个帧长
static int is_waiting = 0;												//标记是否处于等待状态
//保存发送数据以备重传
void save_send_buf(U8* buf, size_t size);
//解封帧，返回定义好的消息
int unpack_frame(U8* buf, size_t len, int is_bit_arr);
//封装帧，返回封装好的帧的指针
U8* pack_frame(U8* buf, U8* src, U8* dst, int* len, char is_bit_arr,  int is_ack);
void go_next_frame();
void check_recv(U8* buf, size_t len, int is_bit_arr);
U8* detect_frame(U8* buf, size_t len, int is_bit_arr);
int CRC16_receive_check(char* pDataIn, int iLenIn);
U8* add_src_addr(U8* buf, U8* source, int len);
U8* add_dest_addr(U8* buf, U8* destination, int len);
U8* add_ack_head(U8* buf, int len, int ack_status);
U8* append_crc16(U8* pDataIn, int iLenIn);
//在已经加了ack src dst crc的基础上封装帧
U8* pack_frame(U8* buf, int len, int* len_packed);
//停等
void send_and_wait(U8* bitframe, int len);
void send_ack();
void auto_send();
#endif // !_LNKLIB_H_
