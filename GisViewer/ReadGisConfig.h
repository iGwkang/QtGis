#pragma once
#include <string>

struct GisConfigData;

//读取地图配置文件
class ReadGisConfig
{
	ReadGisConfig();
	~ReadGisConfig();
public:
	static inline ReadGisConfig & instance()
	{
		static ReadGisConfig conf;
		return conf;
	}

	//读取配置文件
	void readConfig(const std::string &confPath);

	//获取地图最大等级
	unsigned getGisMaxLevel();
	//获取地图最小等级
	unsigned getGisMinLevel();

	//获取地图瓦片路径
	const std::string & getTilePath();

	//获取实体图标目录
	const std::string & getIconDir();

	//获取地图初始化时的等级，经度，纬度
	int getInitLevel();
	bool getGisInitPos(double & initLon, double & initLat);

private:
	GisConfigData *mPtr;
};
