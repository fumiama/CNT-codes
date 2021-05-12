#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Windows.h>
#include "lnklib.h"

void save_send_buf(U8* buf, size_t size) {
	is_waiting = 1;
	memcpy(prev_send_buf, buf, size);
	prev_send_bufsz = size;
}

void check_recv(U8* buf, size_t len, int is_bit_arr) {
	unsigned int head_len = is_bit_arr ? FRAME_HEAD_LEN : (FRAME_HEAD_LEN / 8);;
	unsigned int info_len = is_bit_arr ? FRAME_INFO_LEN : (FRAME_INFO_LEN / 8);;
	switch (unpack_frame(buf, len, is_bit_arr)) {
	case IS_ACK_FRAME:
		go_next_frame();
		break;
	case IS_DAT_FRAME:
		if (is_bit_arr) {
			size_t sz = len / 8 + 1;
			U8* tmp = (U8*)malloc(sz);
			BitArrayToByteArray(this_frame, this_frame_size, tmp, sz);
			SendtoUpper(&tmp[FRAME_HEAD_LEN - 16], sz - FRAME_INFO_LEN + 16);
		} else SendtoUpper(&buf[head_len - 2], len - info_len + 2);
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

U8* detect_frame(U8* buf, size_t len, int is_bit_arr) {
	if (is_bit_arr) {
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
	else return NULL;
}

int CRC16_receive_check(char* pDataIn, int iLenIn)
//����ֵΪ1����-1 ����1��ʾУ��ɹ���-1��ʾУ��ʧ�� ����Ϊ���������飬iLenInΪ���� ����CRC_returnΪУ��ɹ���ɾȥУ��λ�Ľ��
{
	int wResult = 0;
	int wTableNo = 0;
	int i = 0;
	int CRC_i_1, CRC_i_2;
	char* CRC_1;
	CRC_1 = (char*)malloc(sizeof(char) * (iLenIn + CRC_len) * 2);
	char* CRC_return;
	CRC_return = (char*)malloc(sizeof(char) * (iLenIn + CRC_len) * 2);
	unsigned char* CRC;
	CRC = (unsigned char*)malloc(sizeof(unsigned char) * (iLenIn + CRC_len) * 2);
	if (iLenIn % 8 != 0)
	{
		for (i = 0; i < iLenIn - 16; i++)
		{
			CRC_1[i] = pDataIn[i];
			//printf("%c", CRC_1[i]);
		}
		for (i = 0; i < 8 - iLenIn % 8; i++)
		{
			CRC_1[i + iLenIn - 16] = 0;
		}
		for (i = 0; i < 8; i++)
		{
			CRC_1[i + iLenIn - 8 - iLenIn % 8] = pDataIn[iLenIn - 8 + i];
		}
		for (i = 0; i < 8; i++)
		{
			CRC_1[i + iLenIn - iLenIn % 8] = pDataIn[iLenIn - 16 + i];
		}
	}
	else
	{
		for (i = 0; i < iLenIn - 16; i++)
		{
			CRC_1[i] = pDataIn[i];
		}
		for (i = 0; i < 8; i++)
		{
			CRC_1[i + iLenIn - 16] = pDataIn[i + iLenIn - 8];
		}
		for (i = 0; i < 8; i++)
		{
			CRC_1[i + iLenIn - 8] = pDataIn[i + iLenIn - 16];
		}
	}
	int m_location = 0;
	int CRC_location = 0;
	m_location = iLenIn + 8 - iLenIn % 8;
	int location_sum = 0;
	int sum_2 = 0;
	for (CRC_i_1 = 0; CRC_i_1 < m_location; CRC_i_1 = CRC_i_1 + 8)
	{
		for (CRC_i_2 = 0; CRC_i_2 < 8; CRC_i_2++)
		{
			sum_2 = (int(CRC_1[location_sum + 7 - CRC_i_2])) * pow(2, CRC_i_2) + sum_2;
		}
		location_sum = location_sum + 8;
		CRC[CRC_location] = char(sum_2);
		CRC_location++;
		sum_2 = 0;
	}
	for (i = 0; i < CRC_location; i++)
	{
		wTableNo = ((wResult & 0xff) ^ (CRC[i] & 0xff));
		wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
	}
	if (wResult != 0)
	{
		printf("У��ʧ��");
		return -1;
	}
	//printf_s("\n%d\n", iLenIn);
	for (i = 0; i < iLenIn - 16; i++)
	{
		CRC_return[i] = CRC_1[i];
	}
	free(CRC_1);
	free(CRC);
	free(CRC_return);
	return 1;
}

int unpack_frame(U8* buf, size_t len, int is_bit_arr) {
	if (is_bit_arr) {
		U8* frame = detect_frame(buf, len, is_bit_arr);
		if (frame) {
			if (CRC16_receive_check((char*)frame, len)) {
				this_frame = frame;
				if (frame[23]) return IS_ACK_FRAME;
				else return IS_DAT_FRAME;
			}
			else free(frame);
		}
		return IS_ERR_FRAME;
	}
	else return IS_ERR_FRAME;
}

U8* pack_frame(U8* buf, size_t len, char is_bit_arr) {
	if (is_bit_arr) {
		int l;
		return pack_frame(buf, len, &l);
	}
	return NULL;
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
	for (i = 0; i < 8; i++) {
		buf_return[i] = destination[i];
		//printf_s("%c", buf_return[i]);
	}
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
		free(buf);
	}
	return rbuf;
}

static int transfer(int x, char* a)
{
	int p = 1, y = 0, yushu, count = 0;;
	while (1)
	{
		yushu = x % 2;
		a[count] = char(yushu);
		count++;
		x /= 2;
		y += yushu * p;
		p *= 10;
		if (x < 2)
		{
			a[count] = char(yushu);
			count++;
			y += x * p;
			break;
		}
	}
	return count;
}

U8* append_crc16(U8* pDataIn, int iLenIn) //�������Ϊbuf���� �� len ����;
{
	int wResult = 0;
	int wTableNo = 0;
	int i = 0;
	int CRC_i_1, CRC_i_2;
	char* CRC_1;
	CRC_1 = (char*)malloc(sizeof(char) * (iLenIn + CRC_len) * 2);
	U8* CRC_return = (U8*)malloc(sizeof(char) * (iLenIn + CRC_len) * 2);
	unsigned char* CRC;
	CRC = (unsigned char*)malloc(sizeof(unsigned char) * (iLenIn + CRC_len) * 2);
	if (iLenIn % 8 != 0)
	{
		for (i = 0; i < iLenIn; i++)
		{
			CRC_1[i] = pDataIn[i];
		}
		for (i = 0; i < 8 - iLenIn % 8; i++)
		{
			CRC_1[i + iLenIn] = 0;
		}
	}
	else
	{
		for (i = 0; i < iLenIn; i++)
		{
			CRC_1[i] = pDataIn[i];
		}
	}
	int m_location = 0;
	int CRC_location = 0;
	m_location = iLenIn + 8 - iLenIn % 8;
	int location_sum = 0;
	int sum_2 = 0;
	for (CRC_i_1 = 0; CRC_i_1 < m_location; CRC_i_1 = CRC_i_1 + 8)
	{
		for (CRC_i_2 = 0; CRC_i_2 < 8; CRC_i_2++)
		{
			sum_2 = CRC_1[location_sum + 7 - CRC_i_2] * pow(2, CRC_i_2) + sum_2;
		}
		location_sum = location_sum + 8;
		CRC[CRC_location] = char(sum_2);
		CRC_location++;
		sum_2 = 0;
	}

	for (i = 0; i < CRC_location; i++)
	{
		wTableNo = ((wResult & 0xff) ^ (CRC[i] & 0xff));
		wResult = ((wResult >> 8) & 0xff) ^ wCRC16Table[wTableNo];
	}
	//У����ΪwResult;
	int len_to2 = 0;
	char CRC_arry[16] = { 0 };
	char CRC_arry_1[16] = { 0 };
	len_to2 = transfer(wResult, CRC_arry);
	if (len_to2 < 16)
	{
		for (CRC_i_1 = 0; CRC_i_1 < 16 - len_to2; CRC_i_1++)
		{
			CRC_arry_1[CRC_i_1] = 0;
		}
		for (CRC_i_2 = 0; CRC_i_2 < len_to2; CRC_i_2++)
		{
			CRC_arry_1[16 - len_to2 + CRC_i_2] = CRC_arry[len_to2 - 1 - CRC_i_2];
		}
	}
	else {
		for (CRC_i_2 = 0; CRC_i_2 < len_to2; CRC_i_2++)
		{
			CRC_arry_1[CRC_i_2] = CRC_arry[len_to2 - CRC_i_2];
		}
	}
	for (CRC_i_1 = 0; CRC_i_1 < iLenIn; CRC_i_1++)
	{
		CRC_return[CRC_i_1] = CRC_1[CRC_i_1];
	}
	for (CRC_i_1 = 0; CRC_i_1 < 16; CRC_i_1++)
	{
		CRC_return[iLenIn + CRC_i_1] = CRC_arry_1[CRC_i_1];
	}
	// CRC_returnΪ���ر�����������Դ�ӡ�۲�
	//iLenIn = iLenIn + CRC_i_1;//����֮�󳤶ȱ�Ϊ��iLenIn
	free(CRC_1);
	free(CRC);
	free(pDataIn);
	return CRC_return;
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
	the_location_memory--;
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

void auto_send() {
	U8* dst = (U8*)"\1\1\1\1\1\1\1\1";		//ģ��Ŀ�ĵ�ַ
	U8* src = (U8*)"\1\0\1\0\1\0\1\0";		//ģ��Դ��ַ
	while (iWorkMode / 10) {
		U8* buf = (U8*)malloc(16);		//ģ��Ӹ߲���������
		for (int i = 0; i < 16; i++) {
			if (buf[i]) buf[i] = 1;
		}
		printf("��������: ");
		for(int i = 0; i < 16; i++) {
			putchar(buf[i] + 48);
		}
		putchar('\n');
		U8* buf2crc = add_src_addr(add_dest_addr(add_ack_head(buf, 16, 0), dst, 16 + 8), src, 16 + 8 + 8);
		printf("buf2crc: ");
		for (int i = 0; i < 40; i++) {
			putchar(buf2crc[i] + 48);
		}
		putchar('\n');
		U8* buf2pack = append_crc16(buf2crc, 16 + 8 + 8 + 8);
		printf("buf2pack: ");
		for (int i = 0; i < 56; i++) {
			putchar(buf2pack[i] + 48);
		}
		putchar('\n');
		int len = 0;
		U8* bitframe = pack_frame(buf2pack, 16 + 8 + 8 + 8 + 16, &len);
		printf("frame: ");
		for (int i = 0; i < len; i++) {
			putchar(bitframe[i] + 48);
		}
		putchar('\n');
		//U8* byteframe = (U8*)malloc(len/8 + 1);
		//BitArrayToByteArray(bitframe, len, byteframe, len/8+1);
		//free(bitframe);
		//int l = call_send_to_lower(byteframe, len / 8 + 1, 0);
		//printf("������%dλ\n", l);
		SendtoLower(bitframe, len, 0);
		free(bitframe);
		//free(byteframe);
		Sleep(1000);
	}
}