#include "RouteAreaLayer.h"
#include <QGraphicsSceneMouseEvent>
#include "GisStatus.h"
#include "TileSystem.h"
#include "DataManager.hpp"
#include "GisSignals.h"
#include "GeoToQtMath.h"
#include <QPainter>
#include <QKeyEvent>

RouteAreaLayer::RouteAreaLayer(QGraphicsItem *parent)
	: QGraphicsObject(parent), drawRouteMode(false), currentDraw(nullptr)
{

}

RouteAreaLayer::~RouteAreaLayer()
{
}

QRectF RouteAreaLayer::boundingRect() const
{
	return parentItem()->boundingRect();
}

void RouteAreaLayer::setDrawRouteMode(bool mode)
{
	if (drawRouteMode == mode)
		return;
	if (isVisible() == false)
	{
		emit GisSignals::instance()->finishDrawRoute(RouteInfo());
		return;
	}

	//�˳� 
	if (!mode)
	{
		setAcceptHoverEvents(false);

		//��������
		if (currentDraw)
		{
			//����� �������� ֱ��ɾ���ú���
			if (currentDraw->getRealData().points.size() < 2)
			{
				mAllRoute.erase(currentDraw->getID());
				delete currentDraw;

				emit GisSignals::instance()->finishDrawRoute(RouteInfo());
			}
			else
			{
				// ���߻������ ��Ҫ���浽DataManager
				DataManager<RouteInfo>::instance().addData(currentDraw->getRealData());
				emit GisSignals::instance()->finishDrawRoute(currentDraw->getRealData());
			}
		}
		else
		{
			if (drawRouteMode)
			{
				emit GisSignals::instance()->finishDrawRoute(RouteInfo());
			}
		}
	}

	
	currentDraw = nullptr;
	drawRouteMode = mode;
}

void RouteAreaLayer::setEditMode(bool mode)
{
	for (auto & route : mAllRoute)
	{
		route.second->setNodeEditMode(mode);
	}
}

void RouteAreaLayer::updateLevel()
{
	for (auto & route : mAllRoute)
	{
		route.second->updateLevel();
	}

}

void RouteAreaLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (!isVisible())
		return;
	//������� �Ҽ��˳�
	if (event->button() == Qt::LeftButton)
	{
		//���ƺ���ģʽ
		if (drawRouteMode)
		{
			if (currentDraw == nullptr)
			{
				currentDraw = new RouteItem(this);
				mAllRoute[currentDraw->getID()] = currentDraw;

				setAcceptHoverEvents(true);
			}

			// ���ͼ������ �Ͳ�Ӧ�ô������ƺ���

			QPointF pixel = event->pos();

			//��������ת��γ��
			TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), pixel.rx(), pixel.ry());

			//��Ӿ�γ��
			currentDraw->addLonLatPoint(pixel);

			//update();
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (currentDraw && drawRouteMode)
		{
			//�˳���� �����ƶ���ͼģʽ
			GisStatus::instance().setGisMode(MoveMapMode);
			//update();
		}
	}
}

void RouteAreaLayer::addRoute(const RouteInfo &route)
{
	//�����ڲ������
	if (route.ID && mAllRoute.find(route.ID) == mAllRoute.end())
	{
		RouteItem * newItem = new RouteItem(this, route);
		mAllRoute[newItem->getID()] = newItem;
	}
}

void RouteAreaLayer::addRoute(const std::vector<RouteInfo> &routes)
{
	for (auto & route : routes)
	{
		addRoute(route);
	}
}

void RouteAreaLayer::removeRoute(const long long &id)
{
	auto _iter = mAllRoute.find(id);
	if (_iter != mAllRoute.end())
	{
		delete _iter->second;
		mAllRoute.erase(_iter);
	}
}

void RouteAreaLayer::removeRoute(const std::vector<long long> &ids)
{
	for (auto & id : ids)
	{
		removeRoute(id);
	}
}

void RouteAreaLayer::updateRoute(const RouteInfo &route)
{
	auto _iter = mAllRoute.find(route.ID);
	if (_iter != mAllRoute.end())
	{
		_iter->second->setRealData(route);
	}
}

void RouteAreaLayer::updateRoute(const std::vector<RouteInfo> &routes)
{
	for (auto & route : routes)
	{
		updateRoute(route);
	}
}

void RouteAreaLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	if (currentDraw && drawRouteMode)
	{
		QPen pen;
		pen.setStyle(Qt::DotLine);

		painter->setPen(pen);
		painter->setRenderHint(QPainter::Antialiasing);
		/*
		QPoint pixel;
		TileSystem::LonLatToPixelXY(mouseMoveLonlatPos.x(), mouseMoveLonlatPos.y(), GisStatus::instance().getGisLevel(), pixel.rx(), pixel.ry());

		painter->drawLine(currentDraw->getBackPixelPos(), pixel);
		*/

		std::vector<GeoPoint> inVec;
		inVec.push_back(GeoPoint(mouseMoveLonlatPos.x(), mouseMoveLonlatPos.y()));
		inVec.push_back(currentDraw->getBackGeoPos());

		painter->drawPath(GeoToQtMath::getInterpolatPath(inVec, 20000, GisStatus::instance().getGisLevel()));
	}
}

//ʵʱ��ȡ��� λ�� ���ƺ���
void RouteAreaLayer::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	QPoint pixel = event->pos().toPoint();
	TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), mouseMoveLonlatPos.rx(), mouseMoveLonlatPos.ry());
	//update();
	QGraphicsObject::hoverMoveEvent(event);
}

//���ռ��̰����¼�
void RouteAreaLayer::keyReleaseEvent(QKeyEvent *event)
{
	if (!isVisible())
		return;

	switch (event->key())
	{
	case Qt::Key_Escape:
		//����ڻ��ƺ��� ���˳�����
		if (drawRouteMode)
		{
			if (currentDraw)
			{
				mAllRoute.erase(currentDraw->getID());
				delete currentDraw;
				currentDraw = nullptr;
			}
			GisStatus::instance().setGisMode(MoveMapMode);
		}
		break;
	default:
		break;
	}
}

void RouteAreaLayer::setRouteVisible(bool show, const long long &route_id)
{
	if (route_id == 0)	//��������
	{
		GisStatus::instance().setGisMode(MoveMapMode);

		setVisible(show);
		//for (auto & _iter : mAllRoute)
		//{
		//	_iter.second->setVisible(show);
		//}
	}
	else
	{
		// ͨ��id�ҵ����� ��������
		auto _iter = mAllRoute.find(route_id);
		if (_iter != mAllRoute.end())
		{
			_iter->second->setVisible(show);
		}
	}
}

void RouteAreaLayer::setRoutePointVisible(bool show, const long long &route_id /*= 0*/)
{
	if (route_id == 0)	//��������
	{
		for (auto & _iter : mAllRoute)
		{
			_iter.second->setPointVisible(show);
		}
	}
	else
	{
		// ͨ��id�ҵ����� ��������
		auto _iter = mAllRoute.find(route_id);
		if (_iter != mAllRoute.end())
		{
			_iter->second->setPointVisible(show);
		}
	}
}

void RouteAreaLayer::clearData()
{
	//��պ���
	for (auto & item : mAllRoute) {
		delete item.second;
	}
	mAllRoute.clear();

	//ɾ����ǰ�ڻ��ĺ���
	if (currentDraw)
	{
		setAcceptHoverEvents(false);
		drawRouteMode = false;
		delete currentDraw;
		currentDraw = nullptr;
	}
}
