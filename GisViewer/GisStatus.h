#pragma once
#include <GisStruct.h>
#include <atomic>

//��ͼ״̬
class GisStatus
{
	GisStatus();
	GisStatus(const GisStatus &) = delete;
	~GisStatus();
public:
	static GisStatus & instance();

	//��ȡ��ͼ�Ĳ���ģʽ
	const EnumMapMode getGisMode() const;

	//���õ�ͼ����ģʽ
	void setGisMode(EnumMapMode);

	//��ǰ���ص���Ƭ�ȼ�  ���ǵ�ͼ�ȼ��� һ����㶼�����
	//int getTileLevel() const;
	//void setTileLevel(int);

	//��ͼ�ȼ�
	int getGisLevel() const;
	void setGisLevel(int);

	//����/��ȡ ����ʱ��
	void setCurrentSimulationTime(double simSec);
	const double getCurrentSimulationTime() const;

private:
	EnumMapMode mode;			//��ͼ����ģʽ
	std::atomic<int> gisLevel;	//��ͼ�ȼ�
	std::atomic<double> simTime;	//����ʱ��
};
