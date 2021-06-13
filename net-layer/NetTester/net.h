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

//当检测到是其他节点的路由通告时调用此函数更新
//本节点路由表。调用时无需考虑是否真的需要更新
//本函数会自行做出判断。在函数的末尾，如果检测
//到路由表被更新，会调用通告函数通知其他节点。
//当distance>15时本次添加将会被丢弃，认为不可达。
void add_route(uint8_t to, uint8_t next, uint16_t distance);

//获取去向to地址的下一跳的地址，找不到返回-1
uint8_t get_next(uint8_t to);

//获取去向to地址的距离，找不到返回-1
uint16_t get_distance(uint8_t to);

//解封IP包，若为通告包、转发包则返回NULL
//并调用相应处理函数
char* unpack(PACKAGE* package, int* len);

//封装IP包，返回封装后的指针
PACKAGE* pack(char* data, int* len, uint8_t src, uint8_t dst, uint8_t type);

//广播数据包
void broadcast(PACKAGE* package);

//定时发送HELLO包
void register_hello();

//设置本节点ip
void set_ip(uint8_t ip);

void auto_send();
#endif // _NET_H_
