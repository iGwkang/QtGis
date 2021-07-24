#pragma once

#include <QGraphicsObject>
#include <GisStruct.h>

//�����ϵĵ�
class RouteNodeItem : public QGraphicsObject
{
	//Q_OBJECT
public:
	RouteNodeItem(QGraphicsItem *parent, const GeoPoint &);
	~RouteNodeItem();

	QRectF boundingRect() const;

	//�ȼ��ı�֮�� ���µ��λ��
	void updatePos();

	//���õ�������
	void setGeoPoint(const GeoPoint &);

	//��ȡ��������
	const GeoPoint & getGeoPoint();
	
	// �����Ƿ�ɱ༭
	void setNodeEditMode(bool);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);	//����ͷŷ����º�������

private:
	GeoPoint mLocation;	//��������
	bool isMoved;
};
