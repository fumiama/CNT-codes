// readfile.cpp : Defines the entry point for the console application.
//
//#include "stdafx.h"
#include <string>
#include <fstream>
#include <iostream>
#include "cfgFileParms.h"
//#include <iostream>
using namespace std;

CCfgFileParms::CCfgFileParms()
{
	//��ʼ��
	basic.parms = basic.number = 0;
	basic.entryArray = NULL;
	lower.parms = lower.number = 0;
	lower.entryArray = NULL;
	upper.number = upper.parms = 0;
	upper.entryArray = NULL;
	peer.number = peer.parms = 0;
	peer.entryArray = NULL;

	isConfigExist = false;

}
CCfgFileParms::CCfgFileParms(string devID, string layerID, string entID)
{
	//��ʼ��
	basic.parms = basic.number = 0;
	basic.entryArray = NULL;
	lower.parms = lower.number = 0;
	lower.entryArray = NULL;
	upper.number = upper.parms = 0;
	upper.entryArray = NULL;
	peer.number = peer.parms = 0;
	peer.entryArray = NULL;

	isConfigExist = false;

	deviceID = devID;
	layer = layerID;
	entityID = entID;
	read();
}

void CCfgFileParms::myStrcpy(char* str, string src) //ֻ����ASCII�����32���ַ���32Ϊ�ո����µĶ��ǿ����ַ�
{
	size_t i, j;
	j = 0;
	for (i = 0; i < strlen(src.c_str()); i++) {
		if (src[i] > 32 || src[i] < 0) {
			//�����ַ�ҲҪ����
			str[j] = src.c_str()[i];
			j++;
		}
	}
	str[j] = 0;
}

void* CCfgFileParms::getCfgParms(parms_set type)
{
	// TODO: �ڴ˴����ʵ�ִ���.
	struct cfgParms* cfg;
	// TODO: �ڴ˴����ʵ�ִ���.
	switch (type) {
	case parms_set::BASIC:
		cfg = &(this->basic);
		break;
	case parms_set::LOWER:
		cfg = &lower;
		break;
	case parms_set::UPPER:
		cfg = &upper;
		break;
	case parms_set::PEER:
		cfg = &peer;
		break;
	default:
		return NULL;
	}
	return cfg;
}
int CCfgFileParms::getValueInt(int * dst,parms_set type, char* name, int index)
{
	int i;
	string strTmp;
	size_t retval;
	int find;
	struct cfgParms* cfg;

	*dst = 0;
	cfg = (struct cfgParms*)getCfgParms(type);
	if (cfg == NULL) {
		return -1;
	}
	for (i = index * cfg->parms; i < cfg->number * cfg->parms; i++) {
		strTmp = cfg->entryArray[i].name;
		retval = strlen(strTmp.c_str());
		if (retval < strlen(name))
			continue;
		find = (int)strTmp.find(name);
		if (find >= 0) {
			*dst = cfg->entryArray[i].value;
			return 0;
		}
	}
	return -1;
}
char* CCfgFileParms::getValueStr(parms_set type, char* name, int index)
{
	int i;
	string strTmp;
	size_t retval;
	int find;
	struct cfgParms* cfg;

	cfg = (struct cfgParms*)getCfgParms(type);
	if (cfg == NULL) {
		return NULL;
	}

	for (i = index * cfg->parms; i < cfg->number * cfg->parms; i++) {
		strTmp = cfg->entryArray[i].name;
		retval = strlen(strTmp.c_str());
		if (retval < strlen(name))
			continue;
		find = (int)strTmp.find(name);
		if (find >= 0) {
			return cfg->entryArray[i].cValue;
		}
	}
	return NULL;
}
int CCfgFileParms::getNumber(parms_set type) //ȡ�ò���������������繲���ŵ����ж���Զ˵�ַ��
{
	struct cfgParms* cfg;

	cfg = (struct cfgParms*)getCfgParms(type);
	if (cfg == NULL) {
		return 0;
	}
	return cfg->number;

}

