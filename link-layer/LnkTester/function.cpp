//Nettester 的功能文件
#include <iostream>
#include <conio.h>
#include "winsock.h"
#include <cstdio>
#include "CfgFileParms.h"
#include "function.h"
#include "lnklib.h"
#include <process.h>
using namespace std;

//以下为重要的变量
int printCount = 0;	//打印控制
int spin = 0;				//打印动态信息控制

//------华丽的分割线，一些统计用的全局变量------------
int iSndTotal = 0;						//发送数据总量
int iSndTotalCount = 0;			//发送数据总次数
int iSndErrorCount = 0;			//发送错误次数
int iRcvForward = 0;				//转发数据总量
int iRcvForwardCount = 0;		//转发数据总次数
int iRcvToUpper = 0;				//从低层递交高层数据总量
int iRcvToUpperCount = 0;		//从低层递交高层数据总次数
int iRcvUnknownCount = 0;	//收到不明来源数据总次数

//============重点来了==============================
//------------重要的控制参数-----------------------------------------------------
//以下从配置文件ne.txt中读取的重要参数
int lowerMode[10];		//如果低层是物理层模拟软件，这个数组放置的是每个接口对应的数据格式——0为比特数组，1为字节数组
int lowerNumber;			//低层实体数量，比如网络层之下可能有多个链路层
int iWorkMode;			//实体的工作模式，十位数：1 自动发送，0 手动发送；个位数 1 打印数据内容，0 打印统计数据
char strDevID[128];			//设备号，字符串形式，从1开始
char strLayer[128];				//层次名
char strEntity[128];				//实体号，字符串形式，从0开始，可以通过atoi函数变成整数在程序中使用

static void check_key() {
	while (1) {
		if (_kbhit()) {
			//键盘有动作，进入菜单模式
			menu();
		}
		Sleep(100);
	}
}
//***************重要函数提醒******************************
//名称：InitFunction
//功能：初始化功能面，由main函数在读完配置文件，正式进入驱动机制前调用
//输入：
//输出：
void InitFunction() {
	_beginthread((_beginthread_proc_type)check_key, 0, NULL);
}

//***************重要函数提醒******************************
//名称：EndFunction
//功能：结束功能面，由main函数在收到exit命令，整个程序退出前调用
//输入：
//输出：
//void EndFunction() {
//}

//***************重要函数提醒******************************
//名称：TimeOut
//功能：本函数被调用时，意味着sBasicTimer中设置的超时时间到了，
//      函数内容可以全部替换为设计者自己的想法
//      例程中实现了几个同时进行功能，供参考
//      1)根据iWorkMode工作模式，判断是否将键盘输入的数据发送，
//        因为scanf会阻塞，导致计时器在等待键盘的时候完全失效，所以使用_kbhit()无阻塞、不间断地在计时的控制下判断键盘状态
//      2)不断刷新打印各种统计值，通过打印控制符的控制，可以始终保持在同一行打印
//输入：时间到了就触发，只能通过全局变量供给输入
//输出：
void TimeOut() {
	printCount++;
	print_statistics();
}
//------------华丽的分割线，以下是数据的收发--------------------------------------------

