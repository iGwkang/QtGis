#pragma once

#include <QGraphicsObject>
#include <QTimer>
#include "TileItem.h"
#include "LoadTileThread.h"

class LoadTileThread;

/**
 * @brief 瓦片层。地图的底图，瓦片调度，缩放，加载瓦片都通过这个类去调用
 */
class MapItem : public QGraphicsObject
{
	Q_OBJECT

	// 	struct TileImage
	// 	{
	// 		int level;
	// 		QPoint pos;
	// 		QImage image;
	// 	};

public:
	MapItem(QGraphicsItem * parent);
	~MapItem();

	QRectF boundingRect() const;

	//改变地图瓦片等级
	void zoomTileLevel(int delta);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

private slots:
	//定时器
	void loadTimeOutSlot();

	//线程加载完瓦片之后发送给地图层
	void updateTile(int level, QList<TileInfo>);

private:
	//初始化信号槽
	void initSigAndSlt();

	//定时加载瓦片
	void timeoutUpdateTile(int level);

	//拼接瓦片，并且释放不需要的资源
	void puzzle();

	//如果瓦片缺失， 查找上级瓦片
	TileItem *findUpLevelItem(int &level, int &tileX, int &tileY);

	//释放等级大的瓦片
	void freeUpLevelTile();

	//释放不在视口中的瓦片
	void freeTileMemory();

private:
	QRectF mBoundingRect;
	//加载瓦片的线程
	LoadTileThread *mLoadTileThread;
	//当前绘制的瓦片
	//TileImage mCurrentDrawTile;
	std::unordered_map<std::string, TileItem *> levelTile;
	bool isZoomOut;

	//缩放时异步，最终同步
	int gisLevel;	//当前显示的地图等级
	int realLevel;	//真实地图等级

	QTimer timer;	//定时器 500ms刷新刷新一次底图
};
