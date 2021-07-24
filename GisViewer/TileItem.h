#pragma once

#include <QGraphicsItem>
#include "LoadTileThread.h"

class TileItem : public QGraphicsItem
{
public:
	TileItem(QGraphicsItem *parent, const TileInfo &tile);
	~TileItem();

	inline const TileInfo & getTileInfo() const { return tileInfo; }

	//包围盒 256*256
	QRectF boundingRect() const;

	//void setImage(QImage image);

protected:
	//绘制单张瓦片
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

private:
	TileInfo tileInfo;
};
