#include "RouteItem.h"
#include <QDateTime>
#include "TileSystem.h"
#include "GisStatus.h"
#include "GisSignals.h"
#include "GeoToQtMath.h"
#include <QPainter>
#include <time.h>

RouteItem::RouteItem(QGraphicsItem *parent)
	: QGraphicsPathItem(parent)
{
	//生成id
	//mData.ID = QDateTime::currentSecsSinceEpoch();
	mData.ID = clock();
}

RouteItem::RouteItem(QGraphicsItem *parent, const RouteInfo &data)
	: QGraphicsPathItem(parent)
{
	//根据mData 的经纬点数据 创建图元
	setRealData(data);
}

RouteItem::~RouteItem()
{

}

void RouteItem::addLonLatPoint(const QPointF & lonLatPoint)
{
	//添加点 数据
	mData.points.push_back(GeoPoint(lonLatPoint.x(), lonLatPoint.y()));

	//添加点图元
	RouteNodeItem * node = new RouteNodeItem(this, mData.points.back());
	node->hide();
	mNodes.push_back(node);

	updateLevel();
}

void RouteItem::updateLevel()
{
	if (mNodes.empty())
		return;

#if 0
	//根据经纬点 更新

	mNodes[0]->updatePos();

	QPainterPath path;
	path.moveTo(mNodes[0]->pos());

	for (int i = 1; i < mNodes.size(); ++i)
	{
		mNodes[i]->updatePos();
		path.lineTo(mNodes[i]->pos());
	}

	//判断是否闭合
	if (mData.isClose)
	{
		path.closeSubpath();
	}

	setPath(path);
#else

	//更新调整点
	for (int i = 0; i < mNodes.size(); ++i)
	{
		mNodes[i]->updatePos();
	}

	//计算插值点
	std::vector<GeoPoint> pointVec;
	pointVec.reserve(mData.points.size());

	for (auto & p : mData.points)
	{
		pointVec.push_back(p.position);
	}

	setPath(GeoToQtMath::getInterpolatPath(pointVec, 20000, GisStatus::instance().getGisLevel(), mData.isClose));
#endif
}

const RouteInfo & RouteItem::getRealData() const
{
	return mData;
}

void RouteItem::setRealData(const RouteInfo &route)
{
	mData = route;

	//清空
	for (auto & item : mNodes)
	{
		delete item;
	}
	mNodes.clear();

	mNodes.reserve(mData.points.size());
	for (auto & p : mData.points)
	{
		//添加点图元
		RouteNodeItem * node = new RouteNodeItem(this, p);
		node->hide();
		mNodes.push_back(node);
	}
	updateLevel();
}

const QPoint RouteItem::getBackPixelPos() const
{
	return mNodes.back()->pos().toPoint();
}

const GeoPoint & RouteItem::getBackGeoPos() const
{
	return mNodes.back()->getGeoPoint();
}

void RouteItem::setPointVisible(bool show)
{
	for (auto node : mNodes)
	{
		node->setVisible(show);
	}
}

void RouteItem::setNodeEditMode(bool mode)
{
	for (auto node : mNodes)
	{
		node->setNodeEditMode(mode);
	}
}

void RouteItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	painter->setPen(QPen(QColor(mData.color.r, mData.color.g, mData.color.b, mData.color.a)));
	painter->setRenderHint(QPainter::Antialiasing);
	
	//画路径
	painter->drawPath(this->path());

	// 画名字
	if (mNodes.size() > 0)
	{
		QPointF p = (*mNodes.begin())->pos();
		p += QPointF(0, -10);
		painter->drawText(p, mData.routeName.c_str());
	}

	painter->setPen(QPen());
	//画点
	for (auto &point : mNodes)
	{
		QPainterPath ellipse;
		ellipse.addEllipse(QRectF(point->x() - 3, point->y() - 3, 6, 6));
		painter->fillPath(ellipse, QColor(mData.color.r, mData.color.g, mData.color.b, mData.color.a));
		painter->drawPath(ellipse);
	}
}
