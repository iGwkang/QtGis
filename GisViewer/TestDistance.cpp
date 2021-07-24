#include "TestDistance.h"
#include "GeoMath.h"
#include "TileSystem.h"
#include <QPainter>
#include "GisStatus.h"
#include "GeoToQtMath.h"
#include <iostream>
#include <time.h>

TestDistance::TestDistance(QGraphicsItem *parent)
	: QGraphicsPathItem(parent)
{

}

TestDistance::~TestDistance()
{
}

void TestDistance::addLonLatPoint(const QPointF & lonLatPoint, int level)
{
	if (drawDistance.isEmpty())
	{
		drawDistance.push_back(QString::number(0, 'f', 1) + " KM");
	}
	else
	{
		double t = drawDistance.back().split(' ').front().toDouble();
		//计算距离
		double d = t + GeoMath::distance(lonLatPoints.back().longitude, lonLatPoints.back().latitude, lonLatPoint.x(), lonLatPoint.y()) / 1000;
		drawDistance.push_back(QString::number(d, 'f', 1) + " KM");
	}
	
	//添加经纬点
	lonLatPoints.push_back(GeoPoint(lonLatPoint.x(), lonLatPoint.y()));

#if 0	//没有计算插值点
	QPoint pixel;
	//经纬度转像素坐标
	TileSystem::LonLatToPixelXY(lonLatPoint.x(), lonLatPoint.y(), level, pixel.rx(), pixel.ry());
	pixelPos.append(pixel);

	QPainterPath path;
	path.moveTo(pixelPos[0]);

	//需要计算插值
	for (int i = 1; i < pixelPos.size(); ++i)
	{
		path.lineTo(pixelPos[i]);
	}
	setPath(path);
#else
	QPoint _pos;
	//经纬度转像素坐标
	TileSystem::LonLatToPixelXY(lonLatPoint.x(), lonLatPoint.y(), level, _pos.rx(), _pos.ry());
	pixelPos.append(_pos);

	setPath(GeoToQtMath::getInterpolatPath(lonLatPoints, 20000, level));
#endif
}

void TestDistance::updateLevel()
{
	if (lonLatPoints.empty())
		return;
#if 0	//没有计算插值点
	//地图缩放 重新计算经纬点
	//long st = clock();
	TileSystem::LonLatToPixelXY(lonLatPoints[0].x, lonLatPoints[0].y, GisStatus::instance().getGisLevel(), pixelPos[0].rx(), pixelPos[0].ry());

	QPainterPath path;
	path.moveTo(pixelPos[0]);

	//构建需要绘制的path
	for (int i = 1; i < pixelPos.size(); ++i)
	{
		TileSystem::LonLatToPixelXY(lonLatPoints[i].x, lonLatPoints[i].y, GisStatus::instance().getGisLevel(), pixelPos[i].rx(), pixelPos[i].ry());

		path.lineTo(pixelPos[i]);
	}
	
	setPath(path);
	//std::cout << __FUNCTION__ << "  " << __LINE__ << "  cpu_use_time:" << clock() - st << std::endl;
#else

	TileSystem::LonLatToPixelXY(lonLatPoints[0].longitude, lonLatPoints[0].latitude, GisStatus::instance().getGisLevel(), pixelPos[0].rx(), pixelPos[0].ry());

	//构建需要绘制的path
	for (int i = 1; i < pixelPos.size(); ++i)
	{
		TileSystem::LonLatToPixelXY(lonLatPoints[i].longitude, lonLatPoints[i].latitude, GisStatus::instance().getGisLevel(), pixelPos[i].rx(), pixelPos[i].ry());
	}

	//updatePath();
	setPath(GeoToQtMath::getInterpolatPath(lonLatPoints, 20000, GisStatus::instance().getGisLevel()));
#endif
}

const QPoint &TestDistance::getBackPixelPos() const
{
	return pixelPos.back();
}

const GeoPoint &TestDistance::getBackGeoPos() const
{
	return lonLatPoints.back();
}

int TestDistance::getPointNum() const
{
	return lonLatPoints.size();
}

void TestDistance::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	QPen pen;
	pen.setWidth(2);
	painter->setPen(pen);
	painter->setRenderHint(QPainter::Antialiasing);

	//画路径
	painter->drawPath(this->path());

	//画点
	for (auto &point : pixelPos)
	{
		QPainterPath ellipse;
		ellipse.addEllipse(QRectF(point.x() - 4, point.y() - 4, 8, 8));
		painter->fillPath(ellipse, Qt::red);
		painter->drawPath(ellipse);
	}

	//画距离
	for (int i = 0; i < drawDistance.size(); ++i)
	{
		painter->drawText(pixelPos[i] - QPoint(-5, 5),drawDistance[i]);
	}
}
