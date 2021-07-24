#pragma once
#include <string>

//坐标转换接口
class TileSystem
{
	TileSystem();
	~TileSystem();
public:
	//防止经纬度是否超出范围
	static double Clip(double n, double minValue, double maxValue);

	//地图大小
	static unsigned MapSize(int gisLevel);

	//1像素 = ? 米
	static double GroundResolution(double latitude, int gisLevel);

	//
	static double MapScale(double latitude, int gisLevel, int screenDpi);

	//经纬度转像素坐标
	static void LonLatToPixelXY(double lon, double lat, int gisLevel, int & pixelX, int & pixelY);

	//像素坐标转经纬度
	static void PixelXYToLonLat(int pixelX, int pixelY, int gisLevel, double & lon, double & lat);

	//像素点转瓦片坐标
	static void PixelXYToTileXY(int pixelX, int pixelY, int & tileX, int & tileY);

	//瓦片坐标转经纬度
	static void TileXYToPixelXY(int tileX, int tileY, int & pixelX, int & pixelY);

	//瓦片坐标转换为key
	static std::string TileXYToQuadKey(int tileX, int tileY, int gisLevel);

	//通过key值获取到瓦片的xy坐标和地图等级
	static void QuadKeyToTileXY(const std::string & key, int & tileX, int & tileY, int & gisLevel);

};

