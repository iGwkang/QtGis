#pragma once

#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <unordered_map>
#include "GisEntity.h"

class EntityItem;	//实体
class EntityAbilityCircleItem;	//实体能力圈
class EntityScutcheonItem;	//实体标牌信息

/**
 *	@brief 地图上的单个实体。包含能力圈信息
 *	@date 2019/11/05
 *	@author gwk
 */
class EntityItem : public QGraphicsSvgItem
{
	//Q_OBJECT

public:
	EntityItem(QGraphicsItem *parent, const GisEntity &);
	EntityItem(QGraphicsItem *parent);
	~EntityItem();

	QRectF boundingRect() const;

	//缩放地图重绘
	void updateLevel();

	//获取ID
	inline long long getID() const { return mData.realData.ID; }

	//设置实体原始数据
	void setEntityData(const GisEntity &);

	//获取原始数据
	const EntityInfo & getRealData() const;

	//更新实体
	void updateEntity(const GisEntity &);

	//设置能力圈的参数 add by gwk 2019/11/07
	void setCircleParam(const AbilityCircleParam &);

	//删除能力圈 add by gwk 2019/11/07
	void removeCircle(const std::string &);

	//设置扫描参数 add by gwk 2019/11/07
	void setScanParam(const ScanParam &);

	//设置显隐
	void setVisible(bool show);

	//设置能力圈显隐
	void setAbilityCircleVisible(const std::string & circleType, bool show);

	//设置能力圈上扫描效果显隐
	void setAbilityCircleScanVisible(const std::string & circleType, bool show);

	//设置实体标牌的显隐
	void setScutcheonVisible(bool show);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	GisEntity mData;	//实体数据

	//移动实体的标记变量
	QPointF mousePrePos;
	bool isMoved;

	//标牌信息 scutcheon
	EntityScutcheonItem *mScutcheon;

	std::unordered_map<std::string, EntityAbilityCircleItem *> mAbilityCircle;	//能力圈
};


/**
 *	@brief 实体能力圈
 *	@note 能力圈中包含扫描效果, 能力圈是放在实体层的(能力圈和实体是同一个层级)。因为实体的包围盒小，不能装下能力圈
 *	@date 2019/11/07
 *	@author gwk
 */
class EntityAbilityCircleItem : public QGraphicsObject
{
	//Q_OBJECT

public:
	EntityAbilityCircleItem(QGraphicsItem *parent, EntityItem *, const AbilityCircleParam &);
	//~EntityAbilityCircleItem();

	//包围盒大小
	QRectF boundingRect() const;

	//地图缩放 重新计算圈大小
	void updateLevel();

	//设置能力圈所在的实体
	void setEntityParentItem(EntityItem *);

	//设置能力圈的参数
	void setCircleParam(const AbilityCircleParam &);

	//设置扫描参数
	void setScanParam(const ScanParam &);

	//设置扫描显隐
	void setScanVisible(bool show);

	void updateCircle();	//更新圆
	void updateScan();		//更新扫描

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	QRectF mBoundingRect;
	QPainterPath mDrawCirclePath;	//需要填充的圆
	QPainterPath mDrawScanPath;	//扫描扇形

	EntityItem * mEntity;	//能力圈所在的实体

	AbilityCircleParam mCircleParam;	//能力圈参数

	//ScanParam mScanParam;	//扫描参数
	bool scanVisible;	//是否显示扫描
};

/**
 *	@brief 实体上的标牌信息
 *	@author gwk
 *	@date 2019/11/19
 */
class EntityScutcheonItem : public QGraphicsItem
{
public:
	EntityScutcheonItem(QGraphicsItem *parentItem, EntityItem *entity);
	~EntityScutcheonItem();

	QRectF boundingRect() const;

	//重新计算包围盒
	void recalculateBounding();

	//更新坐标
	void updatePos();

	//设置显隐
	void setVisible(bool);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	// 双击展开详细信息
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	
	//在包围盒四个点中 获取距离点p最近的点
	QPointF getMinDistancePoint(const QPointF &p);

private:
	QRectF mBoundingRect;
	QString mDrawStr;
	EntityItem *mEntity;	//标牌表示的实体
	QPointF offset;	//和实体间的间隔 (用于绘制 标牌与实体之间的 连接)
	QGraphicsLineItem *mDrawLine;
	bool infoExpand;	// 双击标牌显示详细信息
	//QLineF mDrawLine;
};
