#include "EntityLayer.h"
#include "DealEntityMsgThread.h"
#include "EntityItem.h"
#include "TileSystem.h"
#include <GisStruct.h>
#include "GisSignals.h"
#include "GisStatus.h"
#include "DataManager.hpp"
#include <QPainter>
#include <QCoreApplication>
#include <QGraphicsSceneDragDropEvent>
#include <QMimeData>
#include <sstream>

EntityLayer::EntityLayer(QGraphicsItem *parent)
	: QGraphicsObject(parent)
{
	// 接收鼠标拖放事件（添加实体）
	setAcceptDrops(true);

	mLoadThread = new DealEntityMsgThread;

	qRegisterMetaType<GisEntity>("GisEntity");
	qRegisterMetaType<AbilityCircleParam>("AbilityCircleParam");
	qRegisterMetaType<ScanParam>("ScanParam");
	qRegisterMetaType<std::string>("std::string");
	connect(mLoadThread, SIGNAL(sigNewEntity(GisEntity)), this, SLOT(sltNewEntity(GisEntity)));
	connect(mLoadThread, SIGNAL(sigRemoveEntity(long long)), this, SLOT(sltRemoveEntity(long long)));
	connect(mLoadThread, SIGNAL(sigUpdateEntity(GisEntity)), this, SLOT(sltUpdateEntity(GisEntity))/*, Qt::QueuedConnection*/);
	connect(mLoadThread, SIGNAL(sigNewCircle(long long, AbilityCircleParam)), this, SLOT(sltNewCircle(long long, AbilityCircleParam)));
	connect(mLoadThread, SIGNAL(sigRmCircle(long long, std::string)), this, SLOT(sltRmCircle(long long, std::string)));
	connect(mLoadThread, SIGNAL(sigNewScan(long long, ScanParam)), this, SLOT(sltNewScan(long long, ScanParam)));
	
	mLoadThread->start();
	mLoadThread->setPriority(QThread::HighestPriority);
}

EntityLayer::~EntityLayer()
{
	for (auto &iter : mAllEntity)
	{
		delete iter.second;
	}
	mAllEntity.clear();

	mLoadThread->stopThread();
	mLoadThread->wait();
	mLoadThread->quit();
	delete mLoadThread;
	//mLoadThread->deleteLater();
	mLoadThread = nullptr;
}

QRectF EntityLayer::boundingRect() const
{
	return parentItem()->boundingRect();
}

void EntityLayer::updateLevel()
{
	for (auto & _iter : mAllEntity)
	{
		_iter.second->updateLevel();
	}
	//update();
}

void EntityLayer::clearData()
{
	// 清空线程中的数据
	mLoadThread->clearData();

	// 线程中 可能已经有信号发出, 先把信号处理完，然后删除实体
	QCoreApplication::processEvents();

	for (auto & _iter : mAllEntity)
	{
		delete _iter.second;
	}
	mAllEntity.clear();

}

void EntityLayer::setEditMode(bool mode)
{
	// 设置实体是否可移动
	for (auto & _iter : mAllEntity)
	{
		_iter.second->setFlag(ItemIsSelectable, mode);
		_iter.second->setFlag(ItemIsMovable, mode);
	}
}

void EntityLayer::setEntityVisible(bool show, const long long &entity_id)
{
	if (entity_id == 0)	//控制所有实体
	{
		setVisible(show);
		//for (auto & _iter : mAllEntity)
		//{
		//	_iter.second->setVisible(show);
		//}
	}
	else
	{
		auto _find = mAllEntity.find(entity_id);
		if (_find != mAllEntity.end())
		{
			_find->second->setVisible(show);
		}
	}
}

void EntityLayer::setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType)
{
	auto _find = mAllEntity.find(entity_id);
	if (_find != mAllEntity.end())
	{
		_find->second->setAbilityCircleVisible(circleType, show);
	}
}

void EntityLayer::setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType)
{
	auto _find = mAllEntity.find(entity_id);
	if (_find != mAllEntity.end())
	{
		_find->second->setAbilityCircleScanVisible(circleType, show);
	}
}

void EntityLayer::setEntityScutcheonVisible(bool show, const long long & entity_id /*= 0*/)
{
	if (0 == entity_id)	//控制所有实体
	{
		for (auto & _iter : mAllEntity)
		{
			_iter.second->setScutcheonVisible(show);
		}
	}
	else
	{
		auto _find = mAllEntity.find(entity_id);
		if (_find != mAllEntity.end())
		{
			_find->second->setScutcheonVisible(show);
		}
	}
}