//�����ӡ
void CCfgFileParms::printArray(parms_set type)
{
	struct cfgParms* cfg;
	int i, j;
	// TODO: �ڴ˴����ʵ�ִ���.
	cfg = (struct cfgParms*)getCfgParms(type);
	if (cfg == NULL) {
		return;
	}
	for (i = 0; i < cfg->number; i++) {
		cout << "  ��" << i << "��" << endl;
		for (j = 0; j < cfg->parms; j++) {
			cout << cfg->entryArray[i * cfg->parms + j].name << " = " << cfg->entryArray[i * cfg->parms + j].cValue << endl;
		}
	}
	return;
}

void CCfgFileParms::print()
{
	cout << "���������" << endl;
	printArray(parms_set::BASIC);

	cout << "�²������" << endl;
	printArray(parms_set::LOWER);

	cout << "�ϲ������" << endl;
	printArray(parms_set::UPPER);

	cout << "�Ե�ʵ�������" << endl;
	printArray(parms_set::PEER);
}
int CCfgFileParms::getAndCheckLine(ifstream& cfgFile, string& str)
{
	int x;
	getline(cfgFile, str);
	if (cfgFile.eof())
		return -1;
	x = (int)str.find("--------");
	if (x >= 0)
		return -2;
	return 0;
}
int CCfgFileParms::readArray(parms_set type, ifstream& cfgFile)
{
	struct cfgParms* cfg;
	string strTmp;
	string strInt;
	int number, parms, i, j, pos;
	int retval;
	string strNumber = "Number";
	string strParms = "Parms";

	// TODO: �ڴ˴����ʵ�ִ���.
	cfg = (struct cfgParms*)getCfgParms(type);
	if (cfg == NULL) {
		return -1;
	}
	// TODO: �ڴ˴����ʵ�ִ���.

	if (0 > getAndCheckLine(cfgFile, strTmp)) {
		return -1;
	}
	//����Ӧ���飬����û����Ҫ����Ĳ�����number=0��parms=0���б�����
	retval = (int)strTmp.find(strNumber);
	if (retval == -1) {
		return -1;
	}

	strInt = strTmp.substr(strTmp.find("=") + 1, strTmp.length() - strTmp.find("="));
	//�ж��������
	number = atoi(strInt.c_str());
	cfg->number = number;

	if (0 > getAndCheckLine(cfgFile, strTmp)) {
		return -1;
	}

	retval = (int)strTmp.find(strParms);
	if (retval == -1) {
		return -1;
	}
	pos = (int)strTmp.find("=");
	strInt = strTmp.substr(pos + 1, strTmp.length() - pos);
	//ÿ���ж��ٸ�����
	parms = atoi(strInt.c_str());
	cfg->parms = parms;
	cfg->entryArray = (struct parmEntry*)malloc(sizeof(struct parmEntry) * number * parms);
	//length = sizeof(struct parmEntry) * parms;
	for (i = 0; i < number; i++) {
		for (j = 0; j < parms; j++) {
			if (0 > getAndCheckLine(cfgFile, strTmp)) {
				return -1;
			}
			pos = (int)strTmp.find("=");
			cfg->entryArray[i * parms + j].name = (char*)malloc(pos + 1);
			myStrcpy(cfg->entryArray[i * parms + j].name, strTmp.substr(0, pos).c_str());
			cfg->entryArray[i * parms + j].cValue = (char*)malloc(strTmp.length() - pos + 1);
			myStrcpy(cfg->entryArray[i * parms + j].cValue, strTmp.substr(pos + 1, strTmp.length() - pos).c_str());
			cfg->entryArray[i * parms + j].value = atoi(cfg->entryArray[i * parms + j].cValue);
		}

	}
	return 0;
}

