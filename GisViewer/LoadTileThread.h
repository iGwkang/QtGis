#pragma once
#include <QThread>
#include <QMutex>
#include <QRect>
#include <QImage>
#include <QWaitCondition>
#include <QReadWriteLock>
#include <unordered_map>
#include <atomic>

//������Ƭ��Ϣ
struct TileInfo
{
	explicit TileInfo() {}
	std::string key;
	int level;		//���ڵȼ�
	int tileX;
	int tileY;
	QImage image;	//image
};

/*
 * @brief ������Ƭ���̣߳����շ��ص���Ƭ��:MapItem
 */
class LoadTileThread : public QThread
{
	Q_OBJECT

	// ��Ƭ��������
	enum TileType
	{
		JPEG,
		MBTILES
	};

public:
	LoadTileThread();
	~LoadTileThread();

	//ֹͣ�߳�
	void stopThread();

	//���õ�ǰ�ӿھ��κ͵�ͼ�ȼ�
	void setCurrentRectAndLevel(const QRect &, int level);
	//������Ƭ
	//bool drawTile(QPainter *painter, int level);

signals:
	//���ͼ��غõ���Ƭ
	void loadedTileImage(int, QPoint, QImage);
	void sendLoadTile(int, QList<TileInfo>);

protected:
	void run();

private:
	//�����ӿ�����͵ȼ� ������Ҫ���ص���Ƭ��Χ
	void calcTileScope(const QRect &r, QPoint &topLeftTile, QPoint &bottomRightTile);

	//������Ƭ
	//void loadTile(const QPoint &topLeftTile, const QPoint &bottomRightTile, int gisLevel, std::unordered_map<int, std::unordered_map<std::string, TileInfo>> &outImage);
	void loadTile(const QPoint &topLeftTile, const QPoint &bottomRightTile, int gisLevel, QList<TileInfo> &outImage);

	//��ȡ�ϼ���Ƭ
	bool getLevelTile(int & tileX, int & tileY, int & gisLevel, std::string &key, QImage &outImage);

	//�ϲ����غõ���Ƭ
	//QImage mergeTileImage(const QRect & viewRect, int gisLevel, const std::unordered_map<int, std::unordered_map<std::string, TileInfo>> &);

	//�ͷż�����Ƭʹ�õ��ڴ�
	void freeTileMemory(int gisLevel);

private:
	std::atomic_bool isRun;// �߳��Ƿ�����

	//�߳���
	QMutex threadMutex;
	QWaitCondition condition;

	QReadWriteLock rwMutex;	//��д��
	int currentGisLevel;	//��ǰ��ͼ�ȼ�
	QRect mRect;	//��ǰ������ӿ�(��Ƭ��Χ)

	// ���ո����̵߳��õ�
	//QMutex drawLock;
	//DrawImage image;

	//���غõ���Ƭ
	std::unordered_map<std::string, QImage> loadedImage;

	TileType tileType;		// ��Ƭ��������
	std::string mTilePath;	// ��Ƭ·��
};
