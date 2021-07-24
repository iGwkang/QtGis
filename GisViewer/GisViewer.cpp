#include "GisViewer.h"
#include <QGraphicsScene>
#include <QWheelEvent>
#include <iostream>
#include <QScrollBar>
#include <QApplication>
#include <QDesktopWidget>
#include <QTimer>
#include <QReadWriteLock>
#include <algorithm>

#include "GisLayerItem.h"
#include "MouseLonLatItem.h"
#include "GisStatus.h"
#include "DataManager.hpp"
#include "AbilityCircleDataManager.hpp"

class GisViewerPrivate
{
public:
	GisViewerPrivate()
		: mLayer(nullptr), mLonlatLabel(nullptr)
	{}
	//图层
	GisLayerItem * mLayer;

	//经纬度
	MouseLonLatItem *mLonlatLabel;
};

GisViewer::GisViewer(QWidget * parent /*= nullptr*/)
	: QGraphicsView(parent), mPtr(new GisViewerPrivate)
{
	//初始化场景
	this->setScene(new QGraphicsScene(this));

	//初始化图层
	mPtr->mLayer = new GisLayerItem;
	scene()->addItem(mPtr->mLayer);

	initUi();
	initSigAndSlt();

	//实时接收鼠标事件
	setMouseTracking(true);
	
	// 设置定时刷新
	float frame = 25;	//帧速
	int timeInterval = 1000 / frame;	//时间间隔 ms
	QTimer *timer = new QTimer(this);
	timer->setTimerType(Qt::PreciseTimer);
	//long preClock = clock();
	
	connect(timer, &QTimer::timeout, this, [&] () mutable {
		//long curClock = clock();
		
		//这一帧和上一帧的时间差
		//long d = curClock - preClock;
		//preClock = curClock;
		
		viewport()->update();
		//scene()->update();
		//QCoreApplication::processEvents();

		//如果d 大于 timeInterval，说明处理的事件长了，需要缩短定时器触发间隔
// 		if (d > timeInterval)
// 		{
// 			timer->setInterval(std::max(long(1), timeInterval - (d - timeInterval)));
// 		}
// 		else
// 		{
// 			timer->setInterval(timeInterval);
// 		}

		//std::cout << timer->interval() << "  " << d << std::endl;
	});
	timer->start(timeInterval);
	
}

GisViewer::~GisViewer()
{
	delete mPtr;
	mPtr = nullptr;
}

void GisViewer::setMeasureMode(bool mode)
{
	//mLayer->setMeasureMode(mode);

	//进入测距模式
	if (mode)
	{
		GisStatus::instance().setGisMode(MeasureMode);
	}
	else
	{
		GisStatus::instance().setGisMode(MoveMapMode);
	}
}

void GisViewer::setDrawRouteMode(bool mode)
{
	if (mode)
	{
		GisStatus::instance().setGisMode(DrawRouteLineMode);
	}
	else
	{
		GisStatus::instance().setGisMode(MoveMapMode);
	}
}

void GisViewer::setMeasureVisible(bool show)
{
	mPtr->mLayer->setMeasureVisible(show);
}

void GisViewer::clearGisScene()
{
	//清空地图场景中的数据
	mPtr->mLayer->clearGisScene();

	//清空数据管理中的数据
	DataManager<EntityInfo>::instance().clearData();
	DataManager<RouteInfo>::instance().clearData();
	AbilityCircleDataManager::instance().clearData();
}

void GisViewer::setRouteVisible(bool show, const long long &route_id)
{
	return mPtr->mLayer->setRouteVisible(show, route_id);
}

void GisViewer::setRoutePointVisible(bool show, const long long &route_id /*= 0*/)
{
	return mPtr->mLayer->setRoutePointVisible(show, route_id);
}

void GisViewer::setEntityVisible(bool show, const long long &entity_id)
{
	return mPtr->mLayer->setEntityVisible(show, entity_id);
}

void GisViewer::setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType)
{
	return mPtr->mLayer->setEntityAbilityCircleVisible(show, entity_id, circleType);
}

void GisViewer::setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType)
{
	return mPtr->mLayer->setEntityAbilityCircleScanVisible(show, entity_id, circleType);
}

void GisViewer::setEntityScutcheonVisible(bool show, const long long & entity_id /*= 0*/)
{
	return mPtr->mLayer->setEntityScutcheonVisible(show, entity_id);
}

