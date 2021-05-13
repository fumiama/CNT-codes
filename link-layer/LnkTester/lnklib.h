#pragma once
#ifndef _LNKLIB_H_
#define _LNKLIB_H_
//���ļ���Ӧlnklib.c���ܾ�cpp��������
#include <process.h>
#include "function.h"
#include "crc16.h"
#define FRAME_HEAD_LEN (8+8+8+8)
//֡ͷ+β���ȣ�����Ϊ ֡ͷ8 SRC8 DST8 ACK8 (DATA) CRC16 ֡β8
#define FRAME_INFO_LEN (8+8+8+8+16+8)
//������ݳ���
#define MAX_DATA_LEN_BIT 4096
//���ķ�����Ϣ
#define IS_ACK_FRAME 0x01
#define IS_ERR_FRAME 0x02
#define IS_DAT_FRAME 0x04
static U8* this_frame;
static size_t this_frame_size;
static U8 prev_send_buf[MAX_BUFFER_SIZE];		//�洢ǰһ��֡�Ա��ش�
static size_t prev_send_bufsz;								//ǰһ��֡��
static int is_waiting = 0;												//����Ƿ��ڵȴ�״̬
//���淢�������Ա��ش�
void save_send_buf(U8* buf, size_t size);
//���֡�����ض���õ���Ϣ
int unpack_frame(U8* buf, size_t len, int is_bit_arr);
//��װ֡�����ط�װ�õ�֡��ָ��
U8* pack_frame(U8* buf, U8* src, U8* dst, int* len, char is_bit_arr,  int is_ack);
void go_next_frame();
void check_recv(U8* buf, size_t len, int is_bit_arr);
U8* detect_frame(U8* buf, size_t len, int is_bit_arr);
int CRC16_receive_check(char* pDataIn, int iLenIn);
U8* add_src_addr(U8* buf, U8* source, int len);
U8* add_dest_addr(U8* buf, U8* destination, int len);
U8* add_ack_head(U8* buf, int len, int ack_status);
U8* append_crc16(U8* pDataIn, int iLenIn);
//���Ѿ�����ack src dst crc�Ļ����Ϸ�װ֡
U8* pack_frame(U8* buf, int len, int* len_packed);
//ͣ��
void send_and_wait(U8* bitframe, int len);
void send_ack();
void auto_send();
#endif // !_LNKLIB_H_
