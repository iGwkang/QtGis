#include "LoadTileThread.h"
#include "TileSystem.h"
#include "ReadGisConfig.h"
#include "LoadTileFromMbtiles.h"
#include <iostream>
#include <unordered_set>

#include <QPainter>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QEventLoop>

LoadTileThread::LoadTileThread()
	: isRun(true), mTilePath(ReadGisConfig::instance().getTilePath().c_str()), currentGisLevel(ReadGisConfig::instance().getInitLevel())
{
	if (mTilePath.endsWith(".mbtiles", Qt::CaseInsensitive))
	{
		tileType = LoadTileThread::MBTILES;
		LoadTileFromMbtiles::instance().setTileFilePath(mTilePath.toStdString());
	}
	else if (mTilePath.startsWith("http", Qt::CaseInsensitive))
	{
		tileType = LoadTileThread::NETWORK;
	}
	else
	{
		tileType = LoadTileThread::IMAGE;
	}
}

LoadTileThread::~LoadTileThread()
{

}

void LoadTileThread::stopThread()
{
	isRun = false;
	condition.wakeAll();
}

//设置当前地图视口矩形和地图等级
void LoadTileThread::setCurrentRectAndLevel(const QRect &rect, int level)
{
	rwMutex.lockForWrite();

	currentGisLevel = level;
	mRect = rect;
	rwMutex.unlock();

	condition.wakeAll();
}
/*
bool LoadTileThread::drawTile(QPainter *painter, int level)
{
	if (level != image.level)
		return false;
	drawLock.lock();

	painter->drawImage(image.pos, image.image);

	drawLock.unlock();
	return true;
}
*/
void LoadTileThread::run()
{
	while (isRun)
	{
		//long st = clock();
		//读锁
		rwMutex.lockForRead();
		int gisLevel = currentGisLevel;
		QRect mViewRect = mRect;
		rwMutex.unlock();

		QPoint topLeftTile, bottomRightTile;
		//计算需要加载哪些瓦片
		calcTileScope(mViewRect, topLeftTile, bottomRightTile);

		//加载瓦片
		//std::unordered_map<int, std::unordered_map<std::string, TileInfo>> outImage;	// key:地图等级 value:每一级的瓦片
		QList<TileInfo> outImage;
		loadTile(topLeftTile, bottomRightTile, gisLevel, outImage);

		//发送图片
		if (isRun && !outImage.empty())
			emit sendLoadTile(gisLevel, outImage);

		//emit loadedTileImage(image.level, image.pos, image.image);
		freeTileMemory(gisLevel);

		//long cpu_use_time = clock() - st;
		//std::cout << "cpu use time:" << cpu_use_time << std::endl;
		//int sleepTime = 1000 - cpu_use_time;
		//if (sleepTime > 0)
		if (isRun)
		{
			QMutexLocker lock(&threadMutex);
			condition.wait(&threadMutex, 1000);
		}
	}

}

//计算瓦片范围
void LoadTileThread::calcTileScope(const QRect &r, QPoint &topLeftTile, QPoint &bottomRightTile)
{
	//像素坐标转瓦片坐标
	TileSystem::PixelXYToTileXY(r.topLeft().x(), r.topLeft().y(), topLeftTile.rx(), topLeftTile.ry());
	TileSystem::PixelXYToTileXY(r.bottomRight().x(), r.bottomRight().y(), bottomRightTile.rx(), bottomRightTile.ry());

}

