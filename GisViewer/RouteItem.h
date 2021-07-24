#pragma once

#include <QGraphicsPathItem>
#include "RouteNodeItem.h"
#include <GisStruct.h>

//航线
class RouteItem : public QGraphicsPathItem
{
	//Q_OBJECT
public:
	RouteItem(QGraphicsItem *parent);
	RouteItem(QGraphicsItem *parent, const RouteInfo &);
	~RouteItem();

	//获取ID
	inline long long getID() const { return mData.ID; }

	//添加经纬点
	void addLonLatPoint(const QPointF & lonLatPoint);

	//缩放地图重绘
	void updateLevel();

	//获取原始数据
	const RouteInfo & getRealData() const;
	//设置原始数据
	void setRealData(const RouteInfo &);

	//获取最后一个点的像素位置
	const QPoint getBackPixelPos() const;

	//获取最后一个点的经纬度
	const GeoPoint &getBackGeoPos() const;

	//设置调整点的显隐 
	void setPointVisible(bool show);

	// 设置调整点是否可编辑
	void setNodeEditMode(bool mode);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	friend class RouteNodeItem;	//航线点item  航线点移动之后需要更新mData

	RouteInfo mData;	//航线实际数据

	QList<RouteNodeItem *> mNodes;	//航线上的点

};
