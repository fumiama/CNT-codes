#include "pipe.h"

DWORD __stdcall WaitPipe(void* www)
{
    Pipe* pipe = (Pipe*)www;
    pipe->isloop();
    return 0;
}


Pipe::Pipe(char* com)
{
    ai.nLength = sizeof(SECURITY_ATTRIBUTES);
    ai.bInheritHandle = true;
    ai.lpSecurityDescriptor = NULL;
    this->setPipeOn(true);
    if (!CreatePipe(&hpiperead, &hpipewrite, &ai, 0))  //��������ܵ�
    {
        this->setStatus(-1, "[0x01]Read ������ʧЧ");
        return;
    }

    if (!CreatePipe(&hpiperead2, &hpipewrite2, &ai, 0))  //��������ܵ�
    {
        this->setStatus(-1, "[0x02]Write ������ʧЧ");
        return;
    }
    GetStartupInfoA(&si);    //��ȡ��ǰ���̵�STARTUPINFO
    si.cb = sizeof(STARTUPINFO);
    si.hStdError = hpipewrite;
    si.hStdOutput = hpipewrite;
    si.hStdInput = hpiperead2;
    si.wShowWindow = SW_SHOW;
    si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESTDHANDLES;

    if (!(CreateProcessA(NULL, com, NULL, NULL, true, NULL, NULL, NULL, &si, &pi)))      //�������ص�CMD����
    {
        this->setStatus(-1, "[0x03] CreateProcess����ִ�г���");
        return;
    }

    DWORD dwThread = NULL;
    hThrLoop = CreateThread(NULL, 0, WaitPipe, this, 0, &dwThread);//chuangjian
    if (!hThrLoop) {
        this->setStatus(-1, "[0x12] �̴߳���ʧ�� CreateThread ISLOOPʧ��");
        return;
    }
}

Pipe::~Pipe()
{
    //�����ں˶���ʹ����֮��һ���ǵùرգ��п��ܻ�����ڴ�й¶
    this->setPipeOn(false);
    this->setStatus(-1);
    CloseHandle(hThrisLoop);
    CloseHandle(hThrLoop);
    CloseHandle(hpipewrite);
    CloseHandle(hpiperead);
    CloseHandle(hpiperead2);
    CloseHandle(hpipewrite2);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}
const INT Pipe::getStatus() const
{
    return this->status;
}

const string& Pipe::getError() const
{
    return this->errorString;
}

const BOOL Pipe::isPipeOff() const
{
    return pipe;
}

void Pipe::setPipeOn(const BOOL bools)
{
    this->pipe = bools;
}

void Pipe::setStatus(const INT status, const char* info)
{
    this->errorString = info;    //��˵�᲻���и��õĸ�ֵ����?
    this->status = status;
}

void Pipe::setStatus(const INT status = 1)
{
    this->status = status;
}

const BOOL Pipe::sendCommand(const char* com)     //ִ������
{
    DWORD dwWrite = 0;
    char www[1024];
    strcpy_s(www, com);
    strcat_s(www, "\n");
    printf("��������: %s", www);
    return WriteFile(hpipewrite2, www, strlen(www), &dwWrite, NULL);
}

char* Pipe::readPipe(DWORD* len) {
    char* outbuff = (char*)malloc(4096);        //�������
    if (ReadFile(this->hpiperead, outbuff, 4095, len, NULL) == NULL)
    {
        this->setPipeOn(false);
        return NULL;
    }
    else if (*len) {
        outbuff[*len] = 0;
        cout << "���յ�����: " << outbuff << endl;
        return outbuff;
    }
    else return NULL;
}

void Pipe::isloop()
{
    DWORD dwRet = WaitForSingleObject(pi.hProcess, INFINITE);
    while (dwRet == WAIT_TIMEOUT)
    {
        dwRet = WaitForSingleObject(pi.hProcess, INFINITE);
    }

    if (dwRet == WAIT_OBJECT_0 || dwRet == WAIT_ABANDONED)
    {
        this->setPipeOn(false);
        std::cout << "[END] Pipe Stoped!" << endl;
    }
}