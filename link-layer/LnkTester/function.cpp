//Nettester �Ĺ����ļ�
#include <iostream>
#include <conio.h>
#include "winsock.h"
#include <cstdio>
#include "CfgFileParms.h"
#include "function.h"
#include "lnklib.h"
#include <process.h>
using namespace std;

//����Ϊ��Ҫ�ı���
int printCount = 0;	//��ӡ����
int spin = 0;				//��ӡ��̬��Ϣ����

//------�����ķָ��ߣ�һЩͳ���õ�ȫ�ֱ���------------
int iSndTotal = 0;						//������������
int iSndTotalCount = 0;			//���������ܴ���
int iSndErrorCount = 0;			//���ʹ������
int iRcvForward = 0;				//ת����������
int iRcvForwardCount = 0;		//ת�������ܴ���
int iRcvToUpper = 0;				//�ӵͲ�ݽ��߲���������
int iRcvToUpperCount = 0;		//�ӵͲ�ݽ��߲������ܴ���
int iRcvUnknownCount = 0;	//�յ�������Դ�����ܴ���

//============�ص�����==============================
//------------��Ҫ�Ŀ��Ʋ���-----------------------------------------------------
//���´������ļ�ne.txt�ж�ȡ����Ҫ����
int lowerMode[10];		//����Ͳ��������ģ����������������õ���ÿ���ӿڶ�Ӧ�����ݸ�ʽ����0Ϊ�������飬1Ϊ�ֽ�����
int lowerNumber;			//�Ͳ�ʵ�����������������֮�¿����ж����·��
int iWorkMode;			//ʵ��Ĺ���ģʽ��ʮλ����1 �Զ����ͣ�0 �ֶ����ͣ���λ�� 1 ��ӡ�������ݣ�0 ��ӡͳ������
char strDevID[128];			//�豸�ţ��ַ�����ʽ����1��ʼ
char strLayer[128];				//�����
char strEntity[128];				//ʵ��ţ��ַ�����ʽ����0��ʼ������ͨ��atoi������������ڳ�����ʹ��

static void check_key() {
	while (1) {
		if (_kbhit()) {
			//�����ж���������˵�ģʽ
			menu();
		}
		Sleep(100);
	}
}
//***************��Ҫ��������******************************
//���ƣ�InitFunction
//���ܣ���ʼ�������棬��main�����ڶ��������ļ�����ʽ������������ǰ����
//���룺
//�����
void InitFunction() {
	_beginthread((_beginthread_proc_type)check_key, 0, NULL);
}

//***************��Ҫ��������******************************
//���ƣ�EndFunction
//���ܣ����������棬��main�������յ�exit������������˳�ǰ����
//���룺
//�����
//void EndFunction() {
//}

//***************��Ҫ��������******************************
//���ƣ�TimeOut
//���ܣ�������������ʱ����ζ��sBasicTimer�����õĳ�ʱʱ�䵽�ˣ�
//      �������ݿ���ȫ���滻Ϊ������Լ����뷨
//      ������ʵ���˼���ͬʱ���й��ܣ����ο�
//      1)����iWorkMode����ģʽ���ж��Ƿ񽫼�����������ݷ��ͣ�
//        ��Ϊscanf�����������¼�ʱ���ڵȴ����̵�ʱ����ȫʧЧ������ʹ��_kbhit()������������ϵ��ڼ�ʱ�Ŀ������жϼ���״̬
//      2)����ˢ�´�ӡ����ͳ��ֵ��ͨ����ӡ���Ʒ��Ŀ��ƣ�����ʼ�ձ�����ͬһ�д�ӡ
//���룺ʱ�䵽�˾ʹ�����ֻ��ͨ��ȫ�ֱ�����������
//�����
void TimeOut() {
	printCount++;
	print_statistics();
}
//------------�����ķָ��ߣ����������ݵ��շ�--------------------------------------------

