//LnkTester.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//
#include <iostream>
#include <conio.h>
#include <winsock.h>
#include <cstdio>
#include "CfgFileParms.h"
#include "function.h"
#include "mac.h"
#pragma comment (lib,"wsock32.lib")

//以下是一些重要的与通信有关的控制参数，但是也可以不用管
SOCKET sock;
struct sockaddr_in local_addr;				//本层实体地址
struct sockaddr_in upper_addr;			//上层实体地址，一般情况下，上层实体只有1个
struct sockaddr_in lower_addr[10];		//最多10个下层对象，数组下标就是下层实体的编号
sockaddr_in cmd_addr;						//统一管理平台地址

//------------华丽的分割线，以下是定时器--------------------------------------------
//基于select的定时器，目的是把数据的收发和定时都统一到一个事件驱动框架下
//可以有多个定时器，本设计实现了一个基准定时器，为周期性10ms定时，也可以当作是一种心跳计时器
//其余的定时器可以在这个基础上完成，可行的方案存在多种
//看懂设计思路后，自行扩展以满足需要
//基准定时器一开启就会立即触发一次
struct threadTimer_t {
	int iType;  //为0表示周期性定时器，定时达到后，会自动启动下一次定时
	ULONG ulInterval;
	LARGE_INTEGER llStopTime;
}sBasicTimer;  //全局唯一的计时器，默认是每10毫秒触发一次，设计者可以在这个计时器的基础上实现自己的各种定时。
               //比如计数100次以后，就是1秒。针对不同的事件设置两个变量，一个用来表示是否开始计时，一个用来表示计数到多大
//*************************************************
//名称：StartTimerOnce
//功能：把全局计时器改为1次性，本函调用的同时计时也开始，当定时到达时TimeOut函数将被调用
//输入：计时间隔时间，单位微秒，计时的起始就是本函数调用时。
//输出：直接改变全局计时器变量 sBasicTimer的内容
void StartTimerOnce(unsigned long ulInterval) {
	LARGE_INTEGER llFreq;

	sBasicTimer.iType = 1;
	sBasicTimer.ulInterval = ulInterval;
	QueryPerformanceFrequency(&llFreq);
	QueryPerformanceCounter(&sBasicTimer.llStopTime);
	sBasicTimer.llStopTime.QuadPart += llFreq.QuadPart * sBasicTimer.ulInterval / 1000000;
}
//*************************************************
//名称：StartTimerPeriodically
//功能：重设全局计时器的周期性触发的间隔时间，同时周期性计时器也开始工作，每次计时到达TimeOut函数被调用1次
//      周期性计时器可以将间隔设短一些，这样就可以在它的基础上实现更多的各种定时时间的定时器了
//      例程默认是启动周期性计时器机制
//输入：计时间隔时间，单位微秒，计时的起始就是本函数调用时。
//输出：直接改变全局计时器变量 sBasicTimer内容
void StartTimerPeriodically(unsigned long ulInterval) {
	LARGE_INTEGER llFreq;

	sBasicTimer.iType = 0;
	sBasicTimer.ulInterval = ulInterval;
	QueryPerformanceFrequency(&llFreq);
	QueryPerformanceCounter(&sBasicTimer.llStopTime);
	sBasicTimer.llStopTime.QuadPart += llFreq.QuadPart * sBasicTimer.ulInterval / 1000000;
}
//***************重要函数提醒******************************
//名称：SendtoUpper
//功能：向高层实体递交数据时，使用这个函数
//输入：U8 * buf,准备递交的数据， int len，数据长度，单位字节，int ifNo
//输出：函数返回值是发送的数据量
int SendtoUpper(U8* buf, int len) {
	int sendlen;
	sendlen = sendto(sock, (char*)buf, len, 0, (sockaddr*) & (upper_addr), sizeof(sockaddr_in));
	return sendlen;
}
//***************重要函数提醒******************************
//名称：SendtoLower
//功能：向低层实体下发数据时，使用这个函数
//输入：U8 * buf,准备下发的数据， int len，数据长度，单位字节,int ifNo,发往的低层接口号
//输出：函数返回值是发送的数据量
int SendtoLower(U8* buf, int len,int ifNo) {
	int sendlen;
	if (ifNo < 0 || ifNo >= lowerNumber) {
		printf("非法端口%d\n", ifNo);
		return 0;
	}
	printf("从端口%d向低层发送%d位\n", ifNo, len);
	sendlen = sendto(sock, (char*)buf, len, 0, (sockaddr*) & (lower_addr[ifNo]), sizeof(sockaddr_in));
	return sendlen;
}
//***************重要函数提醒******************************
//名称：SendtoCommander
//功能：向统一管理平台发送状态数据时，使用这个函数
//输入：U8 * buf,准备下发的数据， int len，数据长度，单位字节
//输出：函数返回值是发送的数据量
int SendtoCommander(U8* buf, int len) {
	int sendlen;
	sendlen = sendto(sock, (char*)buf, len, 0, (sockaddr*)&(cmd_addr), sizeof(sockaddr_in));
	return sendlen;
}