void LoadTileThread::loadTile(const QPoint &topLeftTile, const QPoint &bottomRightTile, int gisLevel, QList<TileInfo> &outImage)
{
	int tileNum = (1 << gisLevel);

	int minTileX = std::max(0, topLeftTile.x());
	int maxTileX = std::min(tileNum - 1, bottomRightTile.x());

	static const int yOrtginal = ReadGisConfig::instance().getTileYOriginal();

	int minTileY, maxTileY;
	if (yOrtginal == 2)
	{
		minTileY = tileNum - 1 - std::min(tileNum - 1, bottomRightTile.y());
		maxTileY = tileNum - 1 - std::max(0, topLeftTile.y());
	}
	else
	{
		minTileY = std::max(0, topLeftTile.y());
		maxTileY = std::min(tileNum - 1, bottomRightTile.y());
	}


	//  ----> x
	// y
	// ^
	// |
	std::unordered_set<std::string> keys;
	for (int tileX = minTileX; tileX <= maxTileX; ++tileX)
	{
		for (int tileY = minTileY; tileY <= maxTileY; ++tileY)
		{
			QImage image;
			int level = gisLevel;
			int tempTileX = tileX;
			int tempTileY = tileY;
			std::string key;
			if (getLevelTile(tempTileX, tempTileY, level, key, image) && keys.find(key) == keys.end())
			{
				TileInfo tile;
				tile.level = level;
				tile.tileX = tempTileX;
				tile.tileY = tempTileY;
				tile.image = image;
				tile.key = key;
				keys.insert(key);
				outImage.push_back(tile);
			}
		}
	}

	freeTileMemory(gisLevel);
}

#if 0
void LoadTileThread::loadTile(const QPoint &topLeftTile, const QPoint &bottomRightTile, int gisLevel, std::unordered_map<int, std::unordered_map<std::string, TileInfo>> &outImage)
{
	int minTileX = std::max(0, topLeftTile.x());
	int maxTileX = std::min((1 << gisLevel) - 1, bottomRightTile.x());

	int maxTileY = (1 << gisLevel) + 1 - std::max(0, topLeftTile.y());
	int minTileY = (1 << gisLevel) - 1 - std::min((1 << gisLevel) - 1, bottomRightTile.y());

	//  ----> x
	// y
	// ^
	// |
	for (int tileX = minTileX; tileX <= maxTileX; ++tileX)
	{
		for (int tileY = minTileY; tileY <= maxTileY; ++tileY)
		{
			QImage image;
			int level = gisLevel;
			int tempTileX = tileX;
			int tempTileY = tileY;
			std::string key;
			if (getLevelTile(tempTileX, tempTileY, level, key, image))
			{
				//如果加载的上级图片 需要缩放
				//if (gisLevel > level)
				//	image = image.scaled(1 << (gisLevel - level), 1 << (gisLevel - level));

				if (outImage[level].find(key) == outImage[level].end())
				{
					TileInfo tile;
					tile.level = level;
					tile.tileX = tempTileX;
					tile.tileY = tempTileY;
					tile.image = image;
					outImage[level][key] = tile;
				}
			}
		}
	}

	freeTileMemory(gisLevel);
}
#endif

