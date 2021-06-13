#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include <process.h>
#include "lnklib.h"
#include "mac.h"
#include "crc16.h"

void save_send_buf(U8* buf, size_t size) {
	//is_waiting = 1;
	memcpy(prev_send_buf, buf, size);
	prev_send_bufsz = size;
}

void check_recv(U8* buf, int len, int ifNo) {
	switch (unpack_frame(buf, len)) {
	case IS_ACK_FRAME:
		go_next_frame();
		break;
	case IS_DAT_FRAME:
		if (is_broadcast_frame(this_frame)) {
			puts("���㲥֡");
			if (!send_from_me(this_frame)) {
				if (get_ttl(this_frame) > 0) {
					if (is_notification(this_frame)) on_recv_notification(this_frame, this_frame_size, ifNo);
					else {
						SendtoUpper(this_frame + 8 + 8 + 8, this_frame_size - 8 - 8 - 8);
						flood(this_frame, &len, ifNo);
					}
				}
				else {
					puts("����TTLΪ0�Ĺ㲥֡");
					free(this_frame);
				}
			}
			else {
				puts("�����Լ����Ĺ㲥֡");
				free(this_frame);
			}
		}
		else if(send_to_me(this_frame)) {		//����
			puts("�յ�����������֡");
			SendtoUpper(this_frame + 8 + 8 + 8, this_frame_size - 8 - 8 - 8);
			send_ack(ifNo);
			free(this_frame);
		}
		else if (is_hello_package(this_frame)) {
			puts("�յ�����������HELLO��");
			SendtoUpper(this_frame + 8 + 8 + 8, this_frame_size - 8 - 8 - 8);
			free(this_frame);
		}
		else {	//ת��
			puts("ת��֡");
			send_ack(ifNo);
			send_to_dst_and_wait(this_frame + 8, this_frame + 24, this_frame_size - 24);
			free(this_frame);
		}
		break;
	case IS_ERR_FRAME:
		puts("�Ѷ�������֡");
		break;
	default:
		break;
	}
}
void go_next_frame() {
	is_waiting = 0;
	prev_send_bufsz = 0;
}