void GisViewer::setCurrentSimulationTime(double simSec)
{
	GisStatus::instance().setCurrentSimulationTime(simSec);
}

void GisViewer::setSimStatus(bool mode)
{
	if (mode)
	{
		GisStatus::instance().setGisMode(SimRunMode);
	}
	else
	{
		GisStatus::instance().setGisMode(MoveMapMode);
	}
}

void GisViewer::setSvgDir(const std::string &dir)
{
	mPtr->mLayer->setSvgDir(dir);
}

bool GisViewer::addEntity(const EntityInfo &data)
{
	//同步到数据管理
	if (DataManager<EntityInfo>::instance().addData(data))
	{
		mPtr->mLayer->addEntity(data);
		return true;
	}
	return false;
}

bool GisViewer::addEntity(const std::vector<EntityInfo> &data)
{
	if (data.empty())
		return false;

	if (DataManager<EntityInfo>::instance().addData(data))
	{
		mPtr->mLayer->addEntity(data);

		return true;
	}

	return false;
}

bool GisViewer::removeEntity(const long long &id)
{
	//同步到数据管理
	if (DataManager<EntityInfo>::instance().removeData(id))
	{
		//删除能力圈里面的数据
		AbilityCircleDataManager::instance().removeEntity(id);

		mPtr->mLayer->removeEntity(id);
		return true;
	}
	return false;
}

int GisViewer::removeEntity(const std::vector<long long> &ids)
{
	//同步到数据管理
	int num = DataManager<EntityInfo>::instance().removeData(ids);
	if (num)
	{
		//删除能力圈里面的数据
		AbilityCircleDataManager::instance().removeEntity(ids);

		mPtr->mLayer->removeEntity(ids);
	}
	return num;
}

bool GisViewer::updateEntity(const EntityInfo &data)
{
	//同步到数据管理
	if (DataManager<EntityInfo>::instance().updateData(data))
	{
		//同步到图层
		mPtr->mLayer->updateEntity(data);

		return true;
	}
	return false;
}

int GisViewer::updateEntity(const std::vector<EntityInfo> &data)
{
	//同步到数据管理
	int num = DataManager<EntityInfo>::instance().updateData(data);
	
	if (num)
	{
		//同步到图层
		mPtr->mLayer->updateEntity(data);
	}
	
	return num;
}

void GisViewer::updateEntity(const std::unordered_map<long long, EntityInfo> &data)
{
	//同步到数据管理
	DataManager<EntityInfo>::instance().updateData(data);

	//同步到图层
	mPtr->mLayer->updateEntity(data);
}

bool GisViewer::getEntity(const long long &id, EntityInfo &data)
{
	return DataManager<EntityInfo>::instance().getData(id, data);
}

void GisViewer::getEntity(const std::vector<long long> &id, std::unordered_map<long long, EntityInfo> &data)
{
	return DataManager<EntityInfo>::instance().getData(id, data);
}

std::unordered_map<long long, EntityInfo> GisViewer::getAllEntity()
{
	return DataManager<EntityInfo>::instance().getAllData();
}

void GisViewer::setCircleParam(long long entity_id, const AbilityCircleParam &param)
{
	// 将能力圈数据存储
	AbilityCircleDataManager::instance().addCircleParamData(entity_id, param);
	mPtr->mLayer->setCircleParam(entity_id, param);
}

void GisViewer::removeCircle(long long entity_id, const std::string &circleType)
{
	// 将能力圈数据存储
	AbilityCircleDataManager::instance().removeCircle(entity_id, circleType);

	mPtr->mLayer->removeCircle(entity_id, circleType);
}

void GisViewer::setScanParam(long long entity_id, const ScanParam &param)
{
	// 将能力圈数据存储
	AbilityCircleDataManager::instance().updateScanParam(entity_id, param);

	mPtr->mLayer->setScanParam(entity_id, param);
}

bool GisViewer::getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData)
{
	return AbilityCircleDataManager::instance().getEntityAbilityCircle(entity_id, acData);
}

std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> GisViewer::getAbilityCircleData()
{
	return AbilityCircleDataManager::instance().getAbilityCircleData();
}

bool GisViewer::addRoute(const RouteInfo &data)
{
	if (data.points.size() < 2)
		return false;

	//添加到数据管理
	if (DataManager<RouteInfo>::instance().addData(data))
	{
		//添加到图层
		mPtr->mLayer->addRoute(data);

		return true;
	}
	return false;
}

