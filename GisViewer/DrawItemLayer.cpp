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
	//������ 
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

		//�������ʾ ���� �ǲ��ģʽ
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
		//���ģʽ
		if (measureMode)
		{
			if (!currentTestDist)
			{
				currentTestDist = new TestDistance(this);
				setAcceptHoverEvents(true);
			}

			//�������ʾ��ʱ�� ����Ӧ ��ӵ���¼�
			if (currentTestDist->isVisible())
			{
				QPointF pixel = event->pos();

				//��������ת��γ��
				TileSystem::PixelXYToLonLat(pixel.x(), pixel.y(), GisStatus::instance().getGisLevel(), pixel.rx(), pixel.ry());

				//��Ӿ�γ��
				currentTestDist->addLonLatPoint(pixel, GisStatus::instance().getGisLevel());

				//update();
			}
		}
	}
	//�Ҽ��˳����
	else if (event->button() == Qt::RightButton)
	{
		//�������ʾ��ʱ�� ����Ӧ �¼�
		if (currentTestDist && currentTestDist->isVisible() && measureMode)
		{
			//�˳���� �����ƶ���ͼģʽ
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
		//����ڻ��ƺ��� ���˳�����
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
