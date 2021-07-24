#include "TileSystem.h"
#include <cmath>
#include <algorithm>
#include "GeoMath.h"

TileSystem::TileSystem()
{
}


TileSystem::~TileSystem()
{
}

double TileSystem::Clip(double n, double minValue, double maxValue)
{
	return std::min(std::max(n, minValue), maxValue);
}

unsigned TileSystem::MapSize(int gisLevel)
{
	return 256 << gisLevel;
}

double TileSystem::GroundResolution(double latitude, int gisLevel)
{
	latitude = Clip(latitude, MinLatitude, MaxLatitude);

	return std::cos(latitude * PI / 180) * EarthRadius * 2 * PI / MapSize(gisLevel);
}

double TileSystem::MapScale(double latitude, int gisLevel, int screenDpi)
{
	return GroundResolution(latitude, gisLevel) * screenDpi / 0.0254;
}

void TileSystem::LonLatToPixelXY(double lon, double lat, int gisLevel, int & pixelX, int & pixelY)
{
	//处理经纬度 避免超出范围
	lon = Clip(lon, MinLongitude, MaxLongitude);
	lat = Clip(lat, MinLatitude, MaxLatitude);

	double x = (lon + 180) / 360;
	double sinLat = std::sin(lat * PI / 180);
	double y = 0.5 - std::log((1 + sinLat) / (1 - sinLat)) / (4 * PI);
	int mapSize = MapSize(gisLevel);

	//pixelX = int(x * mapSize);
	pixelX = int(Clip(x * mapSize + 0.5, 0, mapSize - 1));
	pixelY = int(Clip(y * mapSize + 0.5, 0, mapSize - 1));

}

void TileSystem::PixelXYToLonLat(int pixelX, int pixelY, int gisLevel, double & lon, double & lat)
{
	double mapSize = MapSize(gisLevel);
	double x = (Clip(pixelX, 0, mapSize - 1) / mapSize) - 0.5;
	//double x = pixelX / mapSize - 0.5;
	double y = 0.5 - (Clip(pixelY, 0, mapSize - 1) / mapSize);

	lat = 90 - 360 * std::atan(std::exp(-y * 2 * PI)) / PI;
	lon = 360 * x;
}

void TileSystem::PixelXYToTileXY(int pixelX, int pixelY, int & tileX, int & tileY)
{
	tileX = pixelX / 256;
	tileY = pixelY / 256;
}

void TileSystem::TileXYToPixelXY(int tileX, int tileY, int & pixelX, int & pixelY)
{
	pixelX = tileX * 256;
	pixelY = tileY * 256;
}

std::string TileSystem::TileXYToQuadKey(int tileX, int tileY, int gisLevel)
{
	std::string key;
	for (int i = gisLevel; i > 0; --i)
	{
		char digit = '0';
		int mask = 1 << (i - 1);
		if ((tileX & mask) != 0)
		{
			++digit;
		}
		if ((tileY & mask) != 0)
		{
			digit += 2;
		}

		key += digit;
	}

	return key;
}

void TileSystem::QuadKeyToTileXY(const std::string & key, int & tileX, int & tileY, int & gisLevel)
{
	tileX = tileY = 0;

	gisLevel = key.length();

	for (int i = gisLevel; i > 0; --i)
	{
		int mask = 1 << (i - 1);

		switch (key[gisLevel - i])
		{
		case '0':
			break;
		case '1':
			tileX |= mask;
			break;
		case '2':
			tileY |= mask;
			break;
		case '3':
			tileX |= mask;
			tileY |= mask;
			break;
		default:
			break;
		}
	}
}
