// readfile.cpp : Defines the entry point for the console application.
//

#include <string>
#include <fstream>
#include <iostream>
#include <winsock.h>
#pragma comment(lib,"wsock32.lib")
//#include <iostream>
using namespace std;


class CCfgFileParms
{
public:


	bool isConfigExist; //false����ȡ����true����ȷ��������
	enum parms_set{BASIC,LOWER,UPPER,PEER};

	int getValueInt(int* dst, parms_set type, char* name, int index); //type �ĸ�����Ĳ�����name �������֣�index���ô�����ĸ�С�顪����Ϊÿ��С���ڵĲ�������ͬ����
	char* getValueStr(parms_set type, char* name, int index);//type �ĸ�����Ĳ�����name �������֣�index���ô�����ĸ�С�顪����Ϊÿ��С���ڵĲ�������ͬ����
	int getNumber(parms_set type); //ȡ�ò���������������繲���ŵ����ж���Զ˵�ַ�ȣ�������㣬�����ö���ھ�·������Ϣ��

	void print(); //��ӡ����������������
	int read();   //�����趨��devieceID��layer��entityID����ȡ�����ļ����������isConfigExist��־��Ϊtrue��ʾ��ȷ��������
	void setDeviceID(string ID)
	{
		deviceID = ID;
	}
	void setDeviceID(int ID)
	{
		char buf[40];
		_itoa_s(ID,buf,40,10);
		deviceID = buf;
	}
	void setLayer(string lay)
	{
		layer = lay;
	}

	void setEntityID(string ID)
	{
		entityID = ID;
	}
	void setEntityID(int ID)
	{
		char buf[40];
		_itoa_s(ID, buf, 40, 10);
		entityID = buf;
	}


	CCfgFileParms();
	CCfgFileParms(string devID,string layerID,string entID);

	~CCfgFileParms();
protected:
	string deviceID;
	string layer;
	string entityID;
	struct parmEntry {
		char* name;
		char* cValue;
		int value;
	};
	struct cfgParms {
		int number; //�ж�������������繲���ŵ��ϣ����ڶ���Եȵ������ʵ�壬
		int parms;  //ÿ������ж�������繲���ŵ��ϣ�ÿ���Ե�ʵ��Ĳ������������IP��ַ�Ͷ˿ں�
		parmEntry* entryArray;
	};
	struct cfgParms basic; //���������������в�������߲㡢�Ͳ�ͶԵ��йصĲ�����һ��ֻ��1��
	struct cfgParms lower; //�Ͳ�ʵ������飬һ���ǽӿڵ�ַ������ж���Ͳ�ʵ���Ӧ�������ж������
	struct cfgParms upper; //�߲�ʵ������飬һ���ǽӿڵ�ַ������ж���߲�ʵ���Ӧ�������ж������
	struct cfgParms peer;  //�Ե�ʵ������飬һ���ǶԵ�ʵ��ĵ�ַ����Ϣ������ж���ԵȲ�ʵ���Ӧ�������ж������

	void myStrcpy(char * str,string src );
	
	void printArray(parms_set type);
	int readArray(parms_set type, ifstream & cfgFile);

	void freeArray(parms_set type);
	void * getCfgParms(parms_set type);
	int getAndCheckLine(ifstream& cfgFile, string& str);

};



