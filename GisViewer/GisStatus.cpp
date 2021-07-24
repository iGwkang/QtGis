#include "GisStatus.h"
#include "GisLayerItem.h"
#include "GisSignals.h"

GisStatus::GisStatus()
	: mode(MoveMapMode)
{

}


GisStatus::~GisStatus()
{
}

GisStatus & GisStatus::instance()
{
	static GisStatus _instance;
	return _instance;
}

const EnumMapMode GisStatus::getGisMode() const
{
	return mode;
}

void GisStatus::setGisMode(EnumMapMode m)
{
	//�����ǰ�����ƶ���ͼģʽ����Ҫ���˳�����ģʽ��״̬
	if (mode != MoveMapMode)
	{
		switch (mode)
		{
		case MeasureMode:	//���
			GisLayerItem::getInstance()->setMeasureMode(false);
			break;
		case DrawRouteLineMode:	//���ƺ���
			GisLayerItem::getInstance()->setDrawRouteMode(false);
			break;
		default:
			break;
		}
	}

	mode = m;

	switch (mode)
	{
	case MeasureMode:	//���
		GisLayerItem::getInstance()->setMeasureMode(true);
		break;
	case DrawRouteLineMode:	//���ƺ���
		GisLayerItem::getInstance()->setDrawRouteMode(true);
		break;
	//case SimRunMode:
	//	GisLayerItem::getInstance()->setEditMode(false);
	//	break;
	//case MoveMapMode:
	//	GisLayerItem::getInstance()->setEditMode(true);
	//	break;
	default:
		break;
	}
}

//int GisStatus::getTileLevel() const
//{
//	return tileLevel;
//}
//
//void GisStatus::setTileLevel(int level)
//{
//	tileLevel = level;
//}

int GisStatus::getGisLevel() const
{
	return gisLevel;
}

void GisStatus::setGisLevel(int level)
{
	gisLevel = level;
}

void GisStatus::setCurrentSimulationTime(double simSec)
{
	simTime = simSec;
}

const double GisStatus::getCurrentSimulationTime() const
{
	return simTime;
}
