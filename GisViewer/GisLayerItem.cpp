#include "TileSystem.h"
#include "DrawItemLayer.h"
#include "GisLayerItem.h"
#include "MapItem.h"
#include "ReadGisConfig.h"
#include "RouteAreaLayer.h"
#include "GisStatus.h"
#include "EntityLayer.h"
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QPainter>
#include <QApplication>
#include <iostream>

GisLayerItem * GisLayerItem::getInstance()
{
	return static_instance;
}

GisLayerItem::GisLayerItem(QGraphicsItem * parent)
	: QGraphicsObject(parent)//, mouseIsMoved(false)
{
	this->setFlags(flags() | ItemIsMovable | ItemIsFocusable | ItemIsSelectable);

	//初始化地图等级
	int initLevel = ReadGisConfig::instance().getInitLevel();
	GisStatus::instance().setGisLevel(initLevel);

	//初始化瓦片层
	mTileLayer = new MapItem(this);

	//航线层
	mRouteAreaLayer = new RouteAreaLayer(this);
	
	//实体层
	mEntityLayer = new EntityLayer(this);
	
	//绘图层
	mDrawItemLayer = new DrawItemLayer(this);

	//初始化地图位置
	initGisPos();

	initSigAndSlt();

	//实例
	static_instance = this;
}

GisLayerItem::~GisLayerItem()
{

}

QRectF GisLayerItem::boundingRect() const
{
	return mBoundingRect;
}

QPointF GisLayerItem::getLonLatFromScenePos(const QPointF & scenePos)
{
	QPoint px = this->mapFromScene(scenePos).toPoint();

	QPointF ret;
	TileSystem::PixelXYToLonLat(px.x(), px.y(), GisStatus::instance().getGisLevel(), ret.rx(), ret.ry());
	return ret;
}

QPointF GisLayerItem::getScenePosFromLonLat(const QPointF & lonlatPos)
{
	QPoint px;
	TileSystem::LonLatToPixelXY(lonlatPos.x(), lonlatPos.y(), GisStatus::instance().getGisLevel(), px.rx(), px.ry());
	
	return mapToScene(px);
}

// int GisLayerItem::getGisLevel() const
// {
// 	//return gisLevel;
// 	return scaleLevel;
// }

void GisLayerItem::zoom(int delta)
{
	//long st = clock();

	int gisLevel = GisStatus::instance().getGisLevel();
	if (delta > 0)
	{
		//判断是否超出范围
		//if (gisLevel + 1 > 22)
		if (gisLevel + 1 > ReadGisConfig::instance().getGisMaxLevel())
			return;
		gisLevel++;

		GisStatus::instance().setGisLevel(gisLevel);
	}
	else
	{
		//判断是否超出范围
		//if (gisLevel - 1 < 0)
		if (gisLevel - 1 <  ReadGisConfig::instance().getGisMinLevel())
			return;
		--gisLevel;	//瓦片等级--
		GisStatus::instance().setGisLevel(gisLevel);
	}

	/*
	if (delta > 0)
	{
		if (gisLevel + 1 > 23)
			return;

		//地图等级++
		gisLevel++;
		GisStatus::instance().setGisLevel(gisLevel);

		//瓦片等级
		if (tileLevel >= ReadGisConfig::instance().getGisMaxLevel() || scale() < 1)
		{
			//setScale(scale() * 2);
			//return;
			mTileLayer->setScale(mTileLayer->scale() * 2);
		}
	

		++tileLevel;	//瓦片等级++
		GisStatus::instance().setTileLevel(tileLevel);
	}
	else
	{
		if (gisLevel - 1 < 0)
			return;

		//地图等级++
		gisLevel--;
		GisStatus::instance().setGisLevel(gisLevel);

		if (tileLevel <= ReadGisConfig::instance().getGisMinLevel() || scale() > 1)
		{
			//setScale(scale() / 2);
			//return;
			mTileLayer->setScale(mTileLayer->scale() / 2);
		}

		--tileLevel;	//瓦片等级--
		GisStatus::instance().setTileLevel(tileLevel);
	}

	*/

	//更新包围盒
	mBoundingRect = QRectF(0, 0, 256 << gisLevel, 256 << gisLevel);

	//实体层更新
	mEntityLayer->updateLevel();

	//瓦片层更新
	mTileLayer->zoomTileLevel(delta);

	//绘图层更新
	mDrawItemLayer->updateLevel();

	//航线层更新
	mRouteAreaLayer->updateLevel();
	
	//scene()->update();
	//QApplication::processEvents();
	//std::cout << __FUNCTION__ << "  " << __LINE__ << "  cpu_use_time:" << clock() - st << std::endl;
}

void GisLayerItem::setMeasureMode(bool mode)
{
	mDrawItemLayer->setMeasureMode(mode);
}

void GisLayerItem::setMeasureVisible(bool show)
{
	mDrawItemLayer->setMeasureVisible(show);
}

void GisLayerItem::setRouteVisible(bool show, const long long &route_id)
{
	return mRouteAreaLayer->setRouteVisible(show, route_id);
}

void GisLayerItem::setRoutePointVisible(bool show, const long long &route_id /*= 0*/)
{
	return mRouteAreaLayer->setRoutePointVisible(show, route_id);
}

void GisLayerItem::setEntityVisible(bool show, const long long &entity_id)
{
	return mEntityLayer->setEntityVisible(show, entity_id);
}

void GisLayerItem::setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType)
{
	return mEntityLayer->setEntityAbilityCircleVisible(show, entity_id, circleType);
}

