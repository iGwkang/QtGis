#pragma once
#include <GisStruct.h>
#include <QPoint>
#include <QSvgRenderer>

//ʵ��ͼ���С
const QRectF EntityIconSize(-20, -20, 40, 40);

struct GisEntity
{
	GisEntity() : svgIcon(nullptr) {}
	GisEntity(const EntityInfo &data) : realData(data), svgIcon(nullptr) {}

	EntityInfo realData;	//ԭʼ����
	QPoint pixelPos;		//��������
	QSvgRenderer * svgIcon;	//ͼ��
};

