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
	//����id
	//mData.ID = QDateTime::currentSecsSinceEpoch();
	mData.ID = clock();
}

RouteItem::RouteItem(QGraphicsItem *parent, const RouteInfo &data)
	: QGraphicsPathItem(parent)
{
	//����mData �ľ�γ������ ����ͼԪ
	setRealData(data);
}

RouteItem::~RouteItem()
{

}

void RouteItem::addLonLatPoint(const QPointF & lonLatPoint)
{
	//��ӵ� ����
	mData.points.push_back(GeoPoint(lonLatPoint.x(), lonLatPoint.y()));

	//��ӵ�ͼԪ
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
	//���ݾ�γ�� ����

	mNodes[0]->updatePos();

	QPainterPath path;
	path.moveTo(mNodes[0]->pos());

	for (int i = 1; i < mNodes.size(); ++i)
	{
		mNodes[i]->updatePos();
		path.lineTo(mNodes[i]->pos());
	}

	//�ж��Ƿ�պ�
	if (mData.isClose)
	{
		path.closeSubpath();
	}

	setPath(path);
#else

	//���µ�����
	for (int i = 0; i < mNodes.size(); ++i)
	{
		mNodes[i]->updatePos();
	}

	//�����ֵ��
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

	//���
	for (auto & item : mNodes)
	{
		delete item;
	}
	mNodes.clear();

	mNodes.reserve(mData.points.size());
	for (auto & p : mData.points)
	{
		//��ӵ�ͼԪ
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
	
	//��·��
	painter->drawPath(this->path());

	// ������
	if (mNodes.size() > 0)
	{
		QPointF p = (*mNodes.begin())->pos();
		p += QPointF(0, -10);
		painter->drawText(p, mData.routeName.c_str());
	}

	painter->setPen(QPen());
	//����
	for (auto &point : mNodes)
	{
		QPainterPath ellipse;
		ellipse.addEllipse(QRectF(point->x() - 3, point->y() - 3, 6, 6));
		painter->fillPath(ellipse, QColor(mData.color.r, mData.color.g, mData.color.b, mData.color.a));
		painter->drawPath(ellipse);
	}
}