//***************重要函数提醒******************************
//名称：RecvfromUpper
//功能：本函数被调用时，意味着收到一份高层下发的数据
//      函数内容全部可以替换成设计者自己的
//      例程功能介绍
//         1)通过低层的数据格式参数lowerMode，判断要不要将数据转换成bit流数组发送，发送只发给低层接口0，
//           因为没有任何可供参考的策略，讲道理是应该根据目的地址在多个接口中选择转发的。
//         2)判断iWorkMode，看看是不是需要将发送的数据内容都打印，调试时可以，正式运行时不建议将内容全部打印。
//输入：U8 * buf,高层传进来的数据， int len，数据长度，单位字节
//输出：
void RecvfromUpper(U8* buf, int len) {
	if (!is_waiting) {
		int iSndRetval = 0;
		U8* bufSend = NULL;
		//是高层数据，只从接口0发出去,高层接口默认都是字节流数据格式
		if (lowerMode[0] == 0) {
			//接口0的模式为bit数组，先转换成bit数组，放到bufSend里
			bufSend = (U8*)malloc(len * 8);
			if (bufSend == NULL) {
				return;
			}
			iSndRetval = ByteArrayToBitArray(bufSend, len * 8, buf, len);
			U8* dst = (U8*)"\1\1\1\1\1\1\1\1";		//模拟目的地址
			U8* src = (U8*)"\1\0\1\0\1\0\1\0";		//模拟源地址
			len *= 8;
			U8* packed_frame = pack_frame(bufSend, src, dst, &len, 1, 0);
			//发送
			iSndRetval = SendtoLower(packed_frame, iSndRetval + FRAME_INFO_LEN, 0); //参数依次为数据缓冲，长度，接口号
		}
		else {
			//下层是字节数组接口，可直接发送
			U8* packed_frame = pack_frame(bufSend, len, 0);
			iSndRetval = SendtoLower(buf, len + FRAME_INFO_LEN / 8, 0);
			iSndRetval = iSndRetval * 8;//换算成位
		}
		//如果考虑设计停等协议等重传协议，这份数据需要缓冲起来，应该另外申请空间，把buf或bufSend的内容保存起来，以备重传
		if (bufSend != NULL) {
			//保存bufSend内容
			save_send_buf(bufSend, lowerMode[0] ? len : (len * 8));
			//将空间释放
			free(bufSend);
		}
		else puts("处于等待状态，丢弃高层数据");
		//统计
		if (iSndRetval <= 0) {
			iSndErrorCount++;
		}
		else {
			iSndTotal += iSndRetval;
			iSndTotalCount++;
		}
	}
	//printf("\n收到上层数据 %d 位，发送到接口0\n", retval * 8);
	//打印
	switch (iWorkMode % 10) {
	case 1:
		cout << endl << "高层要求向接口 0 发送数据：" << endl;
		print_data_bit(buf, len, 1);
		break;
	case 2:
		cout << endl << "高层要求向接口 0 发送数据：" << endl;
		print_data_byte(buf, len, 1);
		break;
	default: break;
	}
}
//***************重要函数提醒******************************
//名称：RecvfromLower
//功能：本函数被调用时，意味着得到一份从低层实体递交上来的数据
//      函数内容全部可以替换成设计者想要的样子
//      例程功能介绍：
//          1)转发和上交前，判断收进来的格式和要发送出去的格式是否相同，否则，在bit流数组和字节流数组之间实现转换
//            注意这些判断并不是来自数据本身的特征，而是来自配置文件，所以配置文件的参数写错了，判断也就会失误
//          2)根据iWorkMode，判断是否需要把数据内容打印
//输入：U8 * buf,低层递交上来的数据， int len，数据长度，单位字节，int ifNo ，低层实体号码，用来区分是哪个低层
//输出：
void RecvfromLower(U8* buf, int len, int ifNo) {
	if (ifNo == 0) {
		check_recv(buf, len, !lowerMode[0]);
	}
	//打印
	cout << endl << "接收接口 " << ifNo << " 数据：" << endl;
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
		cout << "共转发 "<< iRcvForward<< " 位，"<< iRcvForwardCount<<" 次，"<<"递交 "<< iRcvToUpper<<" 位，"<< iRcvToUpperCount<<" 次,"<<"发送 "<< iSndTotal <<" 位，"<< iSndTotalCount<<" 次，"<< "发送不成功 "<< iSndErrorCount<<" 次,""收到不明来源 "<< iRcvUnknownCount<<" 次。";
		spin++;
	}
}

