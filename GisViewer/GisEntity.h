#pragma once
#include <GisStruct.h>
#include <QPoint>
#include <QSvgRenderer>

//实体图标大小
const QRectF EntityIconSize(-20, -20, 40, 40);

struct GisEntity
{
	GisEntity() : svgIcon(nullptr) {}
	GisEntity(const EntityInfo &data) : realData(data), svgIcon(nullptr) {}

	EntityInfo realData;	//原始数据
	QPoint pixelPos;		//像素坐标
	QSvgRenderer * svgIcon;	//图标
};

