#include "MapItem.h"
#include "LoadTileThread.h"
#include "TileSystem.h"
#include "ReadGisConfig.h"
#include "GisStatus.h"

#include <QPainter>
#include <iostream>
#include <QGraphicsScene>
#include <algorithm>


MapItem::MapItem(QGraphicsItem * parent)
	: QGraphicsObject(parent), timer(this), mLoadTileThread(new LoadTileThread)
{
	realLevel = gisLevel = ReadGisConfig::instance().getInitLevel();

	mBoundingRect = QRectF(0, 0, 256 << realLevel, 256 << realLevel);
	//初始化信号槽
	initSigAndSlt();

	//启动线程
	mLoadTileThread->start();
	timer.start(500);
}

MapItem::~MapItem()
{
	//退出线程
	mLoadTileThread->stopThread();
	mLoadTileThread->wait();
	mLoadTileThread->quit();
	//mLoadTileThread->deleteLater();
	delete mLoadTileThread;// ->deleteLater();
	mLoadTileThread = nullptr;
}

QRectF MapItem::boundingRect() const
{
	return mBoundingRect;
}

void MapItem::zoomTileLevel(int delta)
{
	//判断是否超出范围

	if (delta > 0)	//放大
	{
		//瓦片等级
		//if (realLevel >= ReadGisConfig::instance().getGisMaxLevel() || scale() < 1)
		//{
		//	setScale(scale() * 2);
		//	return;
		//}
		//地图等级++
		realLevel++;
		mBoundingRect = QRectF(0, 0, 256 << realLevel, 256 << realLevel);

		isZoomOut = false;
		for (auto &item : levelTile)
		{
			//if (item.first.length() == gisLevel || item.second->scale() != 1.0)
			if (item.second->isVisible())
			{
				auto pos = item.second->pos();
				item.second->setPos(pos * 2);
				item.second->setScale(item.second->scale() * 2);
			}
		}
	}
	else
	{
		//缩小
		//if (realLevel <= ReadGisConfig::instance().getGisMinLevel() || scale() > 1)
		//{
		//	setScale(scale() / 2);
		//	return;
		//}

		//地图等级++
		realLevel--;
		mBoundingRect = QRectF(0, 0, 256 << realLevel, 256 << realLevel);

		isZoomOut = true;
		for (auto &item : levelTile)
		{
			//if (item.first.length() == gisLevel || item.second->scale() != 1.0)
			if (item.second->isVisible())
			{
				auto pos = item.second->pos();
				item.second->setPos(pos / 2);
				item.second->setScale(item.second->scale() / 2);
				//item.second->show();
			}
			else
			{
				//item.second->hide();
			}
		}
	}

	/*
	//如果等级不同 需要缩放图片 并且计算坐标
	if (gisLevel > level)	//缩小
	{
		isZoomOut = true;
		for (auto &item : levelTile)
		{
			if (item.first.length() == gisLevel)
			{
				auto pos = item.second->pos();
				item.second->setPos(pos / 2);
				item.second->setScale(item.second->scale() / 2);
				item.second->show();
			}
		}
	}
	else if (gisLevel < level)	//放大
	{
		isZoomOut = false;
		for (auto &item : levelTile)
		{
			if (item.first.length() == gisLevel)
			{
				auto pos = item.second->pos();
				item.second->setPos(pos * 2);
				//pos = item.second->pos();
				//auto scale = item.second->scale();
				item.second->setScale(item.second->scale() * 2);
				//scale = item.second->scale();
				//item.second->setZValue(level);
				item.second->show();
			}
		}
	}

	realLevel = level;
	*/

	//gisLevel = level;
	//std::cout << "gisLevel:" << gisLevel << std::endl;
}

void MapItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr*/)
{
	//painter->drawRect(boundingRect());

	//if (!mLoadTileThread->drawTile(painter, gisLevel))
	{
		//if (!mCurrentDrawTile.image.isNull())
		//	painter->drawImage(mCurrentDrawTile.pos, mCurrentDrawTile.image);
	}

	// 	if (!levelTile[gisLevel].image.isNull())
	// 	{
	// 		painter->drawImage(levelTile[gisLevel].pos, levelTile[gisLevel].image);
	// 	}
		//if (!mCurrentDrawTile.image.isNull())
		//	painter->drawImage(mCurrentDrawTile.pos, mCurrentDrawTile.image);

}

