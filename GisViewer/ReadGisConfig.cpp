#include "ReadGisConfig.h"
#include <QSettings>

struct GisConfigData
{
	GisConfigData()
		: maxLevel(22), minLevel(3), initLevel(5),
		initLongitude(0.0), initLatitude(0.0)
	{

	}

	//瓦片路径
	std::string tilePath;

	//图标目录
	std::string iconDir;

	//地图最大等级
	unsigned maxLevel;

	//地图最小等级
	unsigned minLevel;

	/*** 地图初始化 ***/
	//初始化等级
	unsigned initLevel;
	//初始化经纬度
	double initLongitude;
	double initLatitude;
};

ReadGisConfig::ReadGisConfig()
	: mPtr(nullptr)
{
	//读取配置文件
	readConfig("config.ini");
}

ReadGisConfig::~ReadGisConfig()
{
	delete mPtr;
	mPtr = nullptr;
}

//读取配置文件
void ReadGisConfig::readConfig(const std::string &confPath)
{
	if (!mPtr)
	{
		mPtr = new GisConfigData;
	}

	QSettings settings(confPath.c_str(), QSettings::IniFormat);
	
	//瓦片路径
	mPtr->tilePath = settings.value("GisConfig/TilePath").toString().toStdString();

	//图标路径
	mPtr->iconDir = settings.value("GisConfig/EntityIconDir").toString().toStdString();

	//bool ok = false;
	//地图最大等级
	mPtr->maxLevel = settings.value("GisConfig/MaxLevel", 17).toUInt();
	if (mPtr->maxLevel > 22)
		mPtr->maxLevel = 22;
	
	//地图最小等级
	mPtr->minLevel = settings.value("GisConfig/MinLevel", 0).toUInt();
	if (mPtr->maxLevel < 0)
		mPtr->maxLevel = 0;

	//地图初始化等级
	mPtr->initLevel = settings.value("GisConfig/InitLevel", 5).toUInt();

	//初始化经纬度 InitLongitude	 InitLatitude
	mPtr->initLongitude = settings.value("GisConfig/InitLongitude", 70).toUInt();

	mPtr->initLatitude = settings.value("GisConfig/InitLatitude", 50).toUInt();

}

//获取最大等级
unsigned ReadGisConfig::getGisMaxLevel()
{
	if (!mPtr)
		return 17;

	return mPtr->maxLevel;
}

//获取最小等级
unsigned ReadGisConfig::getGisMinLevel()
{
	if (!mPtr)
		return 3;

	return mPtr->minLevel;
}

//获取瓦片路径
std::string nullStr;
const std::string & ReadGisConfig::getTilePath()
{
	if (!mPtr)
		return nullStr;

	return mPtr->tilePath;
}

//获取实体图标目录
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

//获取地图初始化位置
bool ReadGisConfig::getGisInitPos(double & initLon, double & initLat)
{
	if (!mPtr)
		return false;

	//初始化经度
	initLon = mPtr->initLongitude;
	//初始化纬度
	initLat = mPtr->initLatitude;

	return true;
}
