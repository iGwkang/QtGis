#pragma once

#include <QGraphicsPathItem>
#include <GisStruct.h>

//���item
class TestDistance : public QGraphicsPathItem
{
public:
	TestDistance(QGraphicsItem *parent);
	~TestDistance();

	//��Ӿ�γ��
	void addLonLatPoint(const QPointF & lonLatPoint, int level);

	//���ŵ�ͼ�ػ�
	void updateLevel();

	//��ȡ���һ���������λ��
	const QPoint &getBackPixelPos() const;

	//��ȡ���һ����ľ�γ��
	const GeoPoint &getBackGeoPos() const;

	//��ȡ�������
	int getPointNum() const;

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	std::vector<GeoPoint> lonLatPoints;	//��γ��
	QList<QPoint> pixelPos;				//��γ��ת�������ص�
	QList<QString> drawDistance;		//����
};
