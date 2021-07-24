#pragma once

#include <QGraphicsPathItem>
#include "RouteNodeItem.h"
#include <GisStruct.h>

//����
class RouteItem : public QGraphicsPathItem
{
	//Q_OBJECT
public:
	RouteItem(QGraphicsItem *parent);
	RouteItem(QGraphicsItem *parent, const RouteInfo &);
	~RouteItem();

	//��ȡID
	inline long long getID() const { return mData.ID; }

	//��Ӿ�γ��
	void addLonLatPoint(const QPointF & lonLatPoint);

	//���ŵ�ͼ�ػ�
	void updateLevel();

	//��ȡԭʼ����
	const RouteInfo & getRealData() const;
	//����ԭʼ����
	void setRealData(const RouteInfo &);

	//��ȡ���һ���������λ��
	const QPoint getBackPixelPos() const;

	//��ȡ���һ����ľ�γ��
	const GeoPoint &getBackGeoPos() const;

	//���õ���������� 
	void setPointVisible(bool show);

	// ���õ������Ƿ�ɱ༭
	void setNodeEditMode(bool mode);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	friend class RouteNodeItem;	//���ߵ�item  ���ߵ��ƶ�֮����Ҫ����mData

	RouteInfo mData;	//����ʵ������

	QList<RouteNodeItem *> mNodes;	//�����ϵĵ�

};
