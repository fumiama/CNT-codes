#pragma once
//���ļ���Ӧlnklib.c���ܾ�cpp��������
#include "function.h"
#define FRAME_HEAD_LEN (8+8+8+8)
//֡ͷ+β���ȣ�����Ϊ ֡ͷ8 SRC8 DST8 ACK8 (DATA) CRC16 ֡β8
#define FRAME_INFO_LEN (8+8+8+8+16+8)
//������ݳ���
#define MAX_DATA_LEN 512

#define IS_ACK_FRAME 0x01
#define IS_ERR_FRAME 0x02
#define IS_DAT_FRAME 0x04
static U8 prev_send_buf[MAX_BUFFER_SIZE];		//�洢ǰһ��֡�Ա��ش�
static size_t prev_send_bufsz;								//ǰһ��֡��

void save_send_buf(U8* buf, size_t size);
int unpack_frame(U8* buf, size_t len, int is_bit_arr);
//��װ֡�����ط�װ�õ�֡��ָ��
U8* pack_frame(U8* buf, size_t len, int is_bit_arr);
void go_next_frame();
void check_recv(U8* buf, size_t len, int is_bit_arr);