#pragma once
#include <GisStruct.h>
#include <QPainterPath>

/**
	@file GeoToQtMath.h
	@class GeoToQtMath
	@brief Qt��ͼʱ���þ�γ������ת����������Ҫ���Ƶ�ͼ��
	@author gwk
	@date 2019-11-18 14:01:03
*/
class GeoToQtMath
{
	GeoToQtMath() {}
	~GeoToQtMath() {}
public:

	/**
		@brief ͨ����γ�㼯�� ��������Qt��Ҫ���Ƶ�QPainterPath	
		@note ����һ������, ���մ��Բ���������Ӧ�Ĳ�ֵ�㣬����QPainterPath
		@param[in] geoPoints : ���������еĵ�
		@param[in] spacing : �����ֵ�ļ�� ��λ: m��ÿ�� spacing �׼���һ����ֵ�㣩
		@param[in] isClose : �����Ƿ�պ�
		@return QPainterPath ���ص㼯����ɵ�path
	*/
	static QPainterPath getInterpolatPath(const std::vector<GeoPoint> &geoPoints, double spacing, int level, bool isColse = false);


};