//***************��Ҫ��������******************************
//���ƣ�RecvfromUpper
//���ܣ�������������ʱ����ζ���յ�һ�ݸ߲��·�������
//      ��������ȫ�������滻��������Լ���
//      ���̹��ܽ���
//         1)ͨ���Ͳ�����ݸ�ʽ����lowerMode���ж�Ҫ��Ҫ������ת����bit�����鷢�ͣ�����ֻ�����Ͳ�ӿ�0��
//           ��Ϊû���κοɹ��ο��Ĳ��ԣ���������Ӧ�ø���Ŀ�ĵ�ַ�ڶ���ӿ���ѡ��ת���ġ�
//         2)�ж�iWorkMode�������ǲ�����Ҫ�����͵��������ݶ���ӡ������ʱ���ԣ���ʽ����ʱ�����齫����ȫ����ӡ��
//���룺U8 * buf,�߲㴫���������ݣ� int len�����ݳ��ȣ���λ�ֽ�
//�����
void RecvfromUpper(U8* buf, int len) {
	if (!is_waiting) {
		int iSndRetval = 0;
		U8* bufSend = NULL;
		//�Ǹ߲����ݣ�ֻ�ӽӿ�0����ȥ,�߲�ӿ�Ĭ�϶����ֽ������ݸ�ʽ
		if (lowerMode[0] == 0) {
			//�ӿ�0��ģʽΪbit���飬��ת����bit���飬�ŵ�bufSend��
			bufSend = (U8*)malloc(len * 8);
			if (bufSend == NULL) {
				return;
			}
			iSndRetval = ByteArrayToBitArray(bufSend, len * 8, buf, len);
			U8* dst = (U8*)"\1\1\1\1\1\1\1\1";		//ģ��Ŀ�ĵ�ַ
			U8* src = (U8*)"\1\0\1\0\1\0\1\0";		//ģ��Դ��ַ
			len *= 8;
			U8* packed_frame = pack_frame(bufSend, src, dst, &len, 1, 0);
			//����
			iSndRetval = SendtoLower(packed_frame, iSndRetval + FRAME_INFO_LEN, 0); //��������Ϊ���ݻ��壬���ȣ��ӿں�
		}
		else {
			//�²����ֽ�����ӿڣ���ֱ�ӷ���
			U8* packed_frame = pack_frame(bufSend, len, 0);
			iSndRetval = SendtoLower(buf, len + FRAME_INFO_LEN / 8, 0);
			iSndRetval = iSndRetval * 8;//�����λ
		}
		//����������ͣ��Э����ش�Э�飬���������Ҫ����������Ӧ����������ռ䣬��buf��bufSend�����ݱ����������Ա��ش�
		if (bufSend != NULL) {
			//����bufSend����
			save_send_buf(bufSend, lowerMode[0] ? len : (len * 8));
			//���ռ��ͷ�
			free(bufSend);
		}
		else puts("���ڵȴ�״̬�������߲�����");
		//ͳ��
		if (iSndRetval <= 0) {
			iSndErrorCount++;
		}
		else {
			iSndTotal += iSndRetval;
			iSndTotalCount++;
		}
	}
	//printf("\n�յ��ϲ����� %d λ�����͵��ӿ�0\n", retval * 8);
	//��ӡ
	switch (iWorkMode % 10) {
	case 1:
		cout << endl << "�߲�Ҫ����ӿ� 0 �������ݣ�" << endl;
		print_data_bit(buf, len, 1);
		break;
	case 2:
		cout << endl << "�߲�Ҫ����ӿ� 0 �������ݣ�" << endl;
		print_data_byte(buf, len, 1);
		break;
	default: break;
	}
}
//***************��Ҫ��������******************************
//���ƣ�RecvfromLower
//���ܣ�������������ʱ����ζ�ŵõ�һ�ݴӵͲ�ʵ��ݽ�����������
//      ��������ȫ�������滻���������Ҫ������
//      ���̹��ܽ��ܣ�
//          1)ת�����Ͻ�ǰ���ж��ս����ĸ�ʽ��Ҫ���ͳ�ȥ�ĸ�ʽ�Ƿ���ͬ��������bit��������ֽ�������֮��ʵ��ת��
//            ע����Щ�жϲ������������ݱ�����������������������ļ������������ļ��Ĳ���д���ˣ��ж�Ҳ�ͻ�ʧ��
//          2)����iWorkMode���ж��Ƿ���Ҫ���������ݴ�ӡ
//���룺U8 * buf,�Ͳ�ݽ����������ݣ� int len�����ݳ��ȣ���λ�ֽڣ�int ifNo ���Ͳ�ʵ����룬�����������ĸ��Ͳ�
//�����
void RecvfromLower(U8* buf, int len, int ifNo) {
	if (ifNo == 0) {
		check_recv(buf, len, !lowerMode[0]);
	}
	//��ӡ
	cout << endl << "���սӿ� " << ifNo << " ���ݣ�" << endl;
	switch (iWorkMode % 10) {
	case 1:
		print_data_bit(buf, len, lowerMode[ifNo]);
		break;
	case 2:
		print_data_byte(buf, len, lowerMode[ifNo]);
		break;
	case 0:
		break;
	}
}

void print_statistics() {
	if (printCount % 10 == 0) {
		switch (spin) {
		case 1:
			printf("\r-");
			break;
		case 2:
			printf("\r\\");
			break;
		case 3:
			printf("\r|");
			break;
		case 4:
			printf("\r/");
			spin = 0;
			break;
		}
		cout << "��ת�� "<< iRcvForward<< " λ��"<< iRcvForwardCount<<" �Σ�"<<"�ݽ� "<< iRcvToUpper<<" λ��"<< iRcvToUpperCount<<" ��,"<<"���� "<< iSndTotal <<" λ��"<< iSndTotalCount<<" �Σ�"<< "���Ͳ��ɹ� "<< iSndErrorCount<<" ��,""�յ�������Դ "<< iRcvUnknownCount<<" �Ρ�";
		spin++;
	}
}

