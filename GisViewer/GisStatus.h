#pragma once
#include <GisStruct.h>
#include <atomic>

//地图状态
class GisStatus
{
	GisStatus();
	GisStatus(const GisStatus &) = delete;
	~GisStatus();
public:
	static GisStatus & instance();

	//获取地图的操作模式
	const EnumMapMode getGisMode() const;

	//设置地图操作模式
	void setGisMode(EnumMapMode);

	//当前加载的瓦片等级  （非地图等级） 一般计算都用这个
	//int getTileLevel() const;
	//void setTileLevel(int);

	//地图等级
	int getGisLevel() const;
	void setGisLevel(int);

	//设置/获取 仿真时间
	void setCurrentSimulationTime(double simSec);
	const double getCurrentSimulationTime() const;

private:
	EnumMapMode mode;			//地图操作模式
	std::atomic<int> gisLevel;	//地图等级
	std::atomic<double> simTime;	//仿真时间
};
