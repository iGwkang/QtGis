#include "GeoToQtMath.h"
#include "TileSystem.h"
#include "GeoMath.h"

QPainterPath GeoToQtMath::getInterpolatPath(const std::vector<GeoPoint> &geoPoints, double spacing, int level, bool isColse)
{
	QPoint _pos;

	//�����ֵ��
	QPainterPath path;

	//�������ֵ��֮��ľ�γ�ȼ���
	std::vector<GeoPoint> outPoints;

	//��20��������ֵ
	if (GeoMath::tessellateGeo(geoPoints, outPoints, spacing, isColse))
	{
		TileSystem::LonLatToPixelXY(outPoints[0].longitude, outPoints[0].latitude, level, _pos.rx(), _pos.ry());

		path.moveTo(_pos);

		//������ֵ֮��ľ�γ��
		for (int i = 1; i < outPoints.size(); ++i)
		{
			//תΪ��������
			TileSystem::LonLatToPixelXY(outPoints[i].longitude, outPoints[i].latitude, level, _pos.rx(), _pos.ry());
			path.lineTo(_pos);
		}

		//�պ�
		if (isColse)
		{
			path.closeSubpath();
		}
	}
	return path;
}
