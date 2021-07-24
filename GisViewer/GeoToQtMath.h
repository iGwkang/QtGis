#pragma once
#include <GisStruct.h>
#include <QPainterPath>

/**
	@file GeoToQtMath.h
	@class GeoToQtMath
	@brief Qt绘图时，用经纬点坐标转换成最终需要绘制的图像
	@author gwk
	@date 2019-11-18 14:01:03
*/
class GeoToQtMath
{
	GeoToQtMath() {}
	~GeoToQtMath() {}
public:

	/**
		@brief 通过经纬点集合 返回最终Qt需要绘制的QPainterPath	
		@note 给定一条航线, 按照大地圆弧计算出相应的插值点，返回QPainterPath
		@param[in] geoPoints : 航线中所有的点
		@param[in] spacing : 计算插值的间距 单位: m（每隔 spacing 米计算一个插值点）
		@param[in] isClose : 航线是否闭合
		@return QPainterPath 返回点集合组成的path
	*/
	static QPainterPath getInterpolatPath(const std::vector<GeoPoint> &geoPoints, double spacing, int level, bool isColse = false);


};