//------------华丽的分割线，以下到main以前，都不用管了----------------------------
void initTimer(int interval) {
	sBasicTimer.iType = 0;
	sBasicTimer.ulInterval = interval;//10ms,单位是微秒，10ms相对误差较小，但是也挺耗费CPU
	QueryPerformanceCounter(&sBasicTimer.llStopTime);
}
//根据系统当前时间设置select函数要用的超时时间——to，每次在select前使用
void setSelectTimeOut(timeval* to, struct threadTimer_t* sT) {
	LARGE_INTEGER llCurrentTime;
	LARGE_INTEGER llFreq;
	LONGLONG next;
	//取系统当前时间
	QueryPerformanceFrequency(&llFreq);
	QueryPerformanceCounter(&llCurrentTime);
	if (llCurrentTime.QuadPart >= sT->llStopTime.QuadPart) {
		to->tv_sec = 0;
		to->tv_usec = 0;
		//		sT->llStopTime.QuadPart += llFreq.QuadPart * sT->ulInterval / 1000000;
	}
	else {
		next = sT->llStopTime.QuadPart - llCurrentTime.QuadPart;
		next = next * 1000000 / llFreq.QuadPart;
		to->tv_sec = (long)(next / 1000000);
		to->tv_usec = long(next % 1000000);
	}

}
//根据系统当前时间判断定时器sT是否超时，可每次在select后使用，返回值true表示超时，false表示没有超时
bool isTimeOut(struct threadTimer_t* sT) {
	LARGE_INTEGER llCurrentTime;
	LARGE_INTEGER llFreq;
	//取系统当前时间
	QueryPerformanceFrequency(&llFreq);
	QueryPerformanceCounter(&llCurrentTime);

	if (llCurrentTime.QuadPart >= sT->llStopTime.QuadPart) {
		if (sT->iType == 0) {
			//定时器是周期性的，重置定时器
			sT->llStopTime.QuadPart += llFreq.QuadPart * sT->ulInterval / 1000000;
		}
		return true;
	}
	else {
		return false;
	}
}