bool LoadTileThread::getLevelTile(int & tileX, int & tileY, int & gisLevel, std::string &key, QImage &outImage)
{
	int minL = ReadGisConfig::instance().getGisMinLevel();
	if (gisLevel < minL)
		return false;
	if (LoadTileThread::IMAGE == tileType)
	{
		key = TileSystem::TileXYToQuadKey(tileX, tileY, gisLevel);

		//判断是否已经加载
		auto _find = loadedImage.find(key);
		if (_find != loadedImage.end())
		{
			outImage = _find->second;
			return true;
		}
		 //从文件中加载

		//瓦片路径
		QString tilePath = mTilePath + QString("/%1/%2/%3.%4").arg(gisLevel).arg(tileX).arg(tileY).arg(ReadGisConfig::instance().getTileFormat().c_str());

		QImage image;
		if (image.load(tilePath))
		{
			image = image.scaled(256, 256);
			outImage = image;
			loadedImage[key] = image;

			return true;
		}

		return getLevelTile(tileX /= 2, tileY /= 2, --gisLevel, key, outImage);
	}
	if (LoadTileThread::MBTILES == tileType)
	{
		if (LoadTileFromMbtiles::instance().getTile(gisLevel, tileX, tileY, outImage))
		{
			outImage = outImage.scaled(256, 256);
			key = TileSystem::TileXYToQuadKey(tileX, tileY, gisLevel);
			loadedImage[key] = outImage;
			return true;
		}

		else
		{
			return getLevelTile(tileX /= 2, tileY /= 2, --gisLevel, key, outImage);
		}
	}
	if (LoadTileThread::NETWORK == tileType)
	{
		key = TileSystem::TileXYToQuadKey(tileX, tileY, gisLevel);

		//判断是否已经加载
		auto _find = loadedImage.find(key);
		if (_find != loadedImage.end())
		{
			outImage = _find->second;
			return true;
		}

		//瓦片路径
		// QString tilePath = mTilePath.c_str() + QString("/%1/%2/%3.%4").arg(gisLevel).arg(tileX).arg(tileY).arg(ReadGisConfig::instance().getTileFormat().c_str());
		QString tileUrl = mTilePath;
		tileUrl.replace("{x}", QString::number(tileX));
		tileUrl.replace("{y}", QString::number(tileY));
		tileUrl.replace("{z}", QString::number(gisLevel));

		QNetworkAccessManager *httpMgr = new QNetworkAccessManager();
		QNetworkRequest requestInfo;
		requestInfo.setUrl(QUrl(tileUrl));

		//添加事件循环机制，返回后再运行后面的
		QEventLoop eventLoop;
		QNetworkReply *reply = httpMgr->get(requestInfo);
		connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
		eventLoop.exec();       //block until finish
		//错误处理
		if (reply->error() != QNetworkReply::NoError)
		{
			return false;
		}
		//请求返回的结果
		QByteArray responseByte = reply->readAll();

		delete reply;
		delete httpMgr;

		QImage image;
		if (image.loadFromData(responseByte))
		{
			image = image.scaled(256, 256);
			outImage = image;
			loadedImage[key] = image;

			return true;
		}

		return getLevelTile(tileX /= 2, tileY /= 2, --gisLevel, key, outImage);
	}
	else
	{
		return false;
	}

}
#if 0
QImage LoadTileThread::mergeTileImage(const QRect & viewRect, int gisLevel, const std::unordered_map<int, std::unordered_map<std::string, TileInfo>> & images)
{
	QPoint tpLeft = viewRect.topLeft();
	int tileNum = 1 << gisLevel;

	QImage image(viewRect.width(), viewRect.height(), QImage::Format_RGB32);
	QPainter painter(&image);
	//先画等级高的
	for (auto _iter = images.begin(); _iter != images.end(); ++_iter)
	{
		int level = _iter->first;
		int offsetXY = 1 << (gisLevel - level);
		for (auto _iter1 = _iter->second.begin(); _iter1 != _iter->second.end(); ++_iter1)
		{
			QPoint pos;
			CoordinateTransform::TileXYToPixelXY(_iter1->second.tileX, tileNum - 1 - _iter1->second.tileY, pos.rx(), pos.ry());

			pos.rx() *= offsetXY;
			pos.ry() *= offsetXY;

			/*pos.ry() = mapSize- pos.ry()-256;*/
			int iw = _iter1->second.image.width();
			int w = iw * offsetXY;
			//int w = _iter1->second.image.width() * offsetXY;

			QImage simage = _iter1->second.image.scaled(w, w);

			painter.drawImage(pos - tpLeft, simage);
		}
	}
	return image;
}
#endif

void LoadTileThread::freeTileMemory(int gisLevel)
{
	/*
	//如果加载的瓦片大于200张图 释放内存
	if (loadedImage.size() > 200)
	{
		//loadedImage.erase(loadedImage.begin() + 200, loadedImage.end())

		for (auto iter = loadedImage.begin(); iter != loadedImage.end(); )
		{
			int tileLevel = iter->first.length();
			if (tileLevel != gisLevel)
			{
				iter = loadedImage.erase(iter);
			}
			else
			{
				++iter;
			}
		}

	}
	*/

	while (loadedImage.size() > 300)
	{
		loadedImage.erase(loadedImage.begin());
	}
}
