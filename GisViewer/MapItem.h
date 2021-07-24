#pragma once

#include <QGraphicsObject>
#include <QTimer>
#include "TileItem.h"
#include "LoadTileThread.h"

class LoadTileThread;

/**
 * @brief ��Ƭ�㡣��ͼ�ĵ�ͼ����Ƭ���ȣ����ţ�������Ƭ��ͨ�������ȥ����
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

	//�ı��ͼ��Ƭ�ȼ�
	void zoomTileLevel(int delta);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

private slots:
	//��ʱ��
	void loadTimeOutSlot();

	//�̼߳�������Ƭ֮���͸���ͼ��
	void updateTile(int level, QList<TileInfo>);

private:
	//��ʼ���źŲ�
	void initSigAndSlt();

	//��ʱ������Ƭ
	void timeoutUpdateTile(int level);

	//ƴ����Ƭ�������ͷŲ���Ҫ����Դ
	void puzzle();

	//�����Ƭȱʧ�� �����ϼ���Ƭ
	TileItem *findUpLevelItem(int &level, int &tileX, int &tileY);

	//�ͷŵȼ������Ƭ
	void freeUpLevelTile();

	//�ͷŲ����ӿ��е���Ƭ
	void freeTileMemory();

private:
	QRectF mBoundingRect;
	//������Ƭ���߳�
	LoadTileThread *mLoadTileThread;
	//��ǰ���Ƶ���Ƭ
	//TileImage mCurrentDrawTile;
	std::unordered_map<std::string, TileItem *> levelTile;
	bool isZoomOut;

	//����ʱ�첽������ͬ��
	int gisLevel;	//��ǰ��ʾ�ĵ�ͼ�ȼ�
	int realLevel;	//��ʵ��ͼ�ȼ�

	QTimer timer;	//��ʱ�� 500msˢ��ˢ��һ�ε�ͼ
};
