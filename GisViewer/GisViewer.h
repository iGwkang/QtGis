#pragma once

# ifdef GISVIEWER_LIB
#  define GISVIEWER_EXPORT __declspec(dllexport)
# else
#  define GISVIEWER_EXPORT __declspec(dllimport)
# endif

#include <QGraphicsView>
#include <GisStruct.h>

class GisViewerPrivate;

/**
	@file GisViewer.h
	@class GisViewer
	@brief 地图视口，地图上对外提供的操作接口
	@author gwk
*/
class GISVIEWER_EXPORT GisViewer : public QGraphicsView
{
	Q_OBJECT

	GisViewer(const GisViewer &);
	GisViewer& operator=(const GisViewer &);
public:
	GisViewer(QWidget * parent = nullptr);
	~GisViewer();

public slots:
	// 清空场景(清除地图上实体和航线数据)
	void clearGisScene();

	// 设置当前仿真时间
	void setCurrentSimulationTime(double simSec);
	
	// 设置是否进入仿真状态 （进入仿真状态时，实体、航线都不能移动编辑）
	void setSimStatus(bool mode);

	/***** 地图交互功能 *****/
	
	//进入/退出测距
	void setMeasureMode(bool mode);

	//进入/退出绘制航线 true:进入，false:退出
	void setDrawRouteMode(bool mode);

	/***** 图层控制 *****/

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

	/***** 数据交互 *****/

	/**** 实体操作(线程安全) ****/


	// 设置实体图标目录 add by gwk 2019/11/06
	void setSvgDir(const std::string &);

	// 添加实体
	bool addEntity(const EntityInfo &);
	bool addEntity(const std::vector<EntityInfo> &);

	// 删除实体
	bool removeEntity(const long long &);
	int removeEntity(const std::vector<long long> &);

	// 更新实体
	bool updateEntity(const EntityInfo &);
	int updateEntity(const std::vector<EntityInfo> &);
	void updateEntity(const std::unordered_map<long long, EntityInfo> &);

	// 获取实体
	bool getEntity(const long long &, EntityInfo &);	//根据id获取实体
	void getEntity(const std::vector<long long> &, std::unordered_map<long long, EntityInfo> &);
	std::unordered_map<long long, EntityInfo> getAllEntity();	//获取所有实体

	// 设置能力圈的参数 add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	// 删除能力圈 add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	// 设置扫描参数 add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

	/**
		@brief 获取给实体设置过的能力圈数据
		@param[in] entity_id: 实体id
		@param[out] acData : 返回实体里面所有的能力圈 { key - 能力圈类型 value - 能力圈参数 }
		@return bool 实体存在并且实体里有能力圈 返回 true，否则返回false
		@date 2019-11-25 09:05:57
	*/
	bool getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData);

	/**
		@brief 获取所有设置过的能力圈数据
		@param void
		@return std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> { key - 实体id value - { key - 能力圈类型 value - 能力圈参数数据 } }
		@date 2019-11-25 09:05:57
	*/
	std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> getAbilityCircleData();

	/**** 航线(只能在主线程调用) ****/
	// 添加航线
	bool addRoute(const RouteInfo &);
	bool addRoute(const std::vector<RouteInfo> &);

	// 删除航线
	bool removeRoute(const long long &);
	int removeRoute(const std::vector<long long> &);

	// 更新航线
	bool updateRoute(const RouteInfo &);
	int updateRoute(const std::vector<RouteInfo> &);

	// 获取航线
	bool getRoute(const long long &, RouteInfo &);	//根据id获取航线
	void getRoute(const std::vector<long long> &, std::unordered_map<long long, RouteInfo> &);
	std::unordered_map<long long, RouteInfo> getAllRoute();	//获取所有航线

protected:
	// 窗口大小改变触发事件
	void resizeEvent(QResizeEvent *event);

	// 鼠标移动事件
	void mouseMoveEvent(QMouseEvent *event);

	// 鼠标滚轮事件 触发缩放
	void wheelEvent(QWheelEvent *event);

private:
	void zoom(int delta);	//缩放
	void initUi();
	void initSigAndSlt();	//初始化信号槽

private:
	GisViewerPrivate * mPtr;
};
