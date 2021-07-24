#include "EntityItem.h"
#include "TileSystem.h"
#include "GisStatus.h"
#include "GisSignals.h"
#include "DataManager.hpp"
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include <iostream>
#include <algorithm>

EntityItem::EntityItem(QGraphicsItem *parent)
	: QGraphicsSvgItem(parent)
{
}

EntityItem::EntityItem(QGraphicsItem *parent, const GisEntity &data)
	: QGraphicsSvgItem(parent), mScutcheon(new EntityScutcheonItem(parent, this))
{
	setZValue(1);
	setEntityData(data);
	setFlags(flags() | ItemIsSelectable | ItemIsMovable);
	//setAcceptedMouseButtons(Qt::LeftButton | Qt::RightButton);
}

EntityItem::~EntityItem()
{
	for (auto & circle : mAbilityCircle)
	{
		delete circle.second;
	}
	mAbilityCircle.clear();
	delete mScutcheon;
	mScutcheon = nullptr;
}

QRectF EntityItem::boundingRect() const
{
	return EntityIconSize;
}

void EntityItem::updateLevel()
{
	TileSystem::LonLatToPixelXY(mData.realData.position.longitude, mData.realData.position.latitude, GisStatus::instance().getGisLevel(), mData.pixelPos.rx(), mData.pixelPos.ry());

	setPos(mData.pixelPos);

	//��������Ȧλ��
	for (auto & circle : mAbilityCircle)
	{
		circle.second->updateLevel();
	}

	mScutcheon->updatePos();
}

void EntityItem::setEntityData(const GisEntity & data)
{
	mData = data;
	mScutcheon->recalculateBounding();
	updateLevel();
	setRotation(mData.realData.attitude.bearing);	//���÷�λ��
}

const EntityInfo & EntityItem::getRealData() const
{
	return mData.realData;
}

void EntityItem::updateEntity(const GisEntity &data)
{
	if (mData.svgIcon != data.svgIcon)
	{
		mData.svgIcon = data.svgIcon;
		update();
		return;
	}
	
	mData = data;
	
	//����ʵ��ͼ���������̬
	setPos(mData.pixelPos);	//��������
	setRotation(mData.realData.attitude.bearing);	//���÷�λ��

	//��������Ȧ���������̬ add by gwk 2019/11/07
	for (auto & circle : mAbilityCircle)
	{
		circle.second->setPos(mData.pixelPos);
		circle.second->setRotation(mData.realData.attitude.bearing);
		circle.second->updateCircle();
		circle.second->updateScan();
	}
	mScutcheon->recalculateBounding();
	mScutcheon->updatePos();
}

void EntityItem::setCircleParam(const AbilityCircleParam &circleParam)
{
	auto _iter = mAbilityCircle.find(circleParam.abilityType);
	//����Ѿ����ڣ����������
	if (_iter != mAbilityCircle.end())
	{
		_iter->second->setCircleParam(circleParam);
	}
	else
	{
		//�����ڵĻ� ���½�һ������Ȧ

		//��������Ȧ
		EntityAbilityCircleItem * newCircle = new EntityAbilityCircleItem(parentItem(), this, circleParam);
		mAbilityCircle[circleParam.abilityType] = newCircle;
	}
}

void EntityItem::removeCircle(const std::string &circleType)
{
	//ɾ������Ȧ
	auto _iter = mAbilityCircle.find(circleType);
	if (_iter != mAbilityCircle.end())
	{
		delete _iter->second;
		mAbilityCircle.erase(_iter);
	}
}

void EntityItem::setScanParam(const ScanParam &param)
{
	// ����ɨ�����
	auto _iter = mAbilityCircle.find(param.abilityType);
	//����Ѿ����ڣ�����������
	if (_iter != mAbilityCircle.end())
	{
		_iter->second->setScanParam(param);
	}
}

void EntityItem::setVisible(bool show)
{
	//��������Ȧ
	for (auto & circle : mAbilityCircle)
	{
		circle.second->setVisible(show);
	}

	// ������Ϣ
	mScutcheon->setVisible(show);

	QGraphicsSvgItem::setVisible(show);
}

void EntityItem::setAbilityCircleVisible(const std::string & circleType, bool show)
{
	auto _find = mAbilityCircle.find(circleType);
	if (_find != mAbilityCircle.end())
	{
		_find->second->setVisible(show);
	}
}

void EntityItem::setAbilityCircleScanVisible(const std::string & circleType, bool show)
{
	auto _find = mAbilityCircle.find(circleType);
	if (_find != mAbilityCircle.end())
	{
		_find->second->setScanVisible(show);
	}
}

void EntityItem::setScutcheonVisible(bool show)
{
	mScutcheon->setVisible(show);
}

void EntityItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	painter->setRenderHint(QPainter::Antialiasing);
	
	if (mData.svgIcon)
		mData.svgIcon->render(painter, EntityIconSize);
}

void EntityItem::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	if (MoveMapMode != GisStatus::instance().getGisMode())
		return event->ignore();

	if (event->button() == Qt::LeftButton)
	{
		mousePrePos = event->scenePos();
		isMoved = false;
	}
	QGraphicsSvgItem::mousePressEvent(event);
}

void EntityItem::mouseMoveEvent(QGraphicsSceneMouseEvent *event)
{
	if (MoveMapMode != GisStatus::instance().getGisMode())
		return event->ignore();
	
	if (event->buttons() & Qt::LeftButton)
	{
		QPointF curPos = event->scenePos();
		QPointF d = curPos - mousePrePos;
		mousePrePos = curPos;

		moveBy(d.x(), d.y());

		//����ƶ� ����ʵ������
		TileSystem::PixelXYToLonLat(this->x(), this->y(), GisStatus::instance().getGisLevel(), mData.realData.position.longitude, mData.realData.position.latitude);
		mData.pixelPos = event->pos().toPoint();

		//�ƶ�֮�� ��Ҫ��������Ȧλ��
		for (auto & it : mAbilityCircle)
		{
			it.second->setPos(pos());
		}

		//���±���λ��
		mScutcheon->updatePos();
		//��������
		mScutcheon->recalculateBounding();

		isMoved = true;
	}
	//QGraphicsSvgItem::mouseMoveEvent(event);
}

void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (MoveMapMode != GisStatus::instance().getGisMode())
		return event->ignore();

	// �ƶ���ͼ��ʱ��Ŵ����¼�
	if (event->button() == Qt::LeftButton && isMoved)
	{
		//����ƶ� ����ʵ������
		TileSystem::PixelXYToLonLat(this->x(), this->y(), GisStatus::instance().getGisLevel(), mData.realData.position.longitude, mData.realData.position.latitude);
		mData.pixelPos = pos().toPoint();

		//�ƶ�֮�� ��Ҫ��������Ȧλ��
		for (auto & it : mAbilityCircle)
		{
			it.second->setPos(mData.pixelPos);
			it.second->updateCircle();
			it.second->updateScan();
		}

		//���±���λ��
		mScutcheon->updatePos();
		//��������
		mScutcheon->recalculateBounding();

		//�������ݹ����е�����
		DataManager<EntityInfo>::instance().updateData(mData.realData);

		// ����ʵ�屻�ƶ����ź�
		emit GisSignals::instance()->gisMoveEntity(mData.realData.ID, mData.realData.position);
		//std::cout << mData.realData.ID << " ʵ���ƶ� " << mData.realData.position.x << ' ' << mData.realData.position.y << std::endl;
	}
	else if (event->button() == Qt::RightButton)
	{
		//��ȡ��Ļ����
		QPoint _screenPos = event->screenPos();
		//����ʵ�屻����Ҽ����
		emit GisSignals::instance()->sigEntityRightClicked(mData.realData.ID, _screenPos.x(), _screenPos.y());
		//std::cout << mData.realData.ID << " ����Ҽ����ʵ�� x:" << _screenPos.x() << " y:" << _screenPos.y() << std::endl;
	}
	else
	{
		QGraphicsSvgItem::mouseReleaseEvent(event);
	}
}


EntityAbilityCircleItem::EntityAbilityCircleItem(QGraphicsItem *parent, EntityItem *entityParent, const AbilityCircleParam &param)
	: QGraphicsObject(parent), mEntity(entityParent), scanVisible(true)
{
	setZValue(0);
	setCircleParam(param);
}

// EntityAbilityCircleItem::~EntityAbilityCircleItem()
// {
// }

QRectF EntityAbilityCircleItem::boundingRect() const
{
	return mBoundingRect;
}

void EntityAbilityCircleItem::updateLevel()
{
	updateCircle();
	updateScan();

	//����λ��
	this->setPos(mEntity->pos());
	//update();
}

void EntityAbilityCircleItem::setEntityParentItem(EntityItem *parent)
{
	mEntity = parent;

	this->setPos(mEntity->pos());
	this->setRotation(mEntity->rotation());
}

void EntityAbilityCircleItem::setCircleParam(const AbilityCircleParam &param)
{
	mCircleParam = param;
	updateCircle();

	//�ж�Ȧ�İ뾶�Ƿ��ɨ��뾶С
	//if (mCircleParam.radius < mScanParam.radius)
	if (mCircleParam.radius <= mCircleParam.scanPatam.radius)	//modify by gwk ɨ���������ʵ������Ȧ��
	{
		//mScanParam.radius = mCircleParam.radius;
		mCircleParam.scanPatam.radius = mCircleParam.radius;

		updateScan();
	}

	//���÷�λ
	this->setRotation(mEntity->rotation());
	//����λ��
	this->setPos(mEntity->pos());
}

// modify by gwk 2019-11-25 08:40:38
// �޸�ǰ: ʵ������Ȧ��ɨ������Ƿֿ���
// �޸ĺ�: ʵ������Ȧ�������ɨ��
void EntityAbilityCircleItem::setScanParam(const ScanParam &param)
{
	//mScanParam = param;
	mCircleParam.scanPatam = param;

	//��ֹ������Χ
	//mScanParam.radius = std::min(mScanParam.radius, mCircleParam.radius);
	mCircleParam.scanPatam.radius = std::min(mCircleParam.scanPatam.radius, mCircleParam.radius);

	updateScan();
}

void EntityAbilityCircleItem::setScanVisible(bool show)
{
	scanVisible = show;
	//update();
}

void EntityAbilityCircleItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	//painter->fillRect(mBoundingRect, QColor(255, 255, 255, 0));
	painter->setRenderHint(QPainter::Antialiasing);
	//painter->setCompositionMode(QPainter::RasterOp_SourceXorDestination);
	
	//��Ȧ
	painter->fillPath(mDrawCirclePath, QColor(mCircleParam.circleColor.r, mCircleParam.circleColor.g, mCircleParam.circleColor.b, mCircleParam.circleColor.a));


	//��ɨ������
	if (mCircleParam.scanPatam.radius && !mDrawScanPath.isEmpty() && scanVisible)
	{
#if 0
		//��ǰ����ʱ��
		double curTime = GisStatus::instance().getCurrentSimulationTime();

		//�ж�ɨ����ʽ
		if (CircleScan == mCircleParam.scanPatam.scanStyle)	//Բ��ɨ��
		{
			//�����ٶ� ����ɨ��ĵ�ǰλ��
			double totalAngle = mCircleParam.scanPatam.scanSpeed * curTime / (mCircleParam.endAngle - mCircleParam.startAngle);
			double angle = (totalAngle - int(totalAngle)) * (mCircleParam.endAngle - mCircleParam.startAngle);

			painter->rotate(angle);
		}
		else if (SectorScan == mCircleParam.scanPatam.scanStyle)	//����ɨ��
		{
			double sectorAngle = (mCircleParam.endAngle - mCircleParam.startAngle - mCircleParam.scanPatam.scanAngleSize);

			//�����ٶ� ����ɨ��ĵ�ǰλ��
			double totalAngle = mCircleParam.scanPatam.scanSpeed * curTime / sectorAngle;	//ɨ���Ĵ���
			double angle = (totalAngle - int(totalAngle));

			if (int(totalAngle) % 2 == 0)
			{
				angle = angle * sectorAngle;
			}
			else
			{
				angle = 1 - angle;
				angle = angle * sectorAngle;
			}

			painter->rotate(angle);
		}
#else

		// ֱ��ʹ�ýṹ���еĲ��� by gwk 2019/12/04
		painter->rotate(mCircleParam.scanPatam.curBearing);

#endif

		// ����ɨ������
		painter->fillPath(mDrawScanPath, QColor(mCircleParam.scanPatam.scanColor.r, mCircleParam.scanPatam.scanColor.g,
			mCircleParam.scanPatam.scanColor.b, mCircleParam.scanPatam.scanColor.a));
	}

	
}

void EntityAbilityCircleItem::updateCircle()
{
	// �����Χ��
	double pixelR = mCircleParam.radius / TileSystem::GroundResolution(mEntity->getRealData().position.latitude, GisStatus::instance().getGisLevel());
	double pixelR_2 = pixelR * 2;
	mBoundingRect = QRectF(-pixelR, -pixelR, pixelR_2, pixelR_2);

	//����Ҫ���Ƶ�Ȧ
	mDrawCirclePath = QPainterPath();
	mDrawCirclePath.arcTo(mBoundingRect, 90 - mCircleParam.endAngle, mCircleParam.endAngle - mCircleParam.startAngle);
}

void EntityAbilityCircleItem::updateScan()
{
	// �����Χ��
	double pixelR = mCircleParam.scanPatam.radius / TileSystem::GroundResolution(mEntity->getRealData().position.latitude, GisStatus::instance().getGisLevel());
	double pixelR_2 = pixelR * 2;
	QRectF scanRect(-pixelR, -pixelR, pixelR_2, pixelR_2);

	//ɨ������
	//mDrawScanPath = QPainterPath();
	QPainterPath t;
	if (mCircleParam.scanPatam.radius > 0)
		t.arcTo(scanRect, 90 - mCircleParam.scanPatam.scanAngleSize - mCircleParam.startAngle, mCircleParam.scanPatam.scanAngleSize);
	mDrawScanPath.swap(t);
}