void GisLayerItem::setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType)
{
	return mEntityLayer->setEntityAbilityCircleScanVisible(show, entity_id, circleType);
}

void GisLayerItem::setEntityScutcheonVisible(bool show, const long long & entity_id /*= 0*/)
{
	return mEntityLayer->setEntityScutcheonVisible(show, entity_id);
}

void GisLayerItem::setDrawRouteMode(bool mode)
{
	mRouteAreaLayer->setDrawRouteMode(mode);
}

void GisLayerItem::setEditMode(bool mode)
{
	mEntityLayer->setEditMode(mode);
	mRouteAreaLayer->setEditMode(mode);
}

void GisLayerItem::clearGisScene()
{
	// 绘图层
	mDrawItemLayer->clearData();
	// 航路层
	mRouteAreaLayer->clearData();
	// 实体层
	mEntityLayer->clearData();

	// 地图恢复状态
	GisStatus::instance().setGisMode(MoveMapMode);
	GisStatus::instance().setCurrentSimulationTime(0);
}

void GisLayerItem::setSvgDir(const std::string &dir)
{
	mEntityLayer->setSvgDir(dir);
}

void GisLayerItem::addEntity(const EntityInfo &data)
{
	mEntityLayer->addEntity(data);
}

void GisLayerItem::addEntity(const std::vector<EntityInfo> &data)
{
	mEntityLayer->addEntity(data);
}

void GisLayerItem::removeEntity(const long long &id)
{
	mEntityLayer->removeEntity(id);
}

void GisLayerItem::removeEntity(const std::vector<long long> &id)
{
	mEntityLayer->removeEntity(id);
}

void GisLayerItem::updateEntity(const EntityInfo &data)
{
	mEntityLayer->updateEntity(data);
}

void GisLayerItem::updateEntity(const std::vector<EntityInfo> &data)
{
	mEntityLayer->updateEntity(data);
}

void GisLayerItem::updateEntity(const std::unordered_map<long long, EntityInfo> &data)
{
	mEntityLayer->updateEntity(data);
}

void GisLayerItem::setCircleParam(long long entity_id, const AbilityCircleParam & param)
{
	mEntityLayer->setCircleParam(entity_id, param);
}

void GisLayerItem::removeCircle(long long entity_id, const std::string & circleType)
{
	mEntityLayer->removeCircle(entity_id, circleType);
}

void GisLayerItem::setScanParam(long long entity_id, const ScanParam & param)
{
	mEntityLayer->setScanParam(entity_id, param);
}

void GisLayerItem::addRoute(const RouteInfo &route)
{
	mRouteAreaLayer->addRoute(route);
}

void GisLayerItem::addRoute(const std::vector<RouteInfo> &route)
{
	mRouteAreaLayer->addRoute(route);
}

void GisLayerItem::removeRoute(const long long &id)
{
	mRouteAreaLayer->removeRoute(id);
}

void GisLayerItem::removeRoute(const std::vector<long long> &id)
{
	mRouteAreaLayer->removeRoute(id);
}

void GisLayerItem::updateRoute(const RouteInfo &route)
{
	mRouteAreaLayer->updateRoute(route);
}

void GisLayerItem::updateRoute(const std::vector<RouteInfo> &route)
{
	mRouteAreaLayer->updateRoute(route);
}

void GisLayerItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr*/)
{
	//painter->drawRect(mBoundingRect);
}

void GisLayerItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		pressScenePos = event->scenePos();
		//std::cout << "X:" << event->pos().x() / 255 << "	Y:" << /*(1 << GisStatus::instance().getGisLevel()) - 1 - */event->pos().y() / 255 << std::endl;
	}
	//mouseIsMoved = false;
	mousePressPos = event->scenePos();

	QGraphicsObject::mousePressEvent(event);
}

void GisLayerItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->buttons() & Qt::LeftButton)
	{
		QPointF offset = event->scenePos() - pressScenePos;
		pressScenePos = event->scenePos();
		this->moveBy(offset.x(), offset.y());
	}
	
}

void GisLayerItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (QLineF(mousePressPos, event->scenePos()).length() < 5)
	{
		//传递给子图层
		mDrawItemLayer->mouseReleaseEvent(event);
		mRouteAreaLayer->mouseReleaseEvent(event);
	}
	
	QGraphicsObject::mouseReleaseEvent(event);
}

void GisLayerItem::keyReleaseEvent(QKeyEvent *event)
{
	//传递给子图层
	mDrawItemLayer->keyReleaseEvent(event);
	mRouteAreaLayer->keyReleaseEvent(event);

	QGraphicsObject::keyReleaseEvent(event);
}

void GisLayerItem::initGisPos()
{
	int gisLevel = ReadGisConfig::instance().getInitLevel();
	mBoundingRect = QRectF(0, 0, 256 << gisLevel, 256 << gisLevel);
	QPointF lonLat;
	ReadGisConfig::instance().getGisInitPos(lonLat.rx(), lonLat.ry());

	//经纬度转像素坐标
	QPoint pixel;
	TileSystem::LonLatToPixelXY(lonLat.x(), lonLat.y(), gisLevel, pixel.rx(), pixel.ry());

	this->setPos(-pixel.x() /*+ scene()->width() / 2*/, -pixel.y()/* + scene()->height() / 2*/);
}

void GisLayerItem::initSigAndSlt()
{
	
}

GisLayerItem * GisLayerItem::static_instance = nullptr;
