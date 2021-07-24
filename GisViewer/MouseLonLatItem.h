#pragma once

#include <QGraphicsRectItem>

class MouseLonLatItem : public QGraphicsRectItem
{
public:
	MouseLonLatItem();
	~MouseLonLatItem();

	//设置加经纬度
	void setLonLatInfo(int gisLevel, double lon, double lat);

	//更新标签位置
	void updateLabelPos();

	//设置1像素表示的实际距离
	void setPixelMapScaleSize(double d);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	QString drawStr;
	QPointF currentLonlatPos;
	double pixelRealDistance;	//1像素的实际距离
};
