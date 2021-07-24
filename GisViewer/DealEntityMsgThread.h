#pragma once

#include <QThread>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include "GisEntity.h"

/**
	�޸ļ�¼��
		1. 2019-11-26 09:54:05: 1���޸�ʵ��ͼ��Ŀ¼ʱ��ֱ���޸ģ�û�г����һ��task��
								2��������Ϣ����

*/


/**
 * @brief ����ʵ����Ϣ���߳�
 * ��������У����ڲ���ʵ��Ľӿڻ�Ƶ�����ã�������Ż�������Ƶ�����õ��ظ���Ϣ
 */
class DealEntityMsgThread : public QThread
{
	Q_OBJECT

	typedef void (DealEntityMsgThread::*TaskFunc)(void *);

	//��ʵ��Ĳ����������һ�����񣬷��뵽�������ȥ����
	struct Task
	{
		Task() : param(nullptr) { }
		void * param;	//��������
		TaskFunc func;	//��������Ļص�����
	};

public:
	DealEntityMsgThread();
	~DealEntityMsgThread();
	
	//��ճ���(���ʵ������)
	void clearData();

	//���ʵ��
	void addEntity(const EntityInfo &);
	void addEntity(const std::vector<EntityInfo> &);

	//ɾ��ʵ��
	void removeEntity(const long long &);
	void removeEntity(const std::vector<long long> &);

	//����ʵ��
	void updateEntity(const EntityInfo &);
	void updateEntity(const std::vector<EntityInfo> &);
	void updateEntity(const std::unordered_map<long long, EntityInfo> &);

	//�޸�ʵ��ͼ��Ŀ¼
	void setSvgDir(const std::string &);

	//��������Ȧ�Ĳ��� add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	//ɾ������Ȧ add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	//����ɨ����� add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

	//ֹͣ�߳�
	void stopThread();

signals:
	void sigNewEntity(GisEntity);		//��������ӵ�ʵ��
	void sigRemoveEntity(long long);	//����ɾ����ʵ��
	void sigUpdateEntity(GisEntity);	//���͸��µ�ʵ��
	void sigNewCircle(long long, AbilityCircleParam);	//����ʵ������Ȧ
	void sigRmCircle(long long, std::string);			//ɾ��ʵ������Ȧ
	void sigNewScan(long long, ScanParam);				//����ɨ�����

private:
	// ��Ϣ������
	void pushTaskFilter(const long long &entity_id, const std::string &func, const Task &taskData);
	
	void addEntityFunc(void *);		//���ʵ��
	void removeEntityFunc(void *);	//ɾ��ʵ��
	void updateEntityFunc(void *);	//����ʵ��
	void changeSvgDirFunc(void *);	//�޸�ʵ��ͼ��·��

	//��������Ȧ�Ĳ��� add by gwk 2019/11/07
	void setCircleParamFunc(void *);

	//ɾ������Ȧ add by gwk 2019/11/07
	void removeCircleFunc(void *);

	//����ɨ����� add by gwk 2019/11/07
	void setScanParamFunc(void *);

private:
	bool setEntityImage(GisEntity &);	//����ʵ��ͼ��
	void setEntityPos(GisEntity &);		//����ʵ��λ��

	// ���ر���ͼ��
	QSvgRenderer * loadSvgIcon(const std::string & filePath, Color color);

protected:
	void run();

private:
	std::atomic_bool isRun;	//�߳�����״̬

	std::mutex taskMutex;
	//std::list<Task> mTask;	//����ʵ����������
	std::unordered_map<std::string, Task> mTask;	//����ʵ���������� modify by gwk 2019/11/26 ��Ϣ���� 

	std::unordered_map<long long, GisEntity> allEntity;	//���غõ�ʵ������

	std::string iconDir;	//ͼ��·��
	std::unordered_map<std::string, QByteArray> mAllLoadImage;	//���м��غõ�ͼ�� <key - ͼ��·�� value - ͼ���ļ����� >>
	std::unordered_map<std::string, QSvgRenderer *> colorIcon;	//����ɫ��svgͼ��
};
