#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "mac.h"

static int ifNoOfDst[MAC_ADDR_SZ];	//端口映射表

static int get_ifno_of(U8* dst) {
	U8 c;
	BitArrayToByteArray(dst, 8, &c, 1);
	if (c < MAC_ADDR_SZ) return ifNoOfDst[c];
	else return -1;
}

//失败返回-1
int send_to_dst_and_wait(U8* dst, U8* data, int len) {
	int ifNo = get_ifno_of(dst);
	printf("从端口%d发往", ifNo);
	for (int i = 0; i < 8; i++) {
		putchar(dst[i] + 48);
	}
	putchar('\n');
	if (ifNo >= 0) {
		U8* bitframe = pack_frame(data, local_addr_src, dst, &len, 1, 0);
		//free(data);
		send_and_wait(bitframe, len, ifNo);
	}
	return ifNo;
}

static int set_map_of(U8* dst, int ifNo) {
	U8 c;
	BitArrayToByteArray(dst, 8, &c, 1);
	if (c < MAC_ADDR_SZ) {
		printf("设置地址%d对应于端口%d\n", c, ifNo);
		ifNoOfDst[c] = ifNo;
		return 1;
	}
	else return 0;
}

void set_local_addr(int addr) {
	U8 a = (U8)addr;
	U8 ba;
	ByteArrayToBitArray(&ba, 8, &a, 1);
	memcpy(local_addr_src, &ba, 8);
	printf("本机MAC: %d\n", a);
}

void flood(U8* frame_body, int* len, int exclude) {
	//U8* dst = (U8*)"\1\1\1\1\1\1\1\1";
	int l = *len;
	for (int i = 0; i < lowerNumber; i++) {
		if (i != exclude) {
			U8* bitframe = pack_frame(frame_body, local_addr_src, NULL, len, 0, 0);
			SendtoLower(bitframe, *len, i);	//不等待ACK
			*len = l;
		}
	}
	free(frame_body);
}

void broadcast(U8* buf, int* len, int is_notificaton) {
	U8* dst = (U8*)"\1\1\1\1\1\1\1\1";
	U8* oldbuf = buf;
	buf = add_src_addr(add_dest_addr(add_ack_head(buf, *len, 0), dst, *len + 8), local_addr_src, *len + 8 + 8);
	set_ttl(buf);
	*len += 24;
	free(oldbuf);
	if(is_notificaton) buf[22] = 1;
	flood(buf, len, -1);
	//free(buf);
}

void hello(U8* buf, int* len) {
	U8* dst = (U8*)"\0\0\0\0\0\0\0\0";
	U8* oldbuf = buf;
	buf = add_src_addr(add_dest_addr(add_ack_head(buf, *len, 0), dst, *len + 8), local_addr_src, *len + 8 + 8);
	set_ttl(buf);
	*len += 24;
	free(oldbuf);
	flood(buf, len, -1);
	//free(buf);
}

int is_broadcast_frame(U8* frame_body) {
	char* dst = (char*)frame_body + 8;
	return strstr(dst, "\1\1\1\1\1\1\1\1") == dst;
}

int is_notification(U8* frame_body) {
	return frame_body[22];
}

void on_recv_notification(U8* frame_body, int len, int ifNo) {
	printf("从端口%d收到广播帧\n", ifNo);
	set_map_of(frame_body, ifNo);
	flood(frame_body, &len, ifNo);
}

static void _notify_my_existance() {
	int len;
	while (1) {
		Sleep(23333);
		puts("发送通告广播帧");
		U8* buf = (U8*)malloc(8);
		for (int i = 0; i < 8; i++) {
			if (buf[i]) buf[i] = 1;
		}
		len = 8;
		broadcast(buf, &len, 1);
		Sleep(233333);
	}
}

void notify_my_existance() {
	_beginthread((_beginthread_proc_type)_notify_my_existance, 0, NULL);
}

void set_ttl(U8* frame_body) {
	frame_body[16] = frame_body[17] = frame_body[18] = frame_body[19] = frame_body[20] = frame_body[21] = 1;
}

void decrease_ttl(U8* frame_body) {
	U8 ack;
	BitArrayToByteArray(frame_body + 16, 8, &ack, 1);
	ack -= 0x04;
	ByteArrayToBitArray(frame_body + 16, 8, &ack, 1);
	printf("递减TTL到: %d\n", (ack & 0xfc) >> 2);
}

int get_ttl(U8* frame_body) {
	U8 ack;
	BitArrayToByteArray(frame_body + 16, 8, &ack, 1);
	printf("TTL: %d\n", (ack & 0xfc) >> 2);
	return (ack & 0xfc) >> 2;
}

int send_from_me(U8* frame_body) {
	//printf("源:%llu, 本机:%llu\n", *((uint64_t*)frame_body), *((uint64_t*)local_addr_src));
	return *((uint64_t*)frame_body) == *((uint64_t*)local_addr_src);
}

int send_to_me(U8* frame_body) {
	//printf("目的:%llu, 本机:%llu\n", *((uint64_t*)(frame_body + 8)), *((uint64_t*)local_addr_src));
	return *((uint64_t*)(frame_body + 8)) == *((uint64_t*)local_addr_src);
}

int is_hello_package(U8* frame_body) {
	printf("hello:%llu\n", *((uint64_t*)(frame_body + 8)));
	return *((uint64_t*)(frame_body + 8)) == (uint64_t)0;
}