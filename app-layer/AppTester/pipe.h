#pragma once
#include <iostream>
#include <windows.h>
using namespace std;

class Pipe // ������ֲ�ֻ࣬���� WindowsXP ���� ƽ̨
{
private:
    HANDLE hpiperead = NULL;     //���� �����ܵ�
    HANDLE hpipewrite = NULL;
    HANDLE hpiperead2 = NULL;
    HANDLE hpipewrite2 = NULL;    //д�� �����ܵ�
    HANDLE hProHandle = NULL;
    HANDLE hThrLoop = NULL;
    HANDLE hThrisLoop = NULL;
    SECURITY_ATTRIBUTES ai;     //��ȫ����
    PROCESS_INFORMATION pi;    //������Ϣ
    STARTUPINFOA si;
    BOOL pipe = false;
    INT status = 1;  // 0 = �쳣 1 = ���� -1 = ���� ��
    string errorString;
public:
    char* readPipe(DWORD* len);    //������
    void isloop();    //ѭ��
    const BOOL isPipeOff() const;    //�ܵ��Ƿ��ǿ���
    const INT getStatus() const;    //��ȡ��ǰ״��
    const string& getError() const;    //��ȡ��ǰ������Ϣ
    const BOOL sendCommand(const char*);    //ִ������
    void setPipeOn(const BOOL);    //���ùܵ��Ƿ���
    void setStatus(const INT, const char*);    //�������ô�����Ϣ
    void setStatus(const INT);    //���أ���������״̬
    Pipe(char* str);        //�ܵ�ִ�е�����
    ~Pipe();
};