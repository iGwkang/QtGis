#include "MouseLonLatItem.h"
#include "TileSystem.h"
#include <QPainter>
#include <QFontMetrics>
#include <QGraphicsScene>

#if defined (QT_VERSION) && defined(WIN32) && defined(_MSC_VER)
# if QT_VERSION >= QT_VERSION_CHECK(5,0,0) 
# pragma execution_character_set("UTF-8")
# endif
#endif

MouseLonLatItem::MouseLonLatItem()
{
}

MouseLonLatItem::~MouseLonLatItem()
{
}

void MouseLonLatItem::setLonLatInfo(int gisLevel, double lon, double lat)
{
	//���������
	long long mapScale = TileSystem::GroundResolution(lat, gisLevel) / pixelRealDistance;

	currentLonlatPos = QPointF(lon, lat);
	drawStr = QString("��ͼ�ȼ�:%1  ������:1:%2  ����:%3, γ��:%4").arg(gisLevel).arg(mapScale).arg(lon).arg(lat);

	// �����ַ�����Ҫ�ľ��δ�С
	QFont font;
	QFontMetrics fontMet(font);
	auto r = fontMet.boundingRect(drawStr);
	r.translate(-r.topLeft());
	r.setWidth(r.width() + 10);
	r.setHeight(r.height() + 4);

	this->setRect(r);
	updateLabelPos();
}

void MouseLonLatItem::updateLabelPos()
{
	QPointF sceneBottomRight = scene()->sceneRect().bottomRight();

	auto pos = sceneBottomRight - QPointF(rect().width(), rect().height());
	setPos(pos);
	update();
}

void MouseLonLatItem::setPixelMapScaleSize(double d)
{
	pixelRealDistance = d;

	/*
	
	int t = QApplication::desktop()->screen()->heightMM();//381 mm
	double mi = t * 1.0 / QApplication::desktop()->screen()->height();

	double pixelLen = 100 / mi;	//10cm

	painter->drawLine(6633, 3548, 6633 + pixelLen, 3548);

	QPointF p1, p2;
	CoordinateTransform::PixelXYToLonLat(6633, 3548, 5, p1.rx(), p1.ry());
	CoordinateTransform::PixelXYToLonLat(6633 + pixelLen, 3548, 5, p2.rx(), p2.ry());

	double d = CoordinateTransform::LonLatToDistance(p1.x(), p1.y(), p2.x(), p2.y());

	*/
}

void MouseLonLatItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	//����ɫ
	painter->fillRect(this->rect(), QColor(127, 127, 127));
	//painter->fillRect(this->rect(), Qt::black);

	//��������
	painter->setPen(Qt::white);
	painter->drawText(rect(), Qt::AlignCenter, drawStr);
}
