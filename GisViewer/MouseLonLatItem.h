#pragma once

#include <QGraphicsRectItem>

class MouseLonLatItem : public QGraphicsRectItem
{
public:
	MouseLonLatItem();
	~MouseLonLatItem();

	//���üӾ�γ��
	void setLonLatInfo(int gisLevel, double lon, double lat);

	//���±�ǩλ��
	void updateLabelPos();

	//����1���ر�ʾ��ʵ�ʾ���
	void setPixelMapScaleSize(double d);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	QString drawStr;
	QPointF currentLonlatPos;
	double pixelRealDistance;	//1���ص�ʵ�ʾ���
};