static U8 kbBuf[128];									//����������
void menu() {
	int selection;
	unsigned short port;
	int len;
	//����|��ӡ��[���Ϳ��ƣ�0���ȴ��������룻1���Զ���][��ӡ���ƣ�0�������ڴ�ӡͳ����Ϣ��1����bit����ӡ���ݣ�2���ֽ�����ӡ����]
	cout << endl << endl << "�豸��:" << strDevID << ",    ���:" << strLayer << ",    ʵ���:" << strEntity;
	cout << endl << "1-�����Զ�����; " << endl << "2-ֹͣ�Զ�����; " << endl << "3-�Ӽ������뷢��; ";
	cout << endl << "4-����ӡͳ����Ϣ; " << endl << "5-����������ӡ��������;" << endl << "6-���ֽ�����ӡ��������;";
	cout << endl << "0-ȡ��" << endl << "����������ѡ�����";
	cin >> selection;
	switch (selection) {
	case 0:
		break;
	case 1:
		iWorkMode = 10 + iWorkMode % 10;
		_beginthread((_beginthread_proc_type)auto_send, 0, NULL);
		break;
	case 2:
		iWorkMode = iWorkMode % 10;
		break;
	case 3:
		cout << "�����ַ���(������128�ַ�)��";
		cin >> kbBuf;
		cout << "����Ͳ�ӿںţ�";
		cin >> port;
		len = (int)strlen((const char*)kbBuf) + 1; //�ַ�������и�������
		if (port >= lowerNumber) {
			cout << "û������ӿ�" << endl;
			return;
		}
		else call_send_to_lower(kbBuf, len, port);
		break;
	case 4:
		iWorkMode = (iWorkMode / 10) * 10 + 0;
		break;
	case 5:
		iWorkMode = (iWorkMode / 10) * 10 + 1;
		break;
	case 6:
		iWorkMode = (iWorkMode / 10) * 10 + 2;
		break;
	}
}

