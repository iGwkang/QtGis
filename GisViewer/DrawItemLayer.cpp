#include "DrawItemLayer.h"
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsScene>
#include <QPainter>
#include "TestDistance.h"
#include "GeoToQtMath.h"
#include "TileSystem.h"
#include "GisStatus.h"
#include "GisSignals.h"
#include <QKeyEvent>

DrawItemLayer::DrawItemLayer(QGraphicsItem * parent)
	: QGraphicsObject(parent), measureMode(false), currentTestDist(nullptr)
{

}

DrawItemLayer::~DrawItemLayer()
{
	
}

void DrawItemLayer::clearData()
{
	delete currentTestDist;
	currentTestDist = nullptr;
	measureMode = false;
}

void DrawItemLayer::setMeasureMode(bool mode)
{
	//进入测距 
	if (mode)
	{
		delete currentTestDist;
		currentTestDist = nullptr;
	}
	else
	{
		setAcceptHoverEvents(false);
		if (currentTestDist && currentTestDist->getPointNum() < 2)
		{
			delete currentTestDist;
			currentTestDist = nullptr;
		}
		if (measureMode)
		{
			emit GisSignals::instance()->finishMeasureMode();
		}
	}

	measureMode = mode;
}

void DrawItemLayer::setMeasureVisible(bool show)
{
	if (currentTestDist)
	{
		currentTestDist->setVisible(show);

		//如果是显示 并且 是测距模式
		if (show && measureMode)
		{
			setAcceptHoverEvents(true);
		}
		else
		{
			setAcceptHoverEvents(false);
		}
	}
}

QRectF DrawItemLayer::boundingRect() const
{
	return parentItem()->boundingRect();
}

void DrawItemLayer::updateLevel()
{
	if (currentTestDist)
	{
		currentTestDist->updateLevel();
	}
}

void DrawItemLayer::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	//painter->drawRect(boundingRect());
	if (currentTestDist && measureMode && currentTestDist->isVisible())
	{
		QPen pen(Qt::red);
		pen.setWidth(2);
		pen.setStyle(Qt::DotLine);

		painter->setPen(pen);
		painter->setRenderHint(QPainter::Antialiasing);

		//QPoint pixel;
		//TileSystem::LonLatToPixelXY(mouseMoveLonlatPos.x(), mouseMoveLonlatPos.y(), GisStatus::instance().getGisLevel(), pixel.rx(), pixel.ry());

		//painter->drawLine(currentTestDist->getBackPixelPos(), pixel);

		std::vector<GeoPoint> inVec;
		inVec.push_back(GeoPoint(mouseMoveLonlatPos.x(), mouseMoveLonlatPos.y()));
		inVec.push_back(currentTestDist->getBackGeoPos());

		painter->drawPath(GeoToQtMath::getInterpolatPath(inVec, 20000, GisStatus::instance().getGisLevel()));
	}
}

void DrawItemLayer::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		//测距模式
		if (measureMode)
		{
			if (!currentTestDist)
			{
				currentTestDist = new TestDistance(this);
				setAcceptHoverEvents(true);
			}

			//当测距显示的时候 才响应 添加点的事件
			if (currentTestDist->isVisible())
			{
				QPointF pixel = event->pos();

				//像素坐标转经纬度
				TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), pixel.rx(), pixel.ry());

				//添加经纬点
				currentTestDist->addLonLatPoint(pixel, GisStatus::instance().getGisLevel());

				//update();
			}
		}
	}
	//右键退出测距
	else if (event->button() == Qt::RightButton)
	{
		//当测距显示的时候 才响应 事件
		if (currentTestDist && currentTestDist->isVisible() && measureMode)
		{
			//退出测距 进入移动地图模式
			GisStatus::instance().setGisMode(MoveMapMode);
		}
	}
	
}

void DrawItemLayer::hoverMoveEvent(QGraphicsSceneHoverEvent *event)
{
	QPoint pixel = event->pos().toPoint();
	TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), mouseMoveLonlatPos.rx(), mouseMoveLonlatPos.ry());
	//update();
	QGraphicsObject::hoverMoveEvent(event);
}

void DrawItemLayer::keyReleaseEvent(QKeyEvent *event)
{
	switch (event->key())
	{
	case Qt::Key_Escape:
		//如果在绘制航线 则退出绘制
		if (measureMode)
		{
			delete currentTestDist;
			currentTestDist = nullptr;
			GisStatus::instance().setGisMode(MoveMapMode);
			emit GisSignals::instance()->finishMeasureMode();
		}

		break;

	default:
		break;
	}
}
