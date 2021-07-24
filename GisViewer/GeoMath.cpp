#include "GeoMath.h"
#include "Vec3d.h"
#include "Quat.h"
#include <algorithm>

double GeoMath::distance(double lon1, double lat1, double lon2, double lat2)
{
	double lon1Rad = lon1 * PI / 180;
	double lon2Rad = lon2 * PI / 180;
	double lat1Rad = lat1 * PI / 180;
	double lat2Rad = lat2 * PI / 180;

	double dLat = (lat2Rad - lat1Rad);
	double dLon = (lon2Rad - lon1Rad);
	double a = std::sin(dLat / 2.0) * std::sin(dLat / 2.0) +
		std::cos(lat1Rad) * std::cos(lat2Rad) *
		std::sin(dLon / 2.0) * std::sin(dLon / 2.0);

	double c = 2.0 * std::atan2(std::sqrt(a), std::sqrt(1.0 - a));
	double d = EarthRadius * c;
	return d;
}

void GeoMath::interpolate(double lat1Rad, double lon1Rad, double lat2Rad, double lon2Rad, double t, double& out_latRad, double& out_lonRad)
{
	Vec3d v0, v1;

	convertLatLongHeightToXYZ(lat1Rad, lon1Rad, 0, v0.x(), v0.y(), v0.z());
	double r0 = v0.length();
	v0.normalize();
	convertLatLongHeightToXYZ(lat2Rad, lon2Rad, 0, v1.x(), v1.y(), v1.z());
	double r1 = v1.length();
	v1.normalize();

	Vec3d axis = v0 ^ v1;
	double angle = acos(v0 * v1);
	Quat q(angle * t, axis);

	v0 = (q * v0) * 0.5*(r0 + r1);

	double dummy;
	convertXYZToLatLongHeight(v0.x(), v0.y(), v0.z(), out_latRad, out_lonRad, dummy);
}

bool GeoMath::tessellateGeo(const std::vector<GeoPoint>& linearPath, std::vector<GeoPoint>& out_linearPath, double spacing, bool isClose)
{
	if (linearPath.empty())
		return false;
	out_linearPath.clear();

	//遍历给定的航线点
	for (int i = 0; i < linearPath.size() - 1; ++i)
	{
		const auto & p0 = linearPath[i];
		const auto & p1 = linearPath[i + 1];

		//转为弧度制
		double lon1Rad = DegreesToRadians(p0.longitude);
		double lat1Rad = DegreesToRadians(p0.latitude);
		double lon2Rad = DegreesToRadians(p1.longitude);
		double lat2Rad = DegreesToRadians(p1.latitude);

		//计算 距离 计算 要分的段数
		double d = distance(p0.longitude, p0.latitude, p1.longitude, p1.latitude);
		int slice = d / spacing;	//要分的段数

		out_linearPath.push_back(p0);	//把第一个点添加进去

		//高度差
		double zdelta = p1.altitude - p0.altitude;

		if (slice > 0)
		{
			double f = 1.0 / slice;

			for (int i = 1; i < slice; ++i)
			{
				GeoPoint outP;
				interpolate(lat1Rad, lon1Rad, lat2Rad, lon2Rad, f * i, outP.latitude, outP.longitude);

				//弧度转经纬度
				outP.longitude = RadiansToDegrees(outP.longitude);
				outP.latitude = RadiansToDegrees(outP.latitude);
				outP.altitude = f * i * zdelta + p0.altitude;

				out_linearPath.push_back(outP);
			}
		}
	}

	out_linearPath.push_back(linearPath.back());

	//如果是闭合的
	if (isClose && linearPath.size() > 2)
	{
		const auto & p0 = linearPath.back();
		const auto & p1 = linearPath.front();

		//转为弧度制
		double lon1Rad = DegreesToRadians(p0.longitude);
		double lat1Rad = DegreesToRadians(p0.latitude);
		double lon2Rad = DegreesToRadians(p1.longitude);
		double lat2Rad = DegreesToRadians(p1.latitude);

		//计算 距离 计算 要分的段数
		double d = distance(p0.longitude, p0.latitude, p1.longitude, p1.latitude);
		int slice = d / spacing;	//要分的段数

		double zdelta = p1.altitude - p0.altitude;//高度差

		if (slice > 0)
		{
			double f = 1.0 / slice;

			for (int i = 1; i < slice; ++i)
			{
				GeoPoint outP;
				interpolate(lat1Rad, lon1Rad, lat2Rad, lon2Rad, f * i, outP.latitude, outP.longitude);

				//弧度转经纬度
				outP.longitude = RadiansToDegrees(outP.longitude);
				outP.latitude = RadiansToDegrees(outP.latitude);
				outP.altitude = f * i * zdelta + p0.altitude;

				out_linearPath.push_back(outP);
			}
		}
	}
	return true;
}

void GeoMath::convertLatLongHeightToXYZ(double latitude, double longitude, double height, double& X, double& Y, double& Z)
{
	double sin_latitude = sin(latitude);
	double cos_latitude = cos(latitude);
	//double N = _radiusEquator / sqrt(1.0 - _eccentricitySquared*sin_latitude*sin_latitude);
	double N = EarthRadius;
	X = (N + height)*cos_latitude*cos(longitude);
	Y = (N + height)*cos_latitude*sin(longitude);
	Z = (N + height)*sin_latitude;
}

void GeoMath::convertXYZToLatLongHeight(double X, double Y, double Z, double& latitude, double& longitude, double& height)
{
	// handle polar and center-of-earth cases directly.
	if (X != 0.0)
		longitude = atan2(Y, X);
	else
	{
		if (Y > 0.0)
			longitude = PI_2;
		else if (Y < 0.0)
			longitude = -PI_2;
		else
		{
			// at pole or at center of the earth
			longitude = 0.0;
			if (Z > 0.0)
			{ // north pole.
				latitude = PI_2;
				//height = Z - _radiusPolar;
				height = Z - EarthRadius;
			}
			else if (Z < 0.0)
			{ // south pole.
				latitude = -PI_2;
				//height = -Z - _radiusPolar;
				height = -Z - EarthRadius;
			}
			else
			{ // center of earth.
				latitude = PI_2;
				//height = -_radiusPolar;
				height = -EarthRadius;
			}
			return;
		}
	}


	double p = sqrt(X*X + Y*Y);

	//double theta = atan2(Z*_radiusEquator, (p*_radiusPolar));
	double theta = atan2(Z*EarthRadius, (p*EarthRadius));

	//double eDashSquared = (_radiusEquator*_radiusEquator - _radiusPolar*_radiusPolar) /
	//	(_radiusPolar*_radiusPolar);
	double eDashSquared = 0;

	double sin_theta = sin(theta);
	double cos_theta = cos(theta);

	//latitude = atan((Z + eDashSquared*_radiusPolar*sin_theta*sin_theta*sin_theta) /
	//	(p - _eccentricitySquared*_radiusEquator*cos_theta*cos_theta*cos_theta));

	latitude = atan(Z / p);

	double sin_latitude = sin(latitude);
	double N = EarthRadius; // / sqrt(1.0 - _eccentricitySquared*sin_latitude*sin_latitude);

	height = p / cos(latitude) - N;
}