void MapItem::loadTimeOutSlot()
{
	timeoutUpdateTile(realLevel);
}


void MapItem::initSigAndSlt()
{
	//定时器加载瓦片
	if (!connect(&timer, SIGNAL(timeout()), this, SLOT(loadTimeOutSlot())))
	{
		std::cout << __FUNCTION__ << "  line:" << __LINE__ << " 信号连接失败！ " << std::endl;
	}

	//线程加载完瓦片之后返回
	qRegisterMetaType<QList<TileInfo>>("QList<TileInfo>");
	if (!connect(mLoadTileThread, SIGNAL(sendLoadTile(int, QList<TileInfo>)), this, SLOT(updateTile(int, QList<TileInfo>))))
	{
		std::cout << __FUNCTION__ << "  line:" << __LINE__ << " 信号连接失败！ " << std::endl;
	}
}

void MapItem::timeoutUpdateTile(int level)
{
	//计算视口范围
	QPointF tpLeft = this->mapFromScene(QPointF(0, 0));
	tpLeft.rx() = std::max(0, int(tpLeft.x()));
	tpLeft.ry() = std::max(0, int(tpLeft.y()));

	QPointF btRight = this->mapFromScene(scene()->sceneRect().bottomRight());
	btRight.rx() = std::min(boundingRect().width(), btRight.x());
	btRight.ry() = std::min(boundingRect().width(), btRight.y());

	mLoadTileThread->setCurrentRectAndLevel(QRect(tpLeft.toPoint(), btRight.toPoint()), level);
}

void MapItem::updateTile(int level, QList<TileInfo> tile)
{
	if (realLevel != level)
		return;
	gisLevel = level;
	for (auto & t : tile)
	{
		//TileItem * item = nullptr;
		if (levelTile.find(t.key) == levelTile.end())
		{
			auto item = new TileItem(this, t);
			levelTile[t.key] = item;
			item->setVisible(false);
		}
	}

	puzzle();
}