U8* detect_frame(U8* buf, size_t len) {
		int Frameing_i_1, Frameing_i_2;//ѭ��
		int* buf_frameing_test;
		int Frame_Head = 0, Frame_Tail = 0;
		buf_frameing_test = (int*)malloc(sizeof(int) * len);
		U8* buf_memory_return = NULL;
		buf_memory_return = (U8*)malloc(sizeof(char) * ((len + 32) * 2));
		int frame_standard[8] = { 0,1,1,1,1,1,1,0 };
		int frame_standard_Len = 8;
		for (Frameing_i_1 = 0; Frameing_i_1 < len; Frameing_i_1++)
		{
			buf_frameing_test[Frameing_i_1] = buf[Frameing_i_1];
		}
		char* buf_memory;
		buf_memory = (char*)malloc(sizeof(char) * ((len + 32) * 2));
		int frame_count_1 = 5;//5��1ȷ��
		int frame_count_judge = 0;
		int the_location_test = 0, the_location_memory = 0;//λ��ȷ��
		int count_fake_frame = 0;//����������5��1 ���������
		int num_fake_frame = 0;
		for (Frameing_i_1 = 0; Frameing_i_1 <= len - frame_standard_Len; Frameing_i_1++)
		{
			frame_count_judge = 0;
			for (Frameing_i_2 = 0; Frameing_i_2 <= frame_standard_Len; Frameing_i_2++)
			{
				if (buf_frameing_test[Frameing_i_2 + the_location_test] == frame_standard[Frameing_i_2])
				{
					frame_count_judge++;
					if (frame_count_judge == frame_standard_Len)
					{
						if (Frame_Head == 0)
						{
							Frame_Head = Frameing_i_2 + the_location_test;
							printf_s("֡ͷ %d\n", Frame_Head);
						}
						else
						{
							Frame_Tail = Frameing_i_2 + the_location_test;
							printf_s("֡β %d\n", Frame_Tail);
						}
					}

				}

			}
			the_location_test++;
		}
		if (Frame_Head == 0 || Frame_Tail == 0)
		{
			printf_s("û���ҵ�֡ͷ����֡β\n");
			return NULL;
		}
		else
		{
			the_location_test = Frame_Head + 1, the_location_memory = 0;
			for (Frameing_i_1 = 0; Frameing_i_1 < (Frame_Tail - Frame_Head - 8); Frameing_i_1++)
			{
				if (the_location_test > (Frame_Tail - 8))
				{
					break;
				}
				if (Frameing_i_1 < ((Frame_Tail - Frame_Head - 8) - 5))
				{
					buf_memory[the_location_memory] = buf_frameing_test[the_location_test];
					count_fake_frame = 0;
					for (Frameing_i_2 = 0; Frameing_i_2 < frame_count_1; Frameing_i_2++)
					{
						if (buf_frameing_test[the_location_test + Frameing_i_2] == 1)
						{
							count_fake_frame++;
							if (count_fake_frame == 5)
							{
								for (Frameing_i_2 = 0; Frameing_i_2 < frame_count_1 - 1; Frameing_i_2++)
								{
									the_location_memory++;
									the_location_test++;
									buf_memory[the_location_memory] = buf_frameing_test[the_location_test];
								}
								num_fake_frame++;
								the_location_test++;
								//buf_memory[the_location_memory] = 0;
								count_fake_frame = 0;
							}
						}
					}
					the_location_memory++;
					the_location_test++;
				}
				else
				{
					buf_memory[the_location_memory] = buf_frameing_test[the_location_test];
					the_location_memory++;
					the_location_test++;
				}
			}
			/*for (Frameing_i_1 = 0;Frameing_i_1 < 8;Frameing_i_1++)
			{
				buf_memory_return[Frameing_i_1] = frame_standard[Frameing_i_1]+48;
				printf_s("%c", buf_memory_return[Frameing_i_1]);
			}*/
			for (Frameing_i_1 = 0; Frameing_i_1 < the_location_memory; Frameing_i_1++)
			{
				buf_memory_return[Frameing_i_1] = buf_memory[Frameing_i_1];
				//printf_s("%c", buf_memory_return[Frameing_i_1]);
			}

			this_frame_size = the_location_memory;//����֮��ĳ���Ϊlen
			printf_s("��������ݳ���Ϊ��%zd\n", this_frame_size);
			//�����õ����봮Ϊ buf_memory_returnΪchar�͵�
			free(buf_frameing_test);
			free(buf_memory);
			return buf_memory_return;
		}
}

int CRC16_receive_check(char* pDataIn, int iLenIn)
//����ֵΪ0Ϊ�ɹ�
{
	U8* bytearr = (U8*)malloc(iLenIn / 8);
	BitArrayToByteArray((U8*)pDataIn, iLenIn, bytearr, iLenIn / 8);
	uint16_t* thiscrc = (uint16_t*)(bytearr + iLenIn / 8 - 2);
	uint16_t calccrc = crc16((char*)bytearr, iLenIn / 8 - 2);
	this_frame = (U8*)pDataIn;
	this_frame_size = iLenIn - 16;
	return *thiscrc - calccrc;
}

int unpack_frame(U8* buf, size_t len) {
		U8* frame = detect_frame(buf, len);
		if (frame) {
			printf("recv frame: ");
			for (int i = 0; i < this_frame_size; i++) {
				putchar(frame[i] + 48);
			}
			putchar('\n');
			if (!CRC16_receive_check((char*)frame, this_frame_size)) {
				if (this_frame[23]) return IS_ACK_FRAME;
				else return IS_DAT_FRAME;
			}
			else free(frame);
		}
		return IS_ERR_FRAME;
}

