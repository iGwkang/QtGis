#pragma once

#include <QObject>

# ifdef GISVIEWER_LIB
#  define GISVIEWER_EXPORT __declspec(dllexport)
# else
#  define GISVIEWER_EXPORT __declspec(dllimport)
# endif

#include <GisStruct.h>

//地图上所有信号都由 该类实例发出
class GISVIEWER_EXPORT GisSignals : public QObject
{
	Q_OBJECT

	GisSignals(const GisSignals &);
	GisSignals & operator=(const GisSignals &);

	GisSignals() {}
	~GisSignals() {}
public:
	static inline GisSignals * instance()
	{
		static GisSignals _instance;
		return &_instance;
	}

signals:
	// 结束测距 发出信号
	void finishMeasureMode();

	// 结束绘制航线 新绘制的航线由该信号发出
	void finishDrawRoute(const RouteInfo &);

	// 地图拖动航线上的点 航线被修改
	void gisUpdateRoute(const RouteInfo &);

	// 实体被鼠标右键点击 screenX, screenY 屏幕坐标
	void sigEntityRightClicked(long long id, int screenX, int screenY);

	// 实体被二维地图拖动，更新坐标
	void gisMoveEntity(long long id, const GeoPoint &newGeoPos);

	// 接收鼠标拖放事件、添加实体、返回添加的实体数据，地图内部只是给定实体的位置，实体其余信息由业务层填写
	void sigNewEntity(const EntityInfo &);
};
