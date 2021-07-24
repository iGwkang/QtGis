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

	//更新能力圈位置
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
	setRotation(mData.realData.attitude.bearing);	//设置方位角
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
	
	//设置实体图的坐标和姿态
	setPos(mData.pixelPos);	//设置坐标
	setRotation(mData.realData.attitude.bearing);	//设置方位角

	//设置能力圈的坐标和姿态 add by gwk 2019/11/07
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
	//如果已经存在，则更新数据
	if (_iter != mAbilityCircle.end())
	{
		_iter->second->setCircleParam(circleParam);
	}
	else
	{
		//不存在的话 就新建一个能力圈

		//设置能力圈
		EntityAbilityCircleItem * newCircle = new EntityAbilityCircleItem(parentItem(), this, circleParam);
		mAbilityCircle[circleParam.abilityType] = newCircle;
	}
}

void EntityItem::removeCircle(const std::string &circleType)
{
	//删除能力圈
	auto _iter = mAbilityCircle.find(circleType);
	if (_iter != mAbilityCircle.end())
	{
		delete _iter->second;
		mAbilityCircle.erase(_iter);
	}
}

void EntityItem::setScanParam(const ScanParam &param)
{
	// 设置扫描参数
	auto _iter = mAbilityCircle.find(param.abilityType);
	//如果已经存在，则设置数据
	if (_iter != mAbilityCircle.end())
	{
		_iter->second->setScanParam(param);
	}
}

void EntityItem::setVisible(bool show)
{
	//隐藏能力圈
	for (auto & circle : mAbilityCircle)
	{
		circle.second->setVisible(show);
	}

	// 标牌信息
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

		//左键移动 更新实体坐标
		TileSystem::PixelXYToLonLat(this->x(), this->y(), GisStatus::instance().getGisLevel(), mData.realData.position.longitude, mData.realData.position.latitude);
		mData.pixelPos = event->pos().toPoint();

		//移动之后 需要更新能力圈位置
		for (auto & it : mAbilityCircle)
		{
			it.second->setPos(pos());
		}

		//更新标牌位置
		mScutcheon->updatePos();
		//更新内容
		mScutcheon->recalculateBounding();

		isMoved = true;
	}
	//QGraphicsSvgItem::mouseMoveEvent(event);
}

void EntityItem::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
	if (MoveMapMode != GisStatus::instance().getGisMode())
		return event->ignore();

	// 移动地图的时候才处理事件
	if (event->button() == Qt::LeftButton && isMoved)
	{
		//左键移动 更新实体坐标
		TileSystem::PixelXYToLonLat(this->x(), this->y(), GisStatus::instance().getGisLevel(), mData.realData.position.longitude, mData.realData.position.latitude);
		mData.pixelPos = pos().toPoint();

		//移动之后 需要更新能力圈位置
		for (auto & it : mAbilityCircle)
		{
			it.second->setPos(mData.pixelPos);
			it.second->updateCircle();
			it.second->updateScan();
		}

		//更新标牌位置
		mScutcheon->updatePos();
		//更新内容
		mScutcheon->recalculateBounding();

		//更新数据管理中的数据
		DataManager<EntityInfo>::instance().updateData(mData.realData);

		// 发送实体被移动的信号
		emit GisSignals::instance()->gisMoveEntity(mData.realData.ID, mData.realData.position);
		//std::cout << mData.realData.ID << " 实体移动 " << mData.realData.position.x << ' ' << mData.realData.position.y << std::endl;
	}
	else if (event->button() == Qt::RightButton)
	{
		//获取屏幕坐标
		QPoint _screenPos = event->screenPos();
		//发出实体被鼠标右键点击
		emit GisSignals::instance()->sigEntityRightClicked(mData.realData.ID, _screenPos.x(), _screenPos.y());
		//std::cout << mData.realData.ID << " 鼠标右键点击实体 x:" << _screenPos.x() << " y:" << _screenPos.y() << std::endl;
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

	//设置位置
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

	//判断圈的半径是否比扫描半径小
	//if (mCircleParam.radius < mScanParam.radius)
	if (mCircleParam.radius <= mCircleParam.scanPatam.radius)	//modify by gwk 扫描参数放在实体能力圈里
	{
		//mScanParam.radius = mCircleParam.radius;
		mCircleParam.scanPatam.radius = mCircleParam.radius;

		updateScan();
	}

	//设置方位
	this->setRotation(mEntity->rotation());
	//设置位置
	this->setPos(mEntity->pos());
}

// modify by gwk 2019-11-25 08:40:38
// 修改前: 实体能力圈和扫描参数是分开的
// 修改后: 实体能力圈里包含了扫描
void EntityAbilityCircleItem::setScanParam(const ScanParam &param)
{
	//mScanParam = param;
	mCircleParam.scanPatam = param;

	//防止超出范围
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
	
	//画圈
	painter->fillPath(mDrawCirclePath, QColor(mCircleParam.circleColor.r, mCircleParam.circleColor.g, mCircleParam.circleColor.b, mCircleParam.circleColor.a));


	//画扫描扇形
	if (mCircleParam.scanPatam.radius && !mDrawScanPath.isEmpty() && scanVisible)
	{
#if 0
		//当前仿真时间
		double curTime = GisStatus::instance().getCurrentSimulationTime();

		//判断扫描样式
		if (CircleScan == mCircleParam.scanPatam.scanStyle)	//圆周扫描
		{
			//根据速度 计算扫描的当前位置
			double totalAngle = mCircleParam.scanPatam.scanSpeed * curTime / (mCircleParam.endAngle - mCircleParam.startAngle);
			double angle = (totalAngle - int(totalAngle)) * (mCircleParam.endAngle - mCircleParam.startAngle);

			painter->rotate(angle);
		}
		else if (SectorScan == mCircleParam.scanPatam.scanStyle)	//扇形扫描
		{
			double sectorAngle = (mCircleParam.endAngle - mCircleParam.startAngle - mCircleParam.scanPatam.scanAngleSize);

			//根据速度 计算扫描的当前位置
			double totalAngle = mCircleParam.scanPatam.scanSpeed * curTime / sectorAngle;	//扫过的次数
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

		// 直接使用结构体中的参数 by gwk 2019/12/04
		painter->rotate(mCircleParam.scanPatam.curBearing);

#endif

		// 绘制扫描扇形
		painter->fillPath(mDrawScanPath, QColor(mCircleParam.scanPatam.scanColor.r, mCircleParam.scanPatam.scanColor.g,
			mCircleParam.scanPatam.scanColor.b, mCircleParam.scanPatam.scanColor.a));
	}

	
}

void EntityAbilityCircleItem::updateCircle()
{
	// 计算包围盒
	double pixelR = mCircleParam.radius / TileSystem::GroundResolution(mEntity->getRealData().position.latitude, GisStatus::instance().getGisLevel());
	double pixelR_2 = pixelR * 2;
	mBoundingRect = QRectF(-pixelR, -pixelR, pixelR_2, pixelR_2);

	//设置要绘制的圈
	mDrawCirclePath = QPainterPath();
	mDrawCirclePath.arcTo(mBoundingRect, 90 - mCircleParam.endAngle, mCircleParam.endAngle - mCircleParam.startAngle);
}

void EntityAbilityCircleItem::updateScan()
{
	// 计算包围盒
	double pixelR = mCircleParam.scanPatam.radius / TileSystem::GroundResolution(mEntity->getRealData().position.latitude, GisStatus::instance().getGisLevel());
	double pixelR_2 = pixelR * 2;
	QRectF scanRect(-pixelR, -pixelR, pixelR_2, pixelR_2);

	//扫描扇形
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
	//设置可移动
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
		编号：
		型号：
		经度：
		纬度：
		高度：
		速度：
		航向：
	*/
	auto & realData = mEntity->getRealData();

	int h = 0, w = 0;
	mDrawStr.clear();
	int row = 0;

	{
		QString str = QString("编号: %1").arg(realData.ID);
		w = std::max(w, fontMet.width(str));
		mDrawStr.append(str).append('\n');
		row++;
	}
	{
		QString str = QString("型号: %1").arg(realData.entityName.c_str());
		w = std::max(w, fontMet.width(str));
		mDrawStr.append(str).append('\n');
		row++;
	}
	if (infoExpand)
	{
		{
			QString str = QString("经度: %1").arg(realData.position.longitude, 0, 'f', 2);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("纬度: %1").arg(realData.position.latitude, 0, 'f', 2);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("高度: %1 m").arg(realData.position.altitude, 0, 'f', 1);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("速度: %1 m/s").arg(realData.speed, 0, 'f', 1);
			w = std::max(w, fontMet.width(str));
			mDrawStr.append(str).append('\n');
			row++;
		}
		{
			QString str = QString("航向: %1°").arg(realData.attitude.bearing, 0, 'f', 1);
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

