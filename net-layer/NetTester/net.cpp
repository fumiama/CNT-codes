#include <process.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <time.h>
#include "net.h"
#include "function.h"


//·�ɱ�
static ROUTE_ITEM route_table[ROUTE_TABLE_LEN];
//���ڵ�ip
static uint8_t local_addr_src;

void add_route(uint8_t to, uint8_t next, uint16_t distance) {
	if (distance < INF_DISTANCE && to != local_addr_src && next != local_addr_src) {
		int i;
		for (i = 0; i < ROUTE_TABLE_LEN; i++) {
			if (!route_table[i].distance) break;
			else if (route_table[i].to == to) {
				if (distance < route_table[i].distance && route_table[i].next != next) {
					route_table[i].next = next;
					route_table[i].distance = distance;
					ROUTE_ITEM r = { to, next, distance };
					int l = sizeof(ROUTE_ITEM);
					printf("���·��to:%d, next:%d, distance: %d\n", to, next, distance);
					PACKAGE* p = pack((char*)&r, &l, local_addr_src, BROADCAST_IP, TABLE);
					broadcast(p);
					free(p);
				}
				return;
			}
		}
		route_table[i].to = to;
		route_table[i].next = next;
		route_table[i].distance = distance;
		ROUTE_ITEM r = { to, next, distance };
		int l = sizeof(ROUTE_ITEM);
		printf("���·��to:%d, next:%d, distance: %d\n", to, next, distance);
		PACKAGE* package = pack((char*)&r, &l, local_addr_src, BROADCAST_IP, TABLE);
		broadcast(package);
		free(package);
	}
}

uint8_t get_next(uint8_t to) {
	for (int i = 0; i < ROUTE_TABLE_LEN; i++) {
		if (route_table[i].to == to) {
			printf("��ѯ����%d����һ��Ϊ%d\n", to, route_table[i].next);
			return route_table[i].next;
		}
	}
	printf("δ��ѯ����%d����һ��\n", to);
	return 0;
}

uint16_t get_distance(uint8_t to) {
	for (int i = 0; i < ROUTE_TABLE_LEN; i++) {
		if (route_table[i].to == to) {
			printf("��ѯ����%d�ľ���Ϊ%d\n", to, route_table[i].distance);
			return route_table[i].distance;
		}
	}
	printf("��ѯ����%d�ľ���Ϊ%d\n", to, INF_DISTANCE);
	return INF_DISTANCE;
}

static void scan_table(PACKAGE* package) {
	ROUTE_ITEM* table = (ROUTE_ITEM*)package->data;
	int size = package->len / sizeof(ROUTE_ITEM);
	uint8_t distance = get_distance(package->src);
	if (distance > 0 && distance < INF_DISTANCE) for (int i = 0; i < size; i++) add_route(table[i].to, package->src, distance + table[i].distance);
}

char* unpack(PACKAGE* package, int* len) {
	puts("��ʼ���...");
	printf("[%d--%d->%d]����:%d ����:%d\n", package->src, package->next, package->dst, package->type, package->len);
	switch (package->type) {
	case DATA:
		printf("[%u]�յ����ݰ�\n", time(NULL));
		if (package->dst == local_addr_src || package->dst == BROADCAST_IP) {
			*len = package->len;
			return package->data;
		}
		else if (package->next == local_addr_src) {
			package->next = get_next(package->dst);
			SendtoLower((U8*)package, *len, 0);
		}
		break;
	case HELLO:
		puts("�յ�HELLO��");
		add_route(package->src, package->src, 1);
		break;
	case TABLE:
		puts("�յ�·�ɱ�");
		scan_table(package);
		break;
	default:
		break;
	}
	return NULL;
}

PACKAGE* pack(char* data, int* len, uint8_t src, uint8_t dst, uint8_t type) {
	PACKAGE* package = (PACKAGE*)malloc(*len + PKHEADLEN);
	package->src = src;
	package->dst = dst;
	package->len = *len;
	package->type = type;
	package->next = get_next(dst);
	memcpy(package->data, data, *len);
	*len += PKHEADLEN;
	return package;
}

void broadcast(PACKAGE* package) {
	int len = package->len + PKHEADLEN;
	puts("���͹㲥��");
	SendtoLower((U8*)package, len, 0);
}

static void say_hello() {
	while (1) {
		int len = 6;
		puts("����HELLO��");
		PACKAGE* p = pack((char*)"hello", &len, local_addr_src, HELLO_IP, HELLO);
		broadcast(p);
		free(p);
		Sleep(10000);
	}
}

void register_hello() {
	_beginthread((_beginthread_proc_type)say_hello, 0, NULL);
}

void set_ip(uint8_t ip) {
	printf("����ip:%d\n", ip);
	local_addr_src = ip;
}

void send_to(uint8_t ip, char* buf, int len) {
	PACKAGE* p = pack(buf, &len, local_addr_src, ip, DATA);
	SendtoLower((U8*)p, len, 0);
	free(p);
}

void auto_send() {
	uint32_t c;
	printf("���������͵���ip: ");
	scanf_s("%u", &c);
	while (iWorkMode / 10) {
		char* buf = (char*)"data test";		//ģ��Ӹ߲���������
		int len = 10;
		printf("[%u]ģ�ⷢ������\n", time(NULL));
		send_to((uint8_t)c, buf, len);
		Sleep(8000);
	}
}