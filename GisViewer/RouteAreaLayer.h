#pragma once

#include <QGraphicsObject>
#include <unordered_map>
#include "RouteItem.h"

/**
 *	@brief 航线层，所有操作航线的接口最终在这里实现
 */
class RouteAreaLayer : public QGraphicsObject
{
	//Q_OBJECT

public:
	RouteAreaLayer(QGraphicsItem *parent);
	~RouteAreaLayer();

	QRectF boundingRect() const;

	/***** 地图交互 *****/

	//进入/退出绘制航线 true:进入测距，false:退出测距
	void setDrawRouteMode(bool mode);

	// 设置是否可编辑
	void setEditMode(bool mode);

	//地图缩放 更新等级
	void updateLevel();

	//GisLayer传过来的事件
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);//鼠标释放
	void keyReleaseEvent(QKeyEvent *event);					//接收键盘事件 ESC 退出绘制

	/***** 图层控制 *****/

	/**
		@brief 设置航线的显隐
		@param[in] show 显隐
		@param[in] route_id 航线的id, 默认为0, 控制所有
		@return void
		@date 2019-11-18 16:38:04
	*/
	void setRouteVisible(bool show, const long long &route_id = 0);

	/**
		@brief 设置航线点的显隐
		@param[in] show 显隐
		@param[in] route_id 航线的id, 默认为0, 控制所有航线的点
		@return void
		@date 2019-11-22 13:44:58
	*/
	void setRoutePointVisible(bool show, const long long &route_id = 0);

	/***** 数据交互 *****/
	//清空场景(清除航线数据)
	void clearData();

	//添加航线
	void addRoute(const RouteInfo &);
	void addRoute(const std::vector<RouteInfo> &);

	//删除航线
	void removeRoute(const long long &);
	void removeRoute(const std::vector<long long> &);

	//更新航线
	void updateRoute(const RouteInfo &);
	void updateRoute(const std::vector<RouteInfo> &);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

	//实时获取鼠标位置
	void hoverMoveEvent(QGraphicsSceneHoverEvent *event);

private:
	std::unordered_map<long long, RouteItem *> mAllRoute;	//航线
	//bool m_LayerShow;
	//是否是绘制航线模式
	bool drawRouteMode;
	RouteItem *currentDraw;	//当前在画的航线
	//鼠标移动时的位置 （画航线时使用）
	QPointF mouseMoveLonlatPos;
};
