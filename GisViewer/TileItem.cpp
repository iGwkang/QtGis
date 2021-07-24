#include "TileItem.h"
#include <QPainter>


TileItem::TileItem(QGraphicsItem *parent, const TileInfo &tile)
	: QGraphicsItem(parent), tileInfo(tile)
{
	setZValue(tileInfo.level);
}

TileItem::~TileItem()
{
}

QRectF TileItem::boundingRect() const
{
	return QRectF(0, 0, 256, 256);
}

// void TileItem::setImage(QImage image)
// {
// 	tileInfo.image = image;
// }

void TileItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= nullptr*/)
{
	//painter->drawRect(image.rect());
	painter->drawImage(0, 0, tileInfo.image);
}
