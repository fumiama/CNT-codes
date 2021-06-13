#pragma once
#ifndef _NET_H_
#define _NET_H_
#include <stdint.h>

#define INF_DISTANCE 16
#define ROUTE_TABLE_LEN 128
#define BROADCAST_IP 255
#define HELLO_IP 0
enum PACKTYPE { DATA, HELLO, TABLE };
struct ROUTE_ITEM {
	uint8_t to, next;
	uint16_t distance;
};
typedef struct ROUTE_ITEM ROUTE_ITEM;

#define PKHEADLEN (4 + sizeof(uint32_t))
struct PACKAGE {
	uint8_t src, dst, next, type;
	uint32_t len;
	char data[];
};
typedef struct PACKAGE PACKAGE;

//����⵽�������ڵ��·��ͨ��ʱ���ô˺�������
//���ڵ�·�ɱ�����ʱ���迼���Ƿ������Ҫ����
//�����������������жϡ��ں�����ĩβ��������
//��·�ɱ����£������ͨ�溯��֪ͨ�����ڵ㡣
//��distance>15ʱ������ӽ��ᱻ��������Ϊ���ɴ
void add_route(uint8_t to, uint8_t next, uint16_t distance);

//��ȡȥ��to��ַ����һ���ĵ�ַ���Ҳ�������-1
uint8_t get_next(uint8_t to);

//��ȡȥ��to��ַ�ľ��룬�Ҳ�������-1
uint16_t get_distance(uint8_t to);

//���IP������Ϊͨ�����ת�����򷵻�NULL
//��������Ӧ������
char* unpack(PACKAGE* package, int* len);

//��װIP�������ط�װ���ָ��
PACKAGE* pack(char* data, int* len, uint8_t src, uint8_t dst, uint8_t type);

//�㲥���ݰ�
void broadcast(PACKAGE* package);

//��ʱ����HELLO��
void register_hello();

//���ñ��ڵ�ip
void set_ip(uint8_t ip);

void auto_send();
#endif // _NET_H_