bool GisViewer::addRoute(const std::vector<RouteInfo> &data)
{
	if (data.empty())
		return false;

	if (DataManager<RouteInfo>::instance().addData(data))
	{
		mPtr->mLayer->addRoute(data);
		return false;
	}

	//for (auto & route : data)
	//{
	//	
	//}

	return false;
}

bool GisViewer::removeRoute(const long long &id)
{
	//同步到数据管理
	if (DataManager<RouteInfo>::instance().removeData(id))
	{
		//同步到图层
		mPtr->mLayer->removeRoute(id);

		return true;
	}
	return false;
}

int GisViewer::removeRoute(const std::vector<long long> &ids)
{
	//同步到数据管理
	int num = DataManager<RouteInfo>::instance().removeData(ids);

	//同步到图层
	if (num)
	{
		mPtr->mLayer->removeRoute(ids);
	}

	return num;
}

bool GisViewer::updateRoute(const RouteInfo &data)
{
	//同步到数据管理
	if (DataManager<RouteInfo>::instance().updateData(data))
	{
		//同步到图层
		mPtr->mLayer->updateRoute(data);

		return true;
	}
	return false;
}

int GisViewer::updateRoute(const std::vector<RouteInfo> &data)
{
	//同步到数据管理
	int num = DataManager<RouteInfo>::instance().updateData(data);

	if (num)
	{
		//同步到图层
		mPtr->mLayer->updateRoute(data);
	}

	return num;
}

bool GisViewer::getRoute(const long long &id, RouteInfo &data)
{
	return DataManager<RouteInfo>::instance().getData(id, data);
}

void GisViewer::getRoute(const std::vector<long long> &id, std::unordered_map<long long, RouteInfo> &data)
{
	return DataManager<RouteInfo>::instance().getData(id, data);
}

std::unordered_map<long long, RouteInfo> GisViewer::getAllRoute()
{
	return DataManager<RouteInfo>::instance().getAllData();
}

void GisViewer::resizeEvent(QResizeEvent * event)
{
	scene()->setSceneRect(this->rect());
	mPtr->mLonlatLabel->updateLabelPos();
	QGraphicsView::resizeEvent(event);
}

void GisViewer::mouseMoveEvent(QMouseEvent *event)
{
	static int i = 0;
	++i;
	if (i > 3)
	{
		QPointF lonlat = mPtr->mLayer->getLonLatFromScenePos(event->screenPos());
		mPtr->mLonlatLabel->setLonLatInfo(GisStatus::instance().getGisLevel(), lonlat.x(), lonlat.y());
		i = 0;
	}

	//std::cout << "screen width:" << this->widthMM() << std::endl;

	QGraphicsView::mouseMoveEvent(event);
}

void GisViewer::wheelEvent(QWheelEvent *event)
{
	static int i = 0;
	if (++i < 2)
		return;
	i = 0;

	//场景中的坐标
	QPointF preScenePos = this->mapToScene(event->pos());

	//计算鼠标位置的经纬度
	QPointF lonLat = mPtr->mLayer->getLonLatFromScenePos(preScenePos);

	//缩放
	zoom(event->delta());

	QPointF postScenePos = mPtr->mLayer->getScenePosFromLonLat(lonLat);

	mPtr->mLayer->moveBy(preScenePos.x() - postScenePos.x(), preScenePos.y() - postScenePos.y());

	//更新标签
	mPtr->mLonlatLabel->setLonLatInfo(GisStatus::instance().getGisLevel(), lonLat.x(), lonLat.y());
}

void GisViewer::zoom(int delta)
{
	mPtr->mLayer->zoom(delta);
}

void GisViewer::initUi()
{
	//隐藏滚动条
	this->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	this->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

	//经纬度标签
	mPtr->mLonlatLabel = new MouseLonLatItem;
	scene()->addItem(mPtr->mLonlatLabel);

	//计算当前屏幕1像素 == ?米
	int t = QApplication::desktop()->screen()->heightMM();//381 mm
	double mi = t / 1000.0 / QApplication::desktop()->screen()->height();
	mPtr->mLonlatLabel->setPixelMapScaleSize(mi);

}

void GisViewer::initSigAndSlt()
{

}
