#pragma once

#include <QGraphicsItem>
#include <QGraphicsScene>
#include <unordered_map>
#include <vector>

class MapItem;			//瓦片层
class DrawItemLayer;	//绘图层
class RouteAreaLayer;	//航线层
class EntityLayer;		//实体层

struct EntityInfo;
struct RouteInfo;
struct AbilityCircleParam;
struct ScanParam;

/**
 * @brief 地图上的图层，包括瓦片层，航线层，实体层。对图层的操作有：交互功能，数据传递
 */
class GisLayerItem : public QGraphicsObject
{
	//Q_OBJECT

public:
	//获取图层实例
	static GisLayerItem *getInstance();

	GisLayerItem(QGraphicsItem * parent = nullptr);
	~GisLayerItem();

	/***** 地图功能 *****/
#if 1
public:
	//包围盒大小
	QRectF boundingRect() const;

	//通过场景坐标获取经纬度
	QPointF getLonLatFromScenePos(const QPointF & scenePos);
	//通过经纬度获取场景坐标
	QPointF getScenePosFromLonLat(const QPointF & lonlatPos);
	//获取地图等级
	//int getGisLevel() const;

	//缩放
	void zoom(int delta);

#endif

	/***** 交互功能 *****/
#if 1
public:
	//进入/退出测距
	void setMeasureMode(bool mode);


	//进入/退出绘制航线 true:进入测距，false:退出测距
	void setDrawRouteMode(bool mode);


	//设置地图图元是否可编辑 (仿真运行过程中 地图不可编辑)
	void setEditMode(bool mode);

#endif

	/***** 图层控制 *****/
	//TODO 需要一个整体的图层显隐管控
#if 1
	//设置测距的显隐
	void setMeasureVisible(bool show);

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

	/**
		@brief 设置实体图标的显隐 (同时会隐藏实体上的能力圈和扫描效果)
		@param[in] show 显隐
		@param[in] entity_id 实体的id, 默认为0, 控制所有
		@return void
		@date 2019-11-18 16:50:59
	*/
	void setEntityVisible(bool show, const long long &entity_id = 0);

	/**
		@brief 设置实体能力圈的显隐
		@param[in] show 显隐
		@param[in] entity_id 实体的id
		@param[in] circleType 实体的上能力圈的类型
		@return void
		@date 2019-11-18 16:38:13
	*/
	void setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief 设置实体能力圈扫描开关
		@param[in] show 显隐
		@param[in] entity_id 实体的id
		@param[in] circleType 实体的上能力圈的类型
		@return void
		@date 2019-11-18 16:38:29
	*/
	void setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief 设置实体标牌的显隐
		@param[in] show 标牌的显隐
		@param[in] entity_id 实体的id, 默认为0, 控制所有实体
		@return void
		@date 2019-11-21 08:45:01
	*/
	void setEntityScutcheonVisible(bool show, const long long & entity_id = 0);
#endif

	/***** 数据交互 *****/
#if 1
public:
	//清空场景(清除地图上实体和航线数据)
	void clearGisScene();

	/**** 实体 ****/

	//设置实体图标路径
	void setSvgDir(const std::string &);

	//添加实体
	void addEntity(const EntityInfo &);
	void addEntity(const std::vector<EntityInfo> &);

	//删除实体
	void removeEntity(const long long &);
	void removeEntity(const std::vector<long long> &);

	//更新实体
	void updateEntity(const EntityInfo &);
	void updateEntity(const std::vector<EntityInfo> &);
	void updateEntity(const std::unordered_map<long long, EntityInfo> &);

	//设置能力圈的参数 add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	//删除能力圈 add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	//设置扫描参数 add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

	/**** 航线 ****/

	//添加航线
	void addRoute(const RouteInfo &);
	void addRoute(const std::vector<RouteInfo> &);

	//删除航线
	void removeRoute(const long long &);
	void removeRoute(const std::vector<long long> &);

	//更新航线
	void updateRoute(const RouteInfo &);
	void updateRoute(const std::vector<RouteInfo> &);

#endif

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr);

	//移动地图  鼠标事件
	void mousePressEvent(QGraphicsSceneMouseEvent *event);
	void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
	void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

	//键盘事件
	void keyReleaseEvent(QKeyEvent *event);

private:
	//初始化地图位置
	void initGisPos();
	//初始化信号槽
	void initSigAndSlt();

private:
	static GisLayerItem * static_instance;	//图层的实例

	//包围盒大小
	QRectF mBoundingRect;

	//记录鼠标位置 (移动地图)
	QPointF pressScenePos;

	//瓦片层
	MapItem * mTileLayer;

	//绘图层
	DrawItemLayer *mDrawItemLayer;

	//航线层
	RouteAreaLayer *mRouteAreaLayer;

	//实体层
	EntityLayer *mEntityLayer;

	//鼠标按下的场景位置，用于判断鼠标是否移动
	QPointF mousePressPos;
};
