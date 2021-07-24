#include "ReadGisConfig.h"
#include <QSettings>

struct GisConfigData
{
	GisConfigData()
		: maxLevel(22), minLevel(3), initLevel(5),
		initLongitude(0.0), initLatitude(0.0)
	{

	}

	//��Ƭ·��
	std::string tilePath;

	//ͼ��Ŀ¼
	std::string iconDir;

	//��ͼ���ȼ�
	unsigned maxLevel;

	//��ͼ��С�ȼ�
	unsigned minLevel;

	/*** ��ͼ��ʼ�� ***/
	//��ʼ���ȼ�
	unsigned initLevel;
	//��ʼ����γ��
	double initLongitude;
	double initLatitude;
};

ReadGisConfig::ReadGisConfig()
	: mPtr(nullptr)
{
	//��ȡ�����ļ�
	readConfig("config.ini");
}

ReadGisConfig::~ReadGisConfig()
{
	delete mPtr;
	mPtr = nullptr;
}

//��ȡ�����ļ�
void ReadGisConfig::readConfig(const std::string &confPath)
{
	if (!mPtr)
	{
		mPtr = new GisConfigData;
	}

	QSettings settings(confPath.c_str(), QSettings::IniFormat);
	
	//��Ƭ·��
	mPtr->tilePath = settings.value("GisConfig/TilePath").toString().toStdString();

	//ͼ��·��
	mPtr->iconDir = settings.value("GisConfig/EntityIconDir").toString().toStdString();

	//bool ok = false;
	//��ͼ���ȼ�
	mPtr->maxLevel = settings.value("GisConfig/MaxLevel", 17).toUInt();
	if (mPtr->maxLevel > 22)
		mPtr->maxLevel = 22;
	
	//��ͼ��С�ȼ�
	mPtr->minLevel = settings.value("GisConfig/MinLevel", 0).toUInt();
	if (mPtr->maxLevel < 0)
		mPtr->maxLevel = 0;

	//��ͼ��ʼ���ȼ�
	mPtr->initLevel = settings.value("GisConfig/InitLevel", 5).toUInt();

	//��ʼ����γ�� InitLongitude	 InitLatitude
	mPtr->initLongitude = settings.value("GisConfig/InitLongitude", 70).toUInt();

	mPtr->initLatitude = settings.value("GisConfig/InitLatitude", 50).toUInt();

}

//��ȡ���ȼ�
unsigned ReadGisConfig::getGisMaxLevel()
{
	if (!mPtr)
		return 17;

	return mPtr->maxLevel;
}

//��ȡ��С�ȼ�
unsigned ReadGisConfig::getGisMinLevel()
{
	if (!mPtr)
		return 3;

	return mPtr->minLevel;
}

//��ȡ��Ƭ·��
std::string nullStr;
const std::string & ReadGisConfig::getTilePath()
{
	if (!mPtr)
		return nullStr;

	return mPtr->tilePath;
}

//��ȡʵ��ͼ��Ŀ¼
const std::string & ReadGisConfig::getIconDir()
{
	if (!mPtr)
		return nullStr;

	return mPtr->iconDir;
}

int ReadGisConfig::getInitLevel()
{
	if (!mPtr)
		return 4;

	return  mPtr->initLevel;
}

//��ȡ��ͼ��ʼ��λ��
bool ReadGisConfig::getGisInitPos(double & initLon, double & initLat)
{
	if (!mPtr)
		return false;

	//��ʼ������
	initLon = mPtr->initLongitude;
	//��ʼ��γ��
	initLat = mPtr->initLatitude;

	return true;
}
