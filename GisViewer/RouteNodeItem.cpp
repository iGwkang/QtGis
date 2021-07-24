#include "RouteNodeItem.h"
#include "TileSystem.h"
#include <QPainter>
#include "GisStatus.h"
#include "RouteItem.h"
#include "DataManager.hpp"
#include "GisSignals.h"
#include <QCursor>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>

RouteNodeItem::RouteNodeItem(QGraphicsItem *parent, const GeoPoint & geoPos)
	: QGraphicsObject(parent), mLocation(geoPos), isMoved(false)
{
	updatePos();

	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
	setFlag(QGraphicsItem::ItemIsMovable);
	setFlag(QGraphicsItem::ItemIsSelectable);

	setCursor(Qt::PointingHandCursor);
}

RouteNodeItem::~RouteNodeItem()
{
}

QRectF RouteNodeItem::boundingRect() const
{
	return QRectF(-3, -3, 6, 6);
}

void RouteNodeItem::updatePos()
{
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, false);

	QPoint mPos;
	TileSystem::LonLatToPixelXY(mLocation.longitude, mLocation.latitude, GisStatus::instance().getGisLevel(), mPos.rx(), mPos.ry());
	setPos(mPos);

	setFlag(QGraphicsItem::ItemSendsGeometryChanges);
}

void RouteNodeItem::setGeoPoint(const GeoPoint & geopos)
{
	mLocation = geopos;
	updatePos();
}

const GeoPoint & RouteNodeItem::getGeoPoint()
{
	return mLocation;
}

void RouteNodeItem::setNodeEditMode(bool mode)
{
	setFlag(QGraphicsItem::ItemSendsGeometryChanges, mode);
	setFlag(QGraphicsItem::ItemIsMovable, mode);
	setFlag(QGraphicsItem::ItemIsSelectable, mode);

	mode ? setCursor(Qt::PointingHandCursor) : setCursor(Qt::ArrowCursor);
}

void RouteNodeItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	QColor color = isSelected() ? Qt::red : Qt::yellow;

	painter->fillRect(boundingRect(), color);
	painter->drawRect(boundingRect());
}

QVariant RouteNodeItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == ItemPositionHasChanged && scene())
	{
		QPointF newPos = value.toPointF();
	
		//��������ת��γ��
		TileSystem::PixelXYToLonLat(newPos.x(), newPos.y(), GisStatus::instance().getGisLevel(), mLocation.longitude, mLocation.latitude);

		//���º����ϵ�����
		RouteItem * parent = dynamic_cast<RouteItem *>(parentItem());
		if (parent)
		{
			int index = parent->mNodes.indexOf(this);
			if (index != -1)
			{
				//��������
				parent->mData.points[index].position = mLocation;

				//����DataManager�������
				DataManager<RouteInfo>::instance().updateData(parent->mData);

				//�ػ�
				parent->updateLevel();

				isMoved = true;
			}
			else
			{
				deleteLater();
			}
		}
		else
		{
			deleteLater();
		}
	}

	return QGraphicsObject::itemChange(change, value);
}

void RouteNodeItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (isMoved && event->button() == Qt::LeftButton)
	{
		// ��ȡ����������ѡ�еĵ�����
		QList<QGraphicsItem *> items = scene()->selectedItems();
		QSet<RouteItem *> parentItems;

		for each(QGraphicsItem *item in items)
		{
			//���º����ϵ�����
			RouteNodeItem * nodeItem = dynamic_cast<RouteNodeItem *>(item);
			if (nodeItem)
			{
				RouteItem * routeItem = dynamic_cast<RouteItem *>(nodeItem->parentItem());
				if (routeItem)
					parentItems.insert(routeItem);
				// �����޸ĺ� ��Ҫ�����ź�
			}
		}

		// �����ź�
		for each (auto routeItem in parentItems)
		{
			emit GisSignals::instance()->gisUpdateRoute(routeItem->mData);
		}
	}
	QGraphicsObject::mouseReleaseEvent(event);
}