U8* pack_frame(U8* buf, U8* src, U8*dst, int* len, int not_forward, int is_ack) {
	printf("��������: ");
	for (int i = 0; i < *len; i++) {
		putchar(buf[i] + 48);
	}
	putchar('\n');
	U8* buf2crc;
	if (not_forward) {
		buf2crc = add_src_addr(add_dest_addr(add_ack_head(buf, *len, is_ack), dst, *len + 8), src, *len + 8 + 8);
		set_ttl(buf2crc);
		*len += 8 + 8 + 8;
	}
	else {
		buf2crc = (U8*)malloc(*len);
		memcpy(buf2crc, buf, *len);
		decrease_ttl(buf2crc);
	}
	printf("buf2crc:\t");
	for (int i = 0; i < *len; i++) {
		putchar(buf2crc[i] + 48);
	}
	putchar('\n');
	U8* buf2pack = append_crc16(buf2crc, *len);
	*len += 16;
	printf("buf2pack:\t");
	for (int i = 0; i < *len; i++) {
		putchar(buf2pack[i] + 48);
	}
	putchar('\n');
	U8* bitframe = pack_frame(buf2pack, *len, len);
	printf("frame:\t\t");
	for (int i = 0; i < *len; i++) {
		putchar(bitframe[i] + 48);
	}
	putchar('\n');
	return bitframe;
}

//���Դ ��������buf Դ������source[9]="00000000"; ����һ��buf ���� ��󳤶�Ϊlen
U8* add_src_addr(U8* buf, U8* source, int len) {
	int i = 0;
	U8* buf_return = (U8*)malloc(sizeof(char) * len + 9);
	for (i = 0; i < 8; i++) {
		buf_return[i] = source[i];
		//printf_s("%c", buf_return[i]);
	}
	for (i = 0; i < len; i++) {
		buf_return[i + 8] = buf[i];
		//printf_s("%c", buf_return[i + 8]);
	}
	free(buf);
	return buf_return;
}

//���Ŀ�ĵ�
U8* add_dest_addr(U8* buf, U8* destination, int len) {
	int i = 0;
	U8* buf_return = (U8*)malloc(sizeof(char) * len + 9);
	*(uint64_t*)buf_return = *(uint64_t*)destination;
	for (i = 0; i < len; i++) {
		buf_return[i + 8] = buf[i];
		//printf_s("%c", buf_return[i + 8]);
	}
	free(buf);
	return buf_return;
}

U8* add_ack_head(U8* buf, int len, int ack_status) {
	U8* rbuf = (U8*)malloc(len + 8);
	if (rbuf) {
		memset(rbuf, 0, 8);
		if (ack_status) rbuf[7] = 1;	//��ACK֡
		memcpy(rbuf + 8, buf, len);
		//free(buf);
	}
	return rbuf;
}

U8* append_crc16(U8* pDataIn, int iLenIn) //�������Ϊbuf���� �� len ����;
{
	U8* bytearr = (U8*)malloc(iLenIn / 8 + 2);
	BitArrayToByteArray(pDataIn, iLenIn, bytearr, iLenIn / 8);
	uint32_t r = crc16((char*)bytearr, iLenIn / 8);
	U8* appended = (U8*)malloc(iLenIn + 16);
	*(uint16_t*)(bytearr + iLenIn / 8) = r;
	ByteArrayToBitArray(appended, iLenIn + 16, bytearr, iLenIn / 8 + 2);
	free(bytearr);
	return appended;
}

