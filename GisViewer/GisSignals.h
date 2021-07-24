#pragma once

#include <QObject>

# ifdef GISVIEWER_LIB
#  define GISVIEWER_EXPORT __declspec(dllexport)
# else
#  define GISVIEWER_EXPORT __declspec(dllimport)
# endif

#include <GisStruct.h>

//��ͼ�������źŶ��� ����ʵ������
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
	// ������� �����ź�
	void finishMeasureMode();

	// �������ƺ��� �»��Ƶĺ����ɸ��źŷ���
	void finishDrawRoute(const RouteInfo &);

	// ��ͼ�϶������ϵĵ� ���߱��޸�
	void gisUpdateRoute(const RouteInfo &);

	// ʵ�屻����Ҽ���� screenX, screenY ��Ļ����
	void sigEntityRightClicked(long long id, int screenX, int screenY);

	// ʵ�屻��ά��ͼ�϶�����������
	void gisMoveEntity(long long id, const GeoPoint &newGeoPos);

	// ��������Ϸ��¼������ʵ�塢������ӵ�ʵ�����ݣ���ͼ�ڲ�ֻ�Ǹ���ʵ���λ�ã�ʵ��������Ϣ��ҵ�����д
	void sigNewEntity(const EntityInfo &);
};
