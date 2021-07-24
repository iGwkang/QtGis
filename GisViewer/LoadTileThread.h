#pragma once
#include <QThread>
#include <QMutex>
#include <QRect>
#include <QImage>
#include <QWaitCondition>
#include <QReadWriteLock>
#include <unordered_map>
#include <atomic>

//单张瓦片信息
struct TileInfo
{
	explicit TileInfo() {}
	std::string key;
	int level;		//所在等级
	int tileX;
	int tileY;
	QImage image;	//image
};

/*
 * @brief 加载瓦片的线程，最终返回到瓦片层:MapItem
 */
class LoadTileThread : public QThread
{
	Q_OBJECT

	// 瓦片数据类型
	enum TileType
	{
		JPEG,
		MBTILES
	};

public:
	LoadTileThread();
	~LoadTileThread();

	//停止线程
	void stopThread();

	//设置当前视口矩形和地图等级
	void setCurrentRectAndLevel(const QRect &, int level);
	//绘制瓦片
	//bool drawTile(QPainter *painter, int level);

signals:
	//发送加载好的瓦片
	void loadedTileImage(int, QPoint, QImage);
	void sendLoadTile(int, QList<TileInfo>);

protected:
	void run();

private:
	//根据视口坐标和等级 计算需要加载的瓦片范围
	void calcTileScope(const QRect &r, QPoint &topLeftTile, QPoint &bottomRightTile);

	//加载瓦片
	//void loadTile(const QPoint &topLeftTile, const QPoint &bottomRightTile, int gisLevel, std::unordered_map<int, std::unordered_map<std::string, TileInfo>> &outImage);
	void loadTile(const QPoint &topLeftTile, const QPoint &bottomRightTile, int gisLevel, QList<TileInfo> &outImage);

	//获取上级瓦片
	bool getLevelTile(int & tileX, int & tileY, int & gisLevel, std::string &key, QImage &outImage);

	//合并加载好的瓦片
	//QImage mergeTileImage(const QRect & viewRect, int gisLevel, const std::unordered_map<int, std::unordered_map<std::string, TileInfo>> &);

	//释放加载瓦片使用的内存
	void freeTileMemory(int gisLevel);

private:
	std::atomic_bool isRun;// 线程是否运行

	//线程锁
	QMutex threadMutex;
	QWaitCondition condition;

	QReadWriteLock rwMutex;	//读写锁
	int currentGisLevel;	//当前地图等级
	QRect mRect;	//当前处理的视口(瓦片范围)

	// 最终给主线程调用的
	//QMutex drawLock;
	//DrawImage image;

	//加载好的瓦片
	std::unordered_map<std::string, QImage> loadedImage;

	TileType tileType;		// 瓦片数据类型
	std::string mTilePath;	// 瓦片路径
};