U8* pack_frame(U8* buf, int len, int* len_packed) //��֡ buf_memory_returnΪ��֡���
{
	int Frameing_i_1, Frameing_i_2;//ѭ��
	//int* buf_frameing_test = new int[len];//����buf
	int* buf_frameing_test;
	buf_frameing_test = (int*)malloc(sizeof(int) * len);
	//char* buf_memory_return = new char[(len + 32) * 2];
	U8* buf_memory_return = (U8*)malloc(sizeof(char) * ((len + 32) * 2));
	U8 frame_standard[8] = { 0,1,1,1,1,1,1,0 };
	for (Frameing_i_1 = 0; Frameing_i_1 < len; Frameing_i_1++)
	{
		buf_frameing_test[Frameing_i_1] = buf[Frameing_i_1];
		//printf_s("%d\n",buf_frameing_test[Frameing_i_1]);
	}
	//int *buf_memory = new int[(len+32)*2];//���Ľ�� ������Ȳ��ᳬ��2��֡��
	U8* buf_memory = (U8*)malloc(sizeof(char) * ((len + 32) * 2));
	int frame_count_1 = 5;//5��1ȷ��
	int the_location_test = 0, the_location_memory = 0;//λ��ȷ��
	int count_fake_frame = 0;//����������5��1 ���������
	int num_fake_frame = 0;
	int judge_1 = 0;//����һ���жϣ���Ϊ��������������������һ����󳤶Ȼ��1
	for (Frameing_i_1 = 0; Frameing_i_1 < len; Frameing_i_1++)
	{
		if (the_location_test > len)
		{
			break;
		}
		if (Frameing_i_1 < len - 5)
		{
			buf_memory[the_location_memory] = buf_frameing_test[the_location_test];
			count_fake_frame = 0;
			for (Frameing_i_2 = 0; Frameing_i_2 < frame_count_1; Frameing_i_2++)
			{
				if (buf_frameing_test[the_location_test + Frameing_i_2] == 1)
				{
					count_fake_frame++;
					if (count_fake_frame == 5)
					{
						judge_1++;
						for (Frameing_i_2 = 0; Frameing_i_2 < frame_count_1 - 1; Frameing_i_2++)
						{
							the_location_memory++;
							the_location_test++;
							buf_memory[the_location_memory] = buf_frameing_test[the_location_test];
						}
						num_fake_frame++;
						the_location_memory++;
						buf_memory[the_location_memory] = 0;
						count_fake_frame = 0;
					}
				}
			}
			the_location_memory++;
			the_location_test++;
		}
		else
		{
			buf_memory[the_location_memory] = buf_frameing_test[the_location_test];
			the_location_memory++;
			the_location_test++;
		}
	}
	if (judge_1 > 0)
	{
		the_location_memory--;
	}
	memcpy(buf_memory_return, frame_standard, 8);
	memcpy(buf_memory_return + 8, buf_memory, the_location_memory);
	memcpy(buf_memory_return + 8 + the_location_memory, frame_standard, 8);

	len = the_location_memory + 16;//����֮��ĳ���Ϊlen
	printf_s("����õ�����Ϊ��%d\n", len);
	*len_packed = len;
	//�����õ����봮Ϊ buf_memory_returnΪchar�͵�
	free(buf_frameing_test);
	free(buf_memory);
	free(buf);
	return buf_memory_return;
}

static void _send_and_wait_thread(void* ifNo) {
	is_waiting = 1;
	uint8_t cnt = 0;
	do {
		if (!(cnt--)) SendtoLower(prev_send_buf, prev_send_bufsz, (int)ifNo);
		Sleep(1);
	} while (is_waiting);
	puts("���յ�ACK֡");
}

void send_and_wait(U8* bitframe, int len, int ifNo) {
	if (!is_waiting) {
		save_send_buf(bitframe, len);
		free(bitframe);
		_beginthread((_beginthread_proc_type)_send_and_wait_thread, 0, (int*)ifNo);
	}
	else {
		puts("���ڵȴ����ܾ�����");
		free(bitframe);
	}
}

void send_ack(int ifNo) {
	U8* buf = (U8*)malloc(8);
	int len = 0;
	U8* bitframe = pack_frame(buf, local_addr_src, this_frame, &len, 1, 1);
	SendtoLower(bitframe, len, ifNo);
	free(bitframe);
	free(buf);
	puts("����ACK֡");
}

void auto_send() {
	U8 c;
	printf("���������͵���������: ");
	scanf_s("%d", &c);
	U8* dst = (U8*)malloc(8);		//Ŀ�ĵ�ַ
	ByteArrayToBitArray(dst, 8, &c, 1);
	while (iWorkMode / 10) {
		U8* buf = (U8*)malloc(16);		//ģ��Ӹ߲���������
		for (int i = 0; i < 16; i++) {
			if (buf[i]) buf[i] = 1;
		}
		send_to_dst_and_wait(dst, buf, 16);
		free(buf);
		Sleep(10000);
	}
}