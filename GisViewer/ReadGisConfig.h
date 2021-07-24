#pragma once
#include <string>

struct GisConfigData;

//��ȡ��ͼ�����ļ�
class ReadGisConfig
{
	ReadGisConfig();
	~ReadGisConfig();
public:
	static inline ReadGisConfig & instance()
	{
		static ReadGisConfig conf;
		return conf;
	}

	//��ȡ�����ļ�
	void readConfig(const std::string &confPath);

	//��ȡ��ͼ���ȼ�
	unsigned getGisMaxLevel();
	//��ȡ��ͼ��С�ȼ�
	unsigned getGisMinLevel();

	//��ȡ��ͼ��Ƭ·��
	const std::string & getTilePath();

	//��ȡʵ��ͼ��Ŀ¼
	const std::string & getIconDir();

	//��ȡ��ͼ��ʼ��ʱ�ĵȼ������ȣ�γ��
	int getInitLevel();
	bool getGisInitPos(double & initLon, double & initLat);

private:
	GisConfigData *mPtr;
};
