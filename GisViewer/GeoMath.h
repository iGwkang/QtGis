#pragma once
#include <GisStruct.h>

//地球半径
const double EarthRadius = 6378137.0;	//赤道
const double EarthPolar = 6356752.3142;	//南北极

//最大纬度
const double MaxLatitude = 85.05112878;
//最小纬度
const double MinLatitude = -MaxLatitude;

//最大经度
const double MaxLongitude = 180.0;
//最小经度
const double MinLongitude = -MaxLongitude;

//圆周率
const double PI = 3.14159265358979323846;	// Pi
const double PI_2 = 1.57079632679489661923;	// Pi / 2

/**
	@file GeoMath.h
	@class GeoMath
	@brief 地球上的一些空间计算
	@author gwk
	@date 2019-11-18 10:42:16
*/
class GeoMath
{
	GeoMath();
public:
	// 角度转弧度
	static inline double DegreesToRadians(double angle) { return angle*PI / 180.0; }
	// 弧度转角度
	static inline double RadiansToDegrees(double angle) { return angle*180.0 / PI; }
	
	/**
		@brief 计算地球两点之间的距离 (角度单位)
		@note 根据地球上起始点和终点的经纬度，计算两点之间的距离
		@param[in] lon1 : 起始点纬度
		@param[in] lat1 : 起始点经度
		@param[in] lon2 : 结束点纬度
		@param[in] lat2 : 结束点经度
		@return double : 返回两点间的距离
	*/
	static double distance(double lon1, double lat1, double lon2, double lat2);


	/**
		@brief 计算插值点 (弧度单位)
		@note 根据起始点和终点的经纬度，计算大地圆弧中间的插值点
		@param[in] lat1Rad : 起始点纬度
		@param[in] lon1Rad : 起始点经度
		@param[in] lat2Rad : 结束点纬度
		@param[in] lon2Rad : 结束点经度
		@param[in] t : 插值点百分比位置，范围 [0, 1]
		@param[out] out_latRad : 插值点的纬度
		@param[out] out_lonRad : 差指点的经度
		@return void
	*/
	static void interpolate(double lat1Rad, double lon1Rad,
		double lat2Rad, double lon2Rad,
		double t,
		double& out_latRad, double& out_lonRad);

	/**
		@brief 计算一条路径中的插值点 (角度单位)
		@note 给定一条航线, 按照大地圆弧计算出相应的插值点
		@param[in] linearPath : 航线中所有的点
		@param[out] out_linearPath : 计算完插值之后返回的航线点
		@param[in] spacing : 计算插值的间距 单位: m（每隔 spacing 米计算一个插值点）
		@param[in] isClose : 航线是否闭合
		@return bool 当linearPath为空时 返回false
	*/
	static bool tessellateGeo(const std::vector<GeoPoint> & linearPath, std::vector<GeoPoint> & out_linearPath, double spacing, bool isClose = false);


	//坐标转换  没有考虑偏率 (弧度单位)
	static void convertLatLongHeightToXYZ(double latitudeRad, double longitudeRad, double height,
		double& X, double& Y, double& Z);

	//坐标转换  没有考虑偏率 (弧度单位)
	static void convertXYZToLatLongHeight(double X, double Y, double Z,
		double& latitudeRad, double& longitudeRad, double& height);
};

