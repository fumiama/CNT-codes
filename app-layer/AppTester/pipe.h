#pragma once
#include <iostream>
#include <windows.h>
using namespace std;

class Pipe // 不可移植类，只限于 WindowsXP 以上 平台
{
private:
    HANDLE hpiperead = NULL;     //读入 匿名管道
    HANDLE hpipewrite = NULL;
    HANDLE hpiperead2 = NULL;
    HANDLE hpipewrite2 = NULL;    //写出 匿名管道
    HANDLE hProHandle = NULL;
    HANDLE hThrLoop = NULL;
    HANDLE hThrisLoop = NULL;
    SECURITY_ATTRIBUTES ai;     //安全属性
    PROCESS_INFORMATION pi;    //进程信息
    STARTUPINFOA si;
    BOOL pipe = false;
    INT status = 1;  // 0 = 异常 1 = 正常 -1 = 错误 、
    string errorString;
public:
    char* readPipe(DWORD* len);    //读数据
    void isloop();    //循环
    const BOOL isPipeOff() const;    //管道是否是开启
    const INT getStatus() const;    //获取当前状况
    const string& getError() const;    //获取当前错误信息
    const BOOL sendCommand(const char*);    //执行命令
    void setPipeOn(const BOOL);    //设置管道是否开启
    void setStatus(const INT, const char*);    //用于设置错误信息
    void setStatus(const INT);    //重载，用于设置状态
    Pipe(char* str);        //管道执行的命令
    ~Pipe();
};