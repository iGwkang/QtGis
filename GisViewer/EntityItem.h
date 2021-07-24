#pragma once

#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <unordered_map>
#include "GisEntity.h"

class EntityItem;	//ʵ��
class EntityAbilityCircleItem;	//ʵ������Ȧ
class EntityScutcheonItem;	//ʵ�������Ϣ

/**
 *	@brief ��ͼ�ϵĵ���ʵ�塣��������Ȧ��Ϣ
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

	//���ŵ�ͼ�ػ�
	void updateLevel();

	//��ȡID
	inline long long getID() const { return mData.realData.ID; }

	//����ʵ��ԭʼ����
	void setEntityData(const GisEntity &);

	//��ȡԭʼ����
	const EntityInfo & getRealData() const;

	//����ʵ��
	void updateEntity(const GisEntity &);

	//��������Ȧ�Ĳ��� add by gwk 2019/11/07
	void setCircleParam(const AbilityCircleParam &);

	//ɾ������Ȧ add by gwk 2019/11/07
	void removeCircle(const std::string &);

	//����ɨ����� add by gwk 2019/11/07
	void setScanParam(const ScanParam &);

	//��������
	void setVisible(bool show);

	//��������Ȧ����
	void setAbilityCircleVisible(const std::string & circleType, bool show);

	//��������Ȧ��ɨ��Ч������
	void setAbilityCircleScanVisible(const std::string & circleType, bool show);

	//����ʵ����Ƶ�����
	void setScutcheonVisible(bool show);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private:
	GisEntity mData;	//ʵ������

	//�ƶ�ʵ��ı�Ǳ���
	QPointF mousePrePos;
	bool isMoved;

	//������Ϣ scutcheon
	EntityScutcheonItem *mScutcheon;

	std::unordered_map<std::string, EntityAbilityCircleItem *> mAbilityCircle;	//����Ȧ
};


/**
 *	@brief ʵ������Ȧ
 *	@note ����Ȧ�а���ɨ��Ч��, ����Ȧ�Ƿ���ʵ����(����Ȧ��ʵ����ͬһ���㼶)����Ϊʵ��İ�Χ��С������װ������Ȧ
 *	@date 2019/11/07
 *	@author gwk
 */
class EntityAbilityCircleItem : public QGraphicsObject
{
	//Q_OBJECT

public:
	EntityAbilityCircleItem(QGraphicsItem *parent, EntityItem *, const AbilityCircleParam &);
	//~EntityAbilityCircleItem();

	//��Χ�д�С
	QRectF boundingRect() const;

	//��ͼ���� ���¼���Ȧ��С
	void updateLevel();

	//��������Ȧ���ڵ�ʵ��
	void setEntityParentItem(EntityItem *);

	//��������Ȧ�Ĳ���
	void setCircleParam(const AbilityCircleParam &);

	//����ɨ�����
	void setScanParam(const ScanParam &);

	//����ɨ������
	void setScanVisible(bool show);

	void updateCircle();	//����Բ
	void updateScan();		//����ɨ��

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

private:
	QRectF mBoundingRect;
	QPainterPath mDrawCirclePath;	//��Ҫ����Բ
	QPainterPath mDrawScanPath;	//ɨ������

	EntityItem * mEntity;	//����Ȧ���ڵ�ʵ��

	AbilityCircleParam mCircleParam;	//����Ȧ����

	//ScanParam mScanParam;	//ɨ�����
	bool scanVisible;	//�Ƿ���ʾɨ��
};

/**
 *	@brief ʵ���ϵı�����Ϣ
 *	@author gwk
 *	@date 2019/11/19
 */
class EntityScutcheonItem : public QGraphicsItem
{
public:
	EntityScutcheonItem(QGraphicsItem *parentItem, EntityItem *entity);
	~EntityScutcheonItem();

	QRectF boundingRect() const;

	//���¼����Χ��
	void recalculateBounding();

	//��������
	void updatePos();

	//��������
	void setVisible(bool);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);
	QVariant itemChange(GraphicsItemChange change, const QVariant &value);

	// ˫��չ����ϸ��Ϣ
	void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);
	
	//�ڰ�Χ���ĸ����� ��ȡ�����p����ĵ�
	QPointF getMinDistancePoint(const QPointF &p);

private:
	QRectF mBoundingRect;
	QString mDrawStr;
	EntityItem *mEntity;	//���Ʊ�ʾ��ʵ��
	QPointF offset;	//��ʵ���ļ�� (���ڻ��� ������ʵ��֮��� ����)
	QGraphicsLineItem *mDrawLine;
	bool infoExpand;	// ˫��������ʾ��ϸ��Ϣ
	//QLineF mDrawLine;
};