EntityScutcheonItem::EntityScutcheonItem(QGraphicsItem *parentItem, EntityItem *entity)
	: QGraphicsItem(parentItem),
	mEntity(entity),
	mDrawLine(new QGraphicsLineItem(parentItem)),
	infoExpand(false)
{
	setZValue(2);
	//���ÿ��ƶ�
	setFlag(ItemIsMovable);
	setFlag(ItemIsSelectable);
	setFlag(ItemSendsGeometryChanges);
	mDrawLine->setPen(QPen(Qt::yellow));
	mDrawLine->setZValue(1);
}

EntityScutcheonItem::~EntityScutcheonItem()
{
 	delete mDrawLine;
 	mDrawLine = nullptr;
}

QRectF EntityScutcheonItem::boundingRect() const
{
	return mBoundingRect;
}

void EntityScutcheonItem::recalculateBounding()
{
	if (!isVisible())
		return;

	QFont font;
	QFontMetrics fontMet(font);

	/*
		��ţ�
		�ͺţ�
		���ȣ�
		γ�ȣ�
		�߶ȣ�
		�ٶȣ�
		����
	*/
	auto & realData = mEntity->getRealData();

	int h = 0, w = 0;
	mDrawStr.clear();
	int row = 0;

	{
		QString str = QString("���: %1").arg(realData.ID);
		w = std::max(w, fontMet.width(str));
		mDrawStr.append(str).append('\n');
		row++;
	}
	{
		QString str = QString("�ͺ�: %1").arg(realData.entityName.c_str());
		w = std::max(w, fontMet.width(str));
		mDrawStr.append(str).append('\n');
		row++;
	}
	if (infoExpand)
	{
		{
			QString str = QString("����: %1").arg(realData.position.longitude, 0, 'f', 2);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("γ��: %1").arg(realData.position.latitude, 0, 'f', 2);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("�߶�: %1 m").arg(realData.position.altitude, 0, 'f', 1);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("�ٶ�: %1 m/s").arg(realData.speed, 0, 'f', 1);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("����: %1��").arg(realData.attitude.bearing, 0, 'f', 1);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str);
			row++;
		}
	}
	
	h = row * (fontMet.height() + fontMet.descent());
	mBoundingRect = QRectF(0 + 15, -h - 15, w, h);
	//updatePos();
}

void EntityScutcheonItem::updatePos()
{
	setPos(mEntity->pos() + offset);
}

void EntityScutcheonItem::setVisible(bool show)
{
	mDrawLine->setVisible(show);
	QGraphicsItem::setVisible(show);

	if (show)
	{
		recalculateBounding();
		/*updatePos();*/
	}
}

void EntityScutcheonItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget /*= Q_NULLPTR*/)
{
	painter->setPen(Qt::white);
	painter->fillRect(mBoundingRect, QColor(0, 0, 200, 130));
	painter->drawText(mBoundingRect, mDrawStr);
}

QVariant EntityScutcheonItem::itemChange(GraphicsItemChange change, const QVariant &value)
{
	if (change == QGraphicsItem::ItemPositionHasChanged)
	{
		offset = pos() - mEntity->pos();
		//painter->drawLine(pos(), mEntity->pos());
// 		QLineF line;
// 		line.setP1(mapToParent(boundingRect().bottomLeft()));
// 		line.setP2(mapToParent(mEntity->pos()));
		mDrawLine->setLine(QLineF(getMinDistancePoint(mEntity->pos()), mEntity->pos()));
	}

	return QGraphicsItem::itemChange(change, value);
}

void EntityScutcheonItem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event)
{
	if (event->button() == Qt::LeftButton)
	{
		infoExpand = !infoExpand;

		recalculateBounding();
		updatePos();
	}
}

QPointF EntityScutcheonItem::getMinDistancePoint(const QPointF &p)
{
	QPointF ret;

	QPointF temp = mapToParent(mBoundingRect.topLeft());
	double tempLen = QLineF(temp, p).length();
	ret = temp;

	temp = mapToParent(mBoundingRect.bottomLeft());
	double len = QLineF(temp, p).length();
	if (tempLen > len)
	{
		ret = temp;
		tempLen = len;
	}

	temp = mapToParent(mBoundingRect.topRight());
	len = QLineF(temp, p).length();
	if (tempLen > len)
	{
		ret = temp;
		tempLen = len;
	}

	temp = mapToParent(mBoundingRect.bottomRight());
	len = QLineF(temp, p).length();
	if (tempLen > len)
	{
		ret = temp;
		//tempLen = len;
	}
	return ret;
}