void EntityLayer::setSvgDir(const std::string &dir)
{
	mLoadThread->setSvgDir(dir);
}

void EntityLayer::addEntity(const EntityInfo &data)
{
	mLoadThread->addEntity(data);
}

void EntityLayer::addEntity(const std::vector<EntityInfo> &data)
{
	mLoadThread->addEntity(data);
}

void EntityLayer::removeEntity(const long long &id)
{
	mLoadThread->removeEntity(id);
}

void EntityLayer::removeEntity(const std::vector<long long> &id)
{
	mLoadThread->removeEntity(id);
}

void EntityLayer::updateEntity(const EntityInfo &data)
{
	mLoadThread->updateEntity(data);
}

void EntityLayer::updateEntity(const std::vector<EntityInfo> &data)
{
	mLoadThread->updateEntity(data);
}

void EntityLayer::updateEntity(const std::unordered_map<long long, EntityInfo> &data)
{
	mLoadThread->updateEntity(data);
}

void EntityLayer::setCircleParam(long long entity_id, const AbilityCircleParam & param)
{
	mLoadThread->setCircleParam(entity_id, param);
}

void EntityLayer::removeCircle(long long entity_id, const std::string & circleType)
{
	mLoadThread->removeCircle(entity_id, circleType);
}

void EntityLayer::setScanParam(long long entity_id, const ScanParam & param)
{
	mLoadThread->setScanParam(entity_id, param);
}

void EntityLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	//painter->fillRect(boundingRect(), QColor(0, 0, 0, 0));
}

void EntityLayer::dragEnterEvent(QGraphicsSceneDragDropEvent *event)
{
	event->setAccepted(SimRunMode != GisStatus::instance().getGisMode() && !event->mimeData()->data("EntityInfo").isEmpty());
}

void EntityLayer::dropEvent(QGraphicsSceneDragDropEvent *event)
{
	QByteArray data = event->mimeData()->data("EntityInfo");
	if (SimRunMode != GisStatus::instance().getGisMode() && !data.isEmpty())
	{
		// 转成EntityInfo数据
		EntityInfo entity;

		// 反序列化
		std::stringstream ss(data.toStdString());
		ss >> entity;
		if (entity.ID == 0)
		{
			return;
		}

		// 计算获取经纬高
		TileSystem::PixelXYToLonLat(event->pos().x(), event->pos().y(), GisStatus::instance().getGisLevel(), entity.position.longitude, entity.position.latitude);

		// 添加到数据管理
		DataManager<EntityInfo>::instance().addData(entity);

		// 添加到图层
		addEntity(entity);

		emit GisSignals::instance()->sigNewEntity(entity);
	}
}

void EntityLayer::sltNewEntity(GisEntity newEntity)
{
	if (mAllEntity.find(newEntity.realData.ID) != mAllEntity.end())
		return;

	mAllEntity[newEntity.realData.ID] = new EntityItem(this, newEntity);
}

void EntityLayer::sltRemoveEntity(long long id)
{
	auto _iter = mAllEntity.find(id);
	if (_iter == mAllEntity.end())
		return;

	delete _iter->second;
	mAllEntity.erase(_iter);
}

void EntityLayer::sltUpdateEntity(GisEntity newEntity)
{
	auto _iter = mAllEntity.find(newEntity.realData.ID);
	if (_iter == mAllEntity.end())
		return;

	_iter->second->updateEntity(newEntity);
}

void EntityLayer::sltNewCircle(long long entity_id, AbilityCircleParam param)
{
	auto _iter = mAllEntity.find(entity_id);
	if (_iter == mAllEntity.end())
		return;

	_iter->second->setCircleParam(param);
}

void EntityLayer::sltRmCircle(long long entity_id, std::string circleType)
{
	auto _iter = mAllEntity.find(entity_id);
	if (_iter == mAllEntity.end())
		return;

	_iter->second->removeCircle(circleType);
}

void EntityLayer::sltNewScan(long long entity_id, ScanParam param)
{
	auto _iter = mAllEntity.find(entity_id);
	if (_iter == mAllEntity.end())
		return;

	_iter->second->setScanParam(param);
}
