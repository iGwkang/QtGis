#include "GeoToQtMath.h"
#include "TileSystem.h"
#include "GeoMath.h"

QPainterPath GeoToQtMath::getInterpolatPath(const std::vector<GeoPoint> &geoPoints, double spacing, int level, bool isColse)
{
	QPoint _pos;

	//计算插值点
	QPainterPath path;

	//计算完插值点之后的经纬度集合
	std::vector<GeoPoint> outPoints;

	//按20公里计算插值
	if (GeoMath::tessellateGeo(geoPoints, outPoints, spacing, isColse))
	{
		TileSystem::LonLatToPixelXY(outPoints[0].longitude, outPoints[0].latitude, level, _pos.rx(), _pos.ry());

		path.moveTo(_pos);

		//遍历插值之后的经纬点
		for (int i = 1; i < outPoints.size(); ++i)
		{
			//转为像素坐标
			TileSystem::LonLatToPixelXY(outPoints[i].longitude, outPoints[i].latitude, level, _pos.rx(), _pos.ry());
			path.lineTo(_pos);
		}

		//闭合
		if (isColse)
		{
			path.closeSubpath();
		}
	}
	return path;
}