static U8 kbBuf[128];									//按键缓冲区
void menu() {
	int selection;
	unsigned short port;
	int len;
	//发送|打印：[发送控制（0，等待键盘输入；1，自动）][打印控制（0，仅定期打印统计信息；1，按bit流打印数据，2按字节流打印数据]
	cout << endl << endl << "设备号:" << strDevID << ",    层次:" << strLayer << ",    实体号:" << strEntity;
	cout << endl << "1-启动自动发送; " << endl << "2-停止自动发送; " << endl << "3-从键盘输入发送; ";
	cout << endl << "4-仅打印统计信息; " << endl << "5-按比特流打印数据内容;" << endl << "6-按字节流打印数据内容;";
	cout << endl << "0-取消" << endl << "请输入数字选择命令：";
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
		cout << "输入字符串(不超过128字符)：";
		cin >> kbBuf;
		cout << "输入低层接口号：";
		cin >> port;
		len = (int)strlen((const char*)kbBuf) + 1; //字符串最后有个结束符
		if (port >= lowerNumber) {
			cout << "没有这个接口" << endl;
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

//------------华丽的分割线，以下是一些数据处理的工具函数，可以用，没必要改------------------------------
//*************************************************
//名称：code
//功能：长整数x中的指定位数，放大到A[]这个比特数组中，建议按8的倍数做
//输入：x，被放大的整数，里面包含length长度的位数
//输出：A[],放大后的比特数组
void code(unsigned long x, U8 A[], int length) {
	unsigned long test;
	int i;
	//高位在前
	test = 1;
	test = test << (length - 1);
	for (i = 0; i < length; i++) {
		if (test & x) {
			A[i] = 1;
		}
		else {
			A[i] = 0;
		}
		test = test >> 1; //本算法利用了移位操作和"与"计算，逐位测出x的每一位是0还是1.
	}
}
//名称：decode
//功能：把比特数组A[]里的各位（元素），缩小放回到一个整数中，长度是length位，建议按8的倍数做
//输入：比特数组A[],需要变化的位长
//输出：缩小后，还原的整数
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
//名称：ByteArrayToBitArray
//功能：将字节数组流放大为比特数组流
//输入： int iBitLen——位流长度, U8* byteA——被放大字节数组, int iByteLen——字节数组长度
//输出：函数返回值是转出来有多少位；
//      U8* bitA,比特数组，注意比特数组的空间（声明）大小至少应是字节数组的8倍
int ByteArrayToBitArray(U8* bitA, int iBitLen, U8* byteA, int iByteLen) {
	int i;
	int len;

	len = min(iByteLen, iBitLen / 8);
	for (i = 0; i < len; i++) {
		//每次编码8位
		code(byteA[i], &(bitA[i * 8]), 8);
	}
	return len * 8;
}
//*************************************************
//名称：BitArrayToByteArray
//功能：将字节数组流放大为比特数组流
//输入：U8* bitA,比特数组，int iBitLen——位流长度,  int iByteLen——字节数组长度
//      注意比特数组的空间（声明）大小至少应是字节数组的8倍
//输出：返回值是转出来有多少个字节，如果位流长度不是8位整数倍，则最后1字节不满；
//      U8* byteA——缩小后的字节数组,，
int BitArrayToByteArray(U8* bitA, int iBitLen, U8* byteA, int iByteLen) {
	int i;
	int len;
	int retLen;

	len = min(iByteLen * 8, iBitLen);
	if (iBitLen > iByteLen * 8) {
		//截断转换
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
//名称：print_data_bit
//功能：按比特流形式打印数据缓冲区内容
//输入：U8* A——比特数组, int length——位数, int iMode——原始数据格式，0为比特流数组，1为字节数组
//输出：直接屏幕打印
void print_data_bit(U8* A, int length, int iMode) {
	int i, j;
	U8 B[8];
	int lineCount = 0;
	cout << endl << "数据的位流：" << endl;
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
//名称：print_data_byte
//功能：按字节流数组形式打印数据缓冲区内容,同时打印字符和十六进制数两种格式
//输入：U8* A——比特数组, int length——位数, int iMode——原始数据格式，0为比特流数组，1为字节数组
//输出：直接屏幕打印
void print_data_byte(U8* A, int length, int iMode) {
	int linecount = 0;
	int i;

	if (iMode == 0) {
		length = BitArrayToByteArray(A, length, A, length);
	}
	cout << endl << "数据的字符流及十六进制字节流:" << endl;
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
		//下层接口是比特流数组,需要一片新的缓冲来转换格式
		bufSend = (U8*)malloc(len * 8);

		iSndRetval = ByteArrayToBitArray(bufSend, len * 8, kbBuf, len);
		iSndRetval = SendtoLower(bufSend, iSndRetval, port);
		free(bufSend);
	}
	else {
		//下层接口是字节数组，直接发送
		iSndRetval = SendtoLower(kbBuf, len, port);
		iSndRetval = iSndRetval * 8; //换算成位
	}
	//发送统计
	if (iSndRetval > 0) {
		iSndTotalCount++;
		iSndTotal += iSndRetval;
	}
	else {
		iSndErrorCount++;
	}
	//看要不要打印数据
	cout << endl << "向接口 " << port << " 发送数据：" << endl;
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
//end=========重要的就这些，真正需要动手改的“只有”TimeOut，RecvFromUpper，RecvFromLower=========================
