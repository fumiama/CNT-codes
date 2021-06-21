#include <stdio.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "app.h"
#include "pipe.h"
#include "function.h"

//本节点ip
static uint8_t local_addr_src, recv_ip, send_ip;
static Pipe* pipe;
static volatile int pause_send, pause_recv, is_send_waiting;
static char send_path[512], recv_path[512];

static void read_pipe() {
	while (1) {
		DWORD len;
		char* data = pipe->readPipe(&len);
		printf("Read pipe len: %d\n", len);
		if (data) {
			if (len > 0) {
				if (strstr(data, "action-send:") == data) {
					char* path = data + 12;
					char* peer = strstr(path, "-->");
					if (peer) {
						peer[0] = 0;
						peer += 3;
						send_ip = atoi(peer);
						memcpy(send_path, path, strlen(path));
						send_type_package(send_ip, ACT_RECV);
					}
				}
				else if (strstr(data, "action-recv:") == data) {
					int len = strlen(data) - 12;
					char* d = (char*)malloc(len);
					memcpy(d, data + 12, len - 1);
					d[len - 1] = 0;
					recv_to(d);
				}
				else if (!strcmp(data, "pause-send\n")) pause_send = 1;
				else if (!strcmp(data, "pause-recv\n")) pause_recv = 1;
			}
			free(data);
		}
	}
}

void init_app_window() {
	char buf[512];
	sprintf_s(buf, ".\\FileTransfer.exe %d", local_addr_src);
	pipe = new Pipe(buf);
	_beginthread((_beginthread_proc_type)read_pipe, 0, NULL);
}

static int get_file_size(char* filepath) {
	FILE* fp;
	fopen_s(&fp, filepath, "rb");
	fseek(fp, 0, SEEK_END);
	int len = ftell(fp);
	fclose(fp);
	return len;
}

int send_to_ip(uint8_t ip, char* path) {
	if (pause_send) pause_send = 0;
	else {
		set_progress(100, 1);
		int size = get_file_size(path);
		printf("文件大小:%d\n", size);
		if (size > 0) {
			int remain = size % MAX_PACKAGE_LEN;
			int count = size / MAX_PACKAGE_LEN;
			int total_send = 0;
			printf("分段:%d, 剩余: %d\n", count, remain);
			//发送数据长度
			TCP t;
			t.dst = ip;
			t.src = local_addr_src;
			t.len = size;
			t.type = DATA;
			SendtoLower((U8*)&t, sizeof(TCP), 0);
			TCP* buf3 = (TCP*)malloc(MAX_PACKAGE_LEN + sizeof(TCP));
			FILE* fp;
			fopen_s(&fp, path, "rb");
			buf3->dst = ip;
			buf3->src = local_addr_src;
			buf3->type = DATA;
			buf3->len = MAX_PACKAGE_LEN;
			if (fp) {
				for (int i = 0; i < count; i++) {
					if (fread(buf3->data, MAX_PACKAGE_LEN, 1, fp) == 1) {
						SendtoLower((U8*)(buf3), MAX_PACKAGE_LEN + sizeof(TCP), 0);
						is_send_waiting = 1;
						total_send += MAX_PACKAGE_LEN;
						set_progress(100 * total_send / size, 1);
						U8 cnt = 255;
						while (is_send_waiting || pause_send) {
							Sleep(10);
							if (!(cnt--)) {
								SendtoLower((U8*)(buf3), MAX_PACKAGE_LEN + sizeof(TCP), 0);
							}
						}
					}
				}
				if (remain) {
					buf3->len = remain;
					if (fread(buf3->data, remain, 1, fp) == 1) {
						SendtoLower((U8*)(buf3), remain + sizeof(TCP), 0);
						is_send_waiting = 1;
						total_send += remain;
						set_progress(100 * total_send / size, 1);
						U8 cnt = 255;
						while (is_send_waiting || pause_send) {
							Sleep(10);
							if (!(cnt--)) {
								SendtoLower((U8*)(buf3), remain + sizeof(TCP), 0);
							}
						}
					}
				}
				fclose(fp);
				free(buf3);
				pipe->sendCommand("fin-send");
			}
			return remain;
		}
		else return size;
	}
}


static int total_recv, recv_size;
static FILE* recv_fp;
void recv_to(char* path) {
	if (pause_recv) pause_recv = 0;
	else if(recv_size == 0) {
		char filepath[1024];
		int len = strlen(path);
		memcpy(filepath, path, len);
		filepath[len] = 0;
		if (path[len - 1] != '\\') strcat_s(filepath, "\\receive.bin");
		else strcat_s(filepath, "receive.bin");
		FILE* fp;
		fopen_s(&fp, "datarecv", "wb");
		puts(filepath);
		if (fp) {
			recv_fp = fp;
			send_type_package(recv_ip, ACK_SEND);
		}
		pipe->sendCommand("ack-recv");
		set_progress(100, 0);
	}
}

static void fin_recv() {
	total_recv = recv_size = recv_ip = 0;
	if (recv_fp) {
		fclose(recv_fp);
		recv_fp = NULL;
	}
	pipe->sendCommand("fin-recv");
}

void recv_buf(U8* buf, int len) {
		if (recv_size == 0 && recv_fp && len > 0) {
			recv_size = len;
		}
		else {
			fwrite(buf, sizeof(char), len, recv_fp);
			total_recv += len;
			set_progress(100 * total_recv / recv_size, 0);
			if (total_recv >= recv_size) fin_recv();
		}
		send_type_package(recv_ip, ACK_DAT);
}

void action_recv(uint8_t peer_ip) {
		recv_ip = peer_ip;
		char buf[512];
		sprintf_s(buf, "action-recv:%d", peer_ip);
		pipe->sendCommand(buf);
}

void set_progress(int progress, int is_send) {
	char buf[512];
	if (is_send) sprintf_s(buf, "progress-send:%d", progress);
	else sprintf_s(buf, "progress-recv:%d", progress);
	pipe->sendCommand(buf);
}

void release_wait() {
	is_send_waiting = 0;
}

void ack_send() {
	pipe->sendCommand("ack-send");
	send_to_ip(send_ip, send_path);
}

void send_type_package(uint8_t ip, TCP_TYPE type) {
	TCP t;
	t.dst = ip;
	t.src = local_addr_src;
	t.type = type;
	t.len = 0;
	SendtoLower((U8*)&t, sizeof(TCP), 0);
}

void set_ip(uint8_t ip) {
	printf("本机ip:%d\n", ip);
	local_addr_src = ip;
}