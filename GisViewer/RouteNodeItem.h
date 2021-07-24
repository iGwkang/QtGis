#pragma once

#include <QGraphicsObject>
#include <GisStruct.h>

//航线上的点
class RouteNodeItem : public QGraphicsObject
{
	//Q_OBJECT
public:
	RouteNodeItem(QGraphicsItem *parent, const GeoPoint &);
	~RouteNodeItem();

	QRectF boundingRect() const;

	//等级改变之后 更新点的位置
	void updatePos();

	//设置地理坐标
	void setGeoPoint(const GeoPoint &);

	//获取地理坐标
	const GeoPoint & getGeoPoint();
	
	// 设置是否可编辑
	void setNodeEditMode(bool);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);	//鼠标释放发送新航线数据

private:
	GeoPoint mLocation;	//地理坐标
	bool isMoved;
};
