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

	//退出 
	if (!mode)
	{
		setAcceptHoverEvents(false);

		//结束绘制
		if (currentDraw)
		{
			//如果点 少于两个 直接删除该航线
			if (currentDraw->getRealData().points.size() < 2)
			{
				mAllRoute.erase(currentDraw->getID());
				delete currentDraw;

				emit GisSignals::instance()->finishDrawRoute(RouteInfo());
			}
			else
			{
				// 航线绘制完成 需要保存到DataManager
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
	//左键绘制 右键退出
	if (event->button() == Qt::LeftButton)
	{
		//绘制航线模式
		if (drawRouteMode)
		{
			if (currentDraw == nullptr)
			{
				currentDraw = new RouteItem(this);
				mAllRoute[currentDraw->getID()] = currentDraw;

				setAcceptHoverEvents(true);
			}

			// 如果图层隐藏 就不应该触发绘制航线

			QPointF pixel = event->pos();

			//像素坐标转经纬度
			TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), pixel.rx(), pixel.ry());

			//添加经纬点
			currentDraw->addLonLatPoint(pixel);

			//update();
		}
	}
	else if (event->button() == Qt::RightButton)
	{
		if (currentDraw && drawRouteMode)
		{
			//退出测距 进入移动地图模式
			GisStatus::instance().setGisMode(MoveMapMode);
			//update();
		}
	}
}

void RouteAreaLayer::addRoute(const RouteInfo &route)
{
	//不存在才能添加
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

//实时获取鼠标 位置 绘制航线
void RouteAreaLayer::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	QPoint pixel = event->pos().toPoint();
	TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), mouseMoveLonlatPos.rx(), mouseMoveLonlatPos.ry());
	//update();
	QGraphicsObject::hoverMoveEvent(event);
}

//接收键盘按下事件
void RouteAreaLayer::keyReleaseEvent(QKeyEvent *event)
{
	if (!isVisible())
		return;

	switch (event->key())
	{
	case Qt::Key_Escape:
		//如果在绘制航线 则退出绘制
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
	if (route_id == 0)	//控制所有
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
		// 通过id找到航线 设置显隐
		auto _iter = mAllRoute.find(route_id);
		if (_iter != mAllRoute.end())
		{
			_iter->second->setVisible(show);
		}
	}
}

void RouteAreaLayer::setRoutePointVisible(bool show, const long long &route_id /*= 0*/)
{
	if (route_id == 0)	//控制所有
	{
		for (auto & _iter : mAllRoute)
		{
			_iter.second->setPointVisible(show);
		}
	}
	else
	{
		// 通过id找到航线 设置显隐
		auto _iter = mAllRoute.find(route_id);
		if (_iter != mAllRoute.end())
		{
			_iter->second->setPointVisible(show);
		}
	}
}

void RouteAreaLayer::clearData()
{
	//清空航线
	for (auto & item : mAllRoute) {
		delete item.second;
	}
	mAllRoute.clear();

	//删除当前在画的航线
	if (currentDraw)
	{
		setAcceptHoverEvents(false);
		drawRouteMode = false;
		delete currentDraw;
		currentDraw = nullptr;
	}
}
