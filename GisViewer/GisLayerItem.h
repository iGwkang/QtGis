#pragma once

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <unordered_map>
#include <vector>

class MapItem;			//��Ƭ��
class DrawItemLayer;	//��ͼ��
class RouteAreaLayer;	//���߲�
class EntityLayer;		//ʵ���

struct EntityInfo;
struct RouteInfo;
struct AbilityCircleParam;
struct ScanParam;

/**
 * @brief ��ͼ�ϵ�ͼ�㣬������Ƭ�㣬���߲㣬ʵ��㡣��ͼ��Ĳ����У��������ܣ����ݴ���
 */
class GisLayerItem : public QGraphicsObject
{
	//Q_OBJECT

public:
	//��ȡͼ��ʵ��
	static GisLayerItem *getInstance();

	GisLayerItem(QGraphicsItem * parent = nullptr);
	~GisLayerItem();

	/***** ��ͼ���� *****/
#if 1
public:
	//��Χ�д�С
	QRectF boundingRect() const;

	//ͨ�����������ȡ��γ��
	QPointF getLonLatFromScenePos(const QPointF & scenePos);
	//ͨ����γ�Ȼ�ȡ��������
	QPointF getScenePosFromLonLat(const QPointF & lonlatPos);
	//��ȡ��ͼ�ȼ�
	//int getGisLevel() const;

	//����
	void zoom(int delta);

#endif

	/***** �������� *****/
#if 1
public:
	//����/�˳����
	void setMeasureMode(bool mode);


	//����/�˳����ƺ��� true:�����࣬false:�˳����
	void setDrawRouteMode(bool mode);


	//���õ�ͼͼԪ�Ƿ�ɱ༭ (�������й����� ��ͼ���ɱ༭)
	void setEditMode(bool mode);

#endif

	/***** ͼ����� *****/
	//TODO ��Ҫһ�������ͼ�������ܿ�
#if 1
	//���ò�������
	void setMeasureVisible(bool show);

	/**
		@brief ���ú��ߵ�����
		@param[in] show ����
		@param[in] route_id ���ߵ�id, Ĭ��Ϊ0, ��������
		@return void
		@date 2019-11-18 16:38:04
	*/
	void setRouteVisible(bool show, const long long &route_id = 0);

	/**
		@brief ���ú��ߵ������
		@param[in] show ����
		@param[in] route_id ���ߵ�id, Ĭ��Ϊ0, �������к��ߵĵ�
		@return void
		@date 2019-11-22 13:44:58
	*/
	void setRoutePointVisible(bool show, const long long &route_id = 0);

	/**
		@brief ����ʵ��ͼ������� (ͬʱ������ʵ���ϵ�����Ȧ��ɨ��Ч��)
		@param[in] show ����
		@param[in] entity_id ʵ���id, Ĭ��Ϊ0, ��������
		@return void
		@date 2019-11-18 16:50:59
	*/
	void setEntityVisible(bool show, const long long &entity_id = 0);

	/**
		@brief ����ʵ������Ȧ������
		@param[in] show ����
		@param[in] entity_id ʵ���id
		@param[in] circleType ʵ���������Ȧ������
		@return void
		@date 2019-11-18 16:38:13
	*/
	void setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief ����ʵ������Ȧɨ�迪��
		@param[in] show ����
		@param[in] entity_id ʵ���id
		@param[in] circleType ʵ���������Ȧ������
		@return void
		@date 2019-11-18 16:38:29
	*/
	void setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief ����ʵ����Ƶ�����
		@param[in] show ���Ƶ�����
		@param[in] entity_id ʵ���id, Ĭ��Ϊ0, ��������ʵ��
		@return void
		@date 2019-11-21 08:45:01
	*/
	void setEntityScutcheonVisible(bool show, const long long & entity_id = 0);
#endif

	/***** ���ݽ��� *****/
#if 1
public:
	//��ճ���(�����ͼ��ʵ��ͺ�������)
	void clearGisScene();

	/**** ʵ�� ****/

	//����ʵ��ͼ��·��
	void setSvgDir(const std::string &);

	//���ʵ��
	void addEntity(const EntityInfo &);
	void addEntity(const std::vector<EntityInfo> &);

	//ɾ��ʵ��
	void removeEntity(const long long &);
	void removeEntity(const std::vector<long long> &);

	//����ʵ��
	void updateEntity(const EntityInfo &);
	void updateEntity(const std::vector<EntityInfo> &);
	void updateEntity(const std::unordered_map<long long, EntityInfo> &);

	//��������Ȧ�Ĳ��� add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	//ɾ������Ȧ add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	//����ɨ����� add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

	/**** ���� ****/

	//��Ӻ���
	void addRoute(const RouteInfo &);
	void addRoute(const std::vector<RouteInfo> &);

	//ɾ������
	void removeRoute(const long long &);
	void removeRoute(const std::vector<long long> &);

	//���º���
	void updateRoute(const RouteInfo &);
	void updateRoute(const std::vector<RouteInfo> &);

#endif

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

	//�ƶ���ͼ  ����¼�
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	//�����¼�
	void keyReleaseEvent(QKeyEvent *event);

private:
	//��ʼ����ͼλ��
	void initGisPos();
	//��ʼ���źŲ�
	void initSigAndSlt();

private:
	static GisLayerItem * static_instance;	//ͼ���ʵ��

	//��Χ�д�С
	QRectF mBoundingRect;

	//��¼���λ�� (�ƶ���ͼ)
	QPointF pressScenePos;

	//��Ƭ��
	MapItem * mTileLayer;

	//��ͼ��
	DrawItemLayer *mDrawItemLayer;

	//���߲�
	RouteAreaLayer *mRouteAreaLayer;

	//ʵ���
	EntityLayer *mEntityLayer;

	//��갴�µĳ���λ�ã������ж�����Ƿ��ƶ�
	QPointF mousePressPos;
};
