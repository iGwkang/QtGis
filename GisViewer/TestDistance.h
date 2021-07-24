#pragma once

#include <QGraphicsPathItem>
#include <GisStruct.h>

//测距item
class TestDistance : public QGraphicsPathItem
{
public:
	TestDistance(QGraphicsItem *parent);
	~TestDistance();

	//添加经纬点
	void addLonLatPoint(const QPointF & lonLatPoint, int level);

	//缩放地图重绘
	void updateLevel();

	//获取最后一个点的像素位置
	const QPoint &getBackPixelPos() const;

	//获取最后一个点的经纬度
	const GeoPoint &getBackGeoPos() const;

	//获取点的数量
	int getPointNum() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	std::vector<GeoPoint> lonLatPoints;	//经纬点
	QList<QPoint> pixelPos;				//经纬点转换成像素点
	QList<QString> drawDistance;		//距离
};