void SetColor(int ForgC) {
	WORD wColor;
	//We will need this handle to get the current background attribute
	HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
	CONSOLE_SCREEN_BUFFER_INFO csbi;

	//We use csbi for the wAttributes word.
	if (GetConsoleScreenBufferInfo(hStdOut, &csbi))
	{
		//Mask out all but the background attribute, and add in the forgournd color
		wColor = (csbi.wAttributes & 0xF0) + (ForgC & 0x0F);
		SetConsoleTextAttribute(hStdOut, wColor);
	}
}
//------------华丽的分割线，main来了-----------------
int main(int argc, char* argv[]) {
	U8 * buf;          //存放从高层、低层、各方面来的数据的缓存，大小为MAX_BUFFER_SIZE
	int len;           //buf里有效数据的大小，单位是字节
	int iRecvIntfNo;
	struct sockaddr_in remote_addr;
	WSAData wsa;
	int retval;
	int tmpInt;
	fd_set readfds;
	timeval timeout;
	unsigned long arg;
	string s1, s2, s3;
	int i;
	string strTmp;

	buf = (U8*)malloc(MAX_BUFFER_SIZE);
	if (buf == NULL) {
		cout << "内存不够" << endl;
		return 0;
	}
	CCfgFileParms cfgParms;

	if (argc == 4) {
		s1 = argv[1];
		s2 = argv[2];
		s3 = argv[3];
	}
	else if (argc == 3) {
		s1 = argv[1];
		s2 = "LNK";
		s3 = argv[2];
	}
	else {
		//从键盘读取
		cout << "请输入设备号：";
		cin >> s1;
		cout << "请输入层次名（大写）：";
		cin >> s2;
		//s2 = "NET";
		cout << "请输入实体号：";
		cin >> s3;
	}
	WSAStartup(0x101, &wsa);
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock == SOCKET_ERROR)
		return 0;

	cfgParms.setDeviceID(s1);
	cfgParms.setLayer(s2);
	cfgParms.setEntityID(s3);
	cfgParms.read();
	if (cfgParms.getLayer().compare("LNK") == 0) {
		SetColor(7);
	}
	else {
		SetColor(6);
	}

	cfgParms.print(); //打印出来看看是不是读出来了
	cfgParms.getDeviceID().copy(strDevID, 128);
	cfgParms.getLayer().copy(strLayer, 128);
	cfgParms.getEntity().copy(strEntity, 128);

	if (!cfgParms.isConfigExist) {
		//从键盘输入上、下层本层的地址和端口号等等
		//偷个懒，要求必须填好配置文件
		return 0;
	}
	
	//取本层实体参数，并设置
	local_addr = cfgParms.getUDPAddr(CCfgFileParms::LOCAL,0);
	local_addr.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	set_local_addr(cfgParms.getLocalAddr());
	if (bind(sock, (sockaddr*)& local_addr, sizeof(sockaddr_in)) != 0) {
		printf("参数错误\n");
		return 0;

	}
	//获取工作参数
	retval = cfgParms.getValueInt(iWorkMode, (char*)"workMode");
	if (retval == -1) {
		iWorkMode = 0;
	}

	//读上层实体参数
	upper_addr = cfgParms.getUDPAddr(CCfgFileParms::UPPER, 0);

	//取下层实体参数，并设置
	//先取数量
	lowerNumber = cfgParms.getUDPAddrNumber(CCfgFileParms::LOWER);
	if (0 > lowerNumber) {
		printf("参数错误\n");
		return 0;
	}
	//逐个读取
	for (i = 0; i < lowerNumber; i++) {
		lower_addr[i] = cfgParms.getUDPAddr(CCfgFileParms::LOWER, i);

		//低层接口是Byte或者是bit,默认是bit流
		strTmp = "lowerMode";
		strTmp += std::to_string(i);
		retval = cfgParms.getValueInt(lowerMode[i],(char*)"lowerMode");
		if (0 > retval) {
			lowerMode[i] = 0; //默认都是bit流
		}
	}

	cmd_addr = cfgParms.getUDPAddr(CCfgFileParms::CMDER, 0);

	//心跳定时器
	retval = cfgParms.getValueInt(tmpInt, "heartBeatingTime");
	if (retval == 0)
		tmpInt = tmpInt * 1000;
	else {
		tmpInt = DEFAULT_TIMER_INTERVAL * 1000;
	}
	initTimer(tmpInt);

	//设置套接字为非阻塞态
	arg = 1;
	ioctlsocket(sock, FIONBIO, &arg);

	InitFunction();

	while (1) {
		FD_ZERO(&readfds);
		//采用了基于select机制，不断发送测试数据，和接收测试数据，也可以采用多线程，一线专发送，一线专接收的方案
		//设定超时时间
		if (sock > 0) {
			FD_SET(sock, &readfds);
		}
		setSelectTimeOut(&timeout, &sBasicTimer);
		retval = select(0, &readfds, NULL, NULL, &timeout);
		if (true == isTimeOut(&sBasicTimer)) {

			TimeOut();

			continue;
		}
		if (!FD_ISSET(sock, &readfds)) {
			continue;
		}
		len = sizeof(sockaddr_in);
		retval = recvfrom(sock, (char*)buf, MAX_BUFFER_SIZE, 0,(sockaddr*)&remote_addr,&len); //超过这个大小就不能愉快地玩耍了，因为缓冲不够大
		if (retval == 0) {
			closesocket(sock);
			sock = 0;
			printf("close a socket\n");
			continue;
		}
		else if (retval == -1) {
			retval = WSAGetLastError();
			if (retval == WSAEWOULDBLOCK || retval == WSAECONNRESET)
				continue;
			closesocket(sock);
			sock = 0;
			printf("close a socket\n");
			continue;
		}
		//收到数据后,通过源头判断是上层、下层、还是统一管理平台的命令
		if (remote_addr.sin_port == upper_addr.sin_port) {
			//IP地址也应该比对的，偷个懒
			RecvfromUpper(buf, retval);
		}
		else {
			for (iRecvIntfNo = 0; iRecvIntfNo < lowerNumber; iRecvIntfNo++) {
				//下层收到的数据,检查是哪个接口的
				if (remote_addr.sin_port == lower_addr[iRecvIntfNo].sin_port) {
					RecvfromLower(buf, retval, iRecvIntfNo);
					break;
				}
			}
			if (iRecvIntfNo >= lowerNumber) {
				//检查是不是控制口命令
				if (remote_addr.sin_port == cmd_addr.sin_port) {
					if (strncmp((char*)buf, "exit", 5) == 0) {
						//收到退出命令
						goto ret;
					}
				}
			}
		}
	}
ret:
	//EndFunction();
	free(buf);
	if (sock > 0)
		closesocket(sock);
	WSACleanup();
	return 0;
}


// 运行程序: Ctrl + F5 或调试 >“开始执行(不调试)”菜单
// 调试程序: F5 或调试 >“开始调试”菜单

// 入门使用技巧: 
//   1. 使用解决方案资源管理器窗口添加/管理文件
//   2. 使用团队资源管理器窗口连接到源代码管理
//   3. 使用输出窗口查看生成输出和其他消息
//   4. 使用错误列表窗口查看错误
//   5. 转到“项目”>“添加新项”以创建新的代码文件，或转到“项目”>“添加现有项”以将现有代码文件添加到项目
//   6. 将来，若要再次打开此项目，请转到“文件”>“打开”>“项目”并选择 .sln 文件