//------------�����ķָ��ߣ�������һЩ���ݴ���Ĺ��ߺ����������ã�û��Ҫ��------------------------------
//*************************************************
//���ƣ�code
//���ܣ�������x�е�ָ��λ�����Ŵ�A[]������������У����鰴8�ı�����
//���룺x�����Ŵ���������������length���ȵ�λ��
//�����A[],�Ŵ��ı�������
void code(unsigned long x, U8 A[], int length) {
	unsigned long test;
	int i;
	//��λ��ǰ
	test = 1;
	test = test << (length - 1);
	for (i = 0; i < length; i++) {
		if (test & x) {
			A[i] = 1;
		}
		else {
			A[i] = 0;
		}
		test = test >> 1; //���㷨��������λ������"��"���㣬��λ���x��ÿһλ��0����1.
	}
}
//���ƣ�decode
//���ܣ��ѱ�������A[]��ĸ�λ��Ԫ�أ�����С�Żص�һ�������У�������lengthλ�����鰴8�ı�����
//���룺��������A[],��Ҫ�仯��λ��
//�������С�󣬻�ԭ������
unsigned long decode(U8 A[], int length) {
	unsigned long x;
	int i;

	x = 0;
	for (i = 0; i < length; i++) {
		if (A[i] == 0) {
			x = x << 1;;
		}
		else {
			x = x << 1;
			x = x | 1;
		}
	}
	return x;
}
//���ƣ�ByteArrayToBitArray
//���ܣ����ֽ��������Ŵ�Ϊ����������
//���룺 int iBitLen����λ������, U8* byteA�������Ŵ��ֽ�����, int iByteLen�����ֽ����鳤��
//�������������ֵ��ת�����ж���λ��
//      U8* bitA,�������飬ע���������Ŀռ䣨��������С����Ӧ���ֽ������8��
int ByteArrayToBitArray(U8* bitA, int iBitLen, U8* byteA, int iByteLen) {
	int i;
	int len;

	len = min(iByteLen, iBitLen / 8);
	for (i = 0; i < len; i++) {
		//ÿ�α���8λ
		code(byteA[i], &(bitA[i * 8]), 8);
	}
	return len * 8;
}
//*************************************************
//���ƣ�BitArrayToByteArray
//���ܣ����ֽ��������Ŵ�Ϊ����������
//���룺U8* bitA,�������飬int iBitLen����λ������,  int iByteLen�����ֽ����鳤��
//      ע���������Ŀռ䣨��������С����Ӧ���ֽ������8��
//���������ֵ��ת�����ж��ٸ��ֽڣ����λ�����Ȳ���8λ�������������1�ֽڲ�����
//      U8* byteA������С����ֽ�����,��
int BitArrayToByteArray(U8* bitA, int iBitLen, U8* byteA, int iByteLen) {
	int i;
	int len;
	int retLen;

	len = min(iByteLen * 8, iBitLen);
	if (iBitLen > iByteLen * 8) {
		//�ض�ת��
		retLen = iByteLen;
	}
	else {
		if (iBitLen % 8 != 0)
			retLen = iBitLen / 8 + 1;
		else
			retLen = iBitLen / 8;
	}

	for (i = 0; i < len; i += 8) {
		byteA[i / 8] = (U8)decode(bitA + i, 8);
	}
	return retLen;
}
//*************************************************
//���ƣ�print_data_bit
//���ܣ�����������ʽ��ӡ���ݻ���������
//���룺U8* A������������, int length����λ��, int iMode����ԭʼ���ݸ�ʽ��0Ϊ���������飬1Ϊ�ֽ�����
//�����ֱ����Ļ��ӡ
void print_data_bit(U8* A, int length, int iMode) {
	int i, j;
	U8 B[8];
	int lineCount = 0;
	cout << endl << "���ݵ�λ����" << endl;
	if (iMode == 0) {
		for (i = 0; i < length; i++) {
			lineCount++;
			if (A[i] == 0) {
				printf("0 ");
			}
			else {
				printf("1 ");
			}
			if (lineCount % 8 == 0) {
				printf(" ");
			}
			if (lineCount >= 40) {
				printf("\n");
				lineCount = 0;
			}
		}
	}
	else {
		for (i = 0; i < length; i++) {
			lineCount++;
			code(A[i], B, 8);
			for (j = 0; j < 8; j++) {
				if (B[j] == 0) {
					printf("0 ");
				}
				else {
					printf("1 ");
				}
				lineCount++;
			}
			printf(" ");
			if (lineCount >= 40) {
				printf("\n");
				lineCount = 0;
			}
		}
	}
	printf("\n");
}
//*************************************************
//���ƣ�print_data_byte
//���ܣ����ֽ���������ʽ��ӡ���ݻ���������,ͬʱ��ӡ�ַ���ʮ�����������ָ�ʽ
//���룺U8* A������������, int length����λ��, int iMode����ԭʼ���ݸ�ʽ��0Ϊ���������飬1Ϊ�ֽ�����
//�����ֱ����Ļ��ӡ
void print_data_byte(U8* A, int length, int iMode) {
	int linecount = 0;
	int i;

	if (iMode == 0) {
		length = BitArrayToByteArray(A, length, A, length);
	}
	cout << endl << "���ݵ��ַ�����ʮ�������ֽ���:" << endl;
	for (i = 0; i < length; i++) {
		linecount++;
		printf("%c ", A[i]);
		if (linecount >= 40) {
			printf("\n");
			linecount = 0;
		}
	}
	printf("\n");
	linecount = 0;
	for (i = 0; i < length; i++) {
		linecount++;
		printf("%02x ", (unsigned char)A[i]);
		if (linecount >= 40) {
			printf("\n");
			linecount = 0;
		}
	}
	printf("\n");
}

int call_send_to_lower(U8* bufSend, int len, int port) {
	int iSndRetval;
	if (lowerMode[port] == 0) {
		//�²�ӿ��Ǳ���������,��ҪһƬ�µĻ�����ת����ʽ
		bufSend = (U8*)malloc(len * 8);

		iSndRetval = ByteArrayToBitArray(bufSend, len * 8, kbBuf, len);
		iSndRetval = SendtoLower(bufSend, iSndRetval, port);
		free(bufSend);
	}
	else {
		//�²�ӿ����ֽ����飬ֱ�ӷ���
		iSndRetval = SendtoLower(kbBuf, len, port);
		iSndRetval = iSndRetval * 8; //�����λ
	}
	//����ͳ��
	if (iSndRetval > 0) {
		iSndTotalCount++;
		iSndTotal += iSndRetval;
	}
	else {
		iSndErrorCount++;
	}
	//��Ҫ��Ҫ��ӡ����
	cout << endl << "��ӿ� " << port << " �������ݣ�" << endl;
	switch (iWorkMode % 10) {
	case 1:
		print_data_bit(kbBuf, len, 1);
		break;
	case 2:
		print_data_byte(kbBuf, len, 1);
		break;
	case 0:
		break;
	}
	return iSndRetval;
}
//end=========��Ҫ�ľ���Щ��������Ҫ���ָĵġ�ֻ�С�TimeOut��RecvFromUpper��RecvFromLower=========================