int CCfgFileParms::read()
{
	string strTmp;
	string strInt;
	int deviceFind = 0;
	int layerFind = 0;
	int entityFind = 0;
	int retval;

	ifstream cfgFile("ne.txt");

	if (!cfgFile.is_open())
	{
		isConfigExist = false;
		return 0;
	}
	//ͨ���豸�ţ����������ʵ��ţ��õ��ĸ�������:basic, lower , upper��peer
	deviceFind = 0;
	while ((!cfgFile.eof()) && (!deviceFind)) {
		getline(cfgFile, strTmp);
		//cfgFile >> strTmp;
		retval = (int)strTmp.find("deviceID");
		if (retval == -1) {
			continue;
		}
		retval = (int)strTmp.find(deviceID);
		if (retval == -1) {
			continue;
		}
		//�ҵ��豸
		deviceFind = 1;
		break;
	}
	if (cfgFile.eof() || deviceFind == 0) {
		cfgFile.close();
		return -1;
	}
	layerFind = 0;
	while ((!cfgFile.eof()) && (!layerFind)) {
		getline(cfgFile, strTmp);
		retval = (int)strTmp.find("deviceID");
		if (retval >= 0) {//���ҵ���һ���豸ȥ�ˣ�ֹͣ
			break;
		}
		retval = (int)strTmp.find("layer");
		if (retval == -1) {
			//getline(cfgFile,strTmp);
			continue;
		}
		retval = (int)strTmp.find(layer);
		if (retval == -1) {
			//getline(cfgFile,strTmp);
			continue;
		}
		layerFind = 1;
		break;
	}
	if (cfgFile.eof() || layerFind == 0) {
		cfgFile.close();
		return -1;
	}
	entityFind = 0;
	while ((!cfgFile.eof()) && (!entityFind)) {
		getline(cfgFile, strTmp);
		retval = (int)strTmp.find("layer");
		if (retval >= 0) {//���ҵ���һ�㣬ֹͣ
			break;
		}
		retval = (int)strTmp.find("deviceID");
		if (retval >= 0) {//���ҵ���һ���豸�ˣ�ֹͣ
			break;
		}
		retval = (int)strTmp.find("entityID");
		if (retval == -1) {
			//getline(cfgFile,strTmp);
			continue;
		}
		retval = (int)strTmp.find(entityID);
		if (retval == -1) {
			//getline(cfgFile,strTmp);
			continue;
		}

		//�ҵ�ʵ��
		entityFind = 1;
		break;
	}

	if (cfgFile.eof() || entityFind == 0) {
		cfgFile.close();
		return -1;
	}
	//����basic������
	retval = readArray(parms_set::BASIC, cfgFile);
	if (retval == -1) {
		cfgFile.close();
		return -1;
	}

	//��lowerparm�飬����û����Ҫ����Ĳ�����number=0��parms=0���б�����
	retval = readArray(parms_set::LOWER, cfgFile);
	if (retval == -1) {
		cfgFile.close();
		return -1;
	}

	//��upperparm��
	retval = readArray(parms_set::UPPER, cfgFile);
	if (retval == -1) {
		cfgFile.close();
		return -1;
	}
	//��peerparm��
	retval = readArray(parms_set::PEER, cfgFile);
	if (retval == -1) {
		cfgFile.close();
		return -1;
	}

	cfgFile.close();

	if (deviceFind == 1 && layerFind == 1 && entityFind == 1)
		isConfigExist = true;

	return 0;
}

void CCfgFileParms::freeArray(parms_set type)
{
	int i;
	struct cfgParms* cfg;
	// TODO: �ڴ˴����ʵ�ִ���.
	cfg = (struct cfgParms*)getCfgParms(type);
	if (cfg == NULL) {
		return;
	}

	for (i = 0; i < cfg->number * cfg->parms; i++) {
		free(cfg->entryArray[i].name);
		free(cfg->entryArray[i].cValue);
	}
	cfg->number = 0;
	cfg->parms = 0;
	if (cfg->entryArray != NULL) {
		free(cfg->entryArray);
		cfg->entryArray = NULL;
	}
}
CCfgFileParms::~CCfgFileParms()
{

	//basic��
	freeArray(BASIC);
	//lower��
	freeArray(LOWER);
	//upper��
	freeArray(UPPER);
	//upper��
	freeArray(PEER);
}