void MapItem::puzzle()
{
	//计算视口中需要的item
	QPointF tpLeft = this->mapFromScene(QPointF(0, 0));
	tpLeft.rx() = std::max(0, int(tpLeft.x()));
	tpLeft.ry() = std::max(0, int(tpLeft.y()));

	QPointF btRight = this->mapFromScene(scene()->sceneRect().bottomRight());
	btRight.rx() = std::min(boundingRect().width(), btRight.x());
	btRight.ry() = std::min(boundingRect().width(), btRight.y());

	QPoint topLeftTile, bottomRightTile;
	//像素坐标转瓦片坐标
	TileSystem::PixelXYToTileXY(tpLeft.x(), tpLeft.y(), topLeftTile.rx(), topLeftTile.ry());
	TileSystem::PixelXYToTileXY(btRight.x(), btRight.y(), bottomRightTile.rx(), bottomRightTile.ry());

	int tileNum = 1 << gisLevel;

	int minTileX = std::max(0, topLeftTile.x());
	int maxTileX = std::min(tileNum - 1, bottomRightTile.x());

	int minTileY = tileNum - 1 - std::min(tileNum - 1, bottomRightTile.y());
	int maxTileY = tileNum - 1 - std::max(0, topLeftTile.y());

	//视口上的瓦片
	for (int tileX = minTileX; tileX <= maxTileX; ++tileX)
	{
		for (int tileY = minTileY; tileY <= maxTileY; ++tileY)
		{
			std::string key = TileSystem::TileXYToQuadKey(tileX, tileY, gisLevel);

			//查找当前已经存在的item中是否已经存在要显示的瓦片
			auto _find = levelTile.find(key);
			if (_find != levelTile.end())
			{
				auto item = _find->second;
				QPoint pos;
				TileSystem::TileXYToPixelXY(tileX, tileNum - 1 - tileY, pos.rx(), pos.ry());

				item->setPos(pos);
				item->setScale(1);
				item->setVisible(true);
			}
			else
			{
				//寻找上级瓦片  等级-1, tileX/2, tileY/2
				int upLevel = gisLevel;
				int upTileX = tileX;
				int upTileY = tileY;
				if (auto _item = findUpLevelItem(upLevel, upTileX, upTileY))
				{
					QPoint pos;	//这张瓦片原来的坐标
					TileSystem::TileXYToPixelXY(upTileX, (1 << upLevel) - 1 - upTileY, pos.rx(), pos.ry());

					int _scale = 1 << (gisLevel - upLevel);

					_item->setPos(pos * _scale);
					_item->setScale(_scale);
					_item->setVisible(true);
				}

			}
		}
	}


	//如果是缩小 释放上级瓦片
	if (isZoomOut)
		freeUpLevelTile();


	//释放内存
	freeTileMemory();


	isZoomOut = false;

	/*for (auto _iter = levelTile.begin(); _iter != levelTile.end(); )
	{

	}*/
	/*
	int gisWidth = 256 << gisLevel;

	for (auto _iter = levelTile.begin(); _iter != levelTile.end(); )
	{
		if (std::abs(int(_iter->first.length()) - gisLevel) > 2)
		{
			delete _iter->second;
			_iter = levelTile.erase(_iter);
			//++_iter;
		}
		else if(_iter->first.length() != gisLevel)
		{
			//_iter->second->setScale(1);
			_iter->second->hide();
			++_iter;
		}
		else
		{
			_iter->second->show();
			_iter->second->setScale(1);

			auto tileinfo = _iter->second->getTileInfo();

			QPoint pixPos;// (tileinfo.tileX * (1 << gisLevel), tileinfo.tileY * (1 << gisLevel));
			CoordinateTransform::TileXYToPixelXY(tileinfo.tileX, tileinfo.tileX, pixPos.rx(), pixPos.ry());

			pixPos.ry() = gisWidth - pixPos.ry() - 256;

			_iter->second->setPos(pixPos);

			++_iter;
		}
	}
	*/
	// 	QPointF tpLeft = this->mapFromScene(QPointF(0, 0));
	// 	tpLeft.rx() = std::max(0, int(tpLeft.x()));
	// 	tpLeft.ry() = std::max(0, int(tpLeft.y()));
	// 
	// 	QPointF btRight = this->mapFromScene(scene()->sceneRect().bottomRight());
	// 	btRight.rx() = std::min(boundingRect().width(), btRight.x());
	// 	btRight.ry() = std::min(boundingRect().width(), btRight.y());
}

TileItem * MapItem::findUpLevelItem(int &level, int &tileX, int &tileY)
{
	if (level <= ReadGisConfig::instance().getGisMinLevel())
	{
		return nullptr;
	}

	level--;
	tileX /= 2;
	tileY /= 2;

	std::string key = TileSystem::TileXYToQuadKey(tileX, tileY, level);

	//如果存在 直接返回
	auto _find = levelTile.find(key);
	if (_find != levelTile.end())
	{
		return _find->second;
	}
	else
	{
		//如果不存在 继续往上级查找
		return findUpLevelItem(level, tileX, tileY);
	}
}

void MapItem::freeUpLevelTile()
{
	for (auto _iter = levelTile.begin(); _iter != levelTile.end(); )
	{
		auto &tile = _iter->second->getTileInfo();
		//缩小 释放等级高的瓦片
		if (tile.level > gisLevel)
		{
			delete _iter->second;
			_iter = levelTile.erase(_iter);
		}
		else
		{
			++_iter;
		}
	}
}

void MapItem::freeTileMemory()
{
	int i = 0;	//避免无法退出循环 用了一个计数器 最多循环10次
	while (levelTile.size() > 200)
	{
		auto _iter = levelTile.begin();
		if (!_iter->second->isVisible() || !scene()->sceneRect().intersects(_iter->second->mapToScene(_iter->second->boundingRect()).boundingRect()))	//判断item 是否在视口
		{
			//释放距离视口远的瓦片
			delete _iter->second;
			levelTile.erase(_iter);
		}
		if (++i > 10)
			return;
	}
}
