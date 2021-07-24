#pragma once

#include <QGraphicsObject>
#include <unordered_map>
#include "RouteItem.h"

/**
 *	@brief ���߲㣬���в������ߵĽӿ�����������ʵ��
 */
class RouteAreaLayer : public QGraphicsObject
{
	//Q_OBJECT

public:
	RouteAreaLayer(QGraphicsItem *parent);
	~RouteAreaLayer();

	QRectF boundingRect() const;

	/***** ��ͼ���� *****/

	//����/�˳����ƺ��� true:�����࣬false:�˳����
	void setDrawRouteMode(bool mode);

	// �����Ƿ�ɱ༭
	void setEditMode(bool mode);

	//��ͼ���� ���µȼ�
	void updateLevel();

	//GisLayer���������¼�
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);//����ͷ�
	void keyReleaseEvent(QKeyEvent *event);					//���ռ����¼� ESC �˳�����

	/***** ͼ����� *****/

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

	/***** ���ݽ��� *****/
	//��ճ���(�����������)
	void clearData();

	//��Ӻ���
	void addRoute(const RouteInfo &);
	void addRoute(const std::vector<RouteInfo> &);

	//ɾ������
	void removeRoute(const long long &);
	void removeRoute(const std::vector<long long> &);

	//���º���
	void updateRoute(const RouteInfo &);
	void updateRoute(const std::vector<RouteInfo> &);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

	//ʵʱ��ȡ���λ��
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
	std::unordered_map<long long, RouteItem *> mAllRoute;	//����
	//bool m_LayerShow;
	//�Ƿ��ǻ��ƺ���ģʽ
	bool drawRouteMode;
	RouteItem *currentDraw;	//��ǰ�ڻ��ĺ���
	//����ƶ�ʱ��λ�� ��������ʱʹ�ã�
	QPointF mouseMoveLonlatPos;
};
