#pragma once

#include <QThread>
#include <unordered_map>
#include <atomic>
#include <mutex>
#include "GisEntity.h"

/**
	修改记录：
		1. 2019-11-26 09:54:05: 1、修改实体图标目录时，直接修改，没有抽象成一个task。
								2、增加消息过滤

*/


/**
 * @brief 处理实体消息的线程
 * 仿真过程中，对于操作实体的接口会频繁调用，这个类优化过滤了频繁调用的重复消息
 */
class DealEntityMsgThread : public QThread
{
	Q_OBJECT

	typedef void (DealEntityMsgThread::*TaskFunc)(void *);

	//对实体的操作都抽象成一个任务，放入到任务队列去处理
	struct Task
	{
		Task() : param(nullptr) { }
		void * param;	//函数参数
		TaskFunc func;	//处理任务的回调函数
	};

public:
	DealEntityMsgThread();
	~DealEntityMsgThread();
	
	//清空场景(清除实体数据)
	void clearData();

	//添加实体
	void addEntity(const EntityInfo &);
	void addEntity(const std::vector<EntityInfo> &);

	//删除实体
	void removeEntity(const long long &);
	void removeEntity(const std::vector<long long> &);

	//更新实体
	void updateEntity(const EntityInfo &);
	void updateEntity(const std::vector<EntityInfo> &);
	void updateEntity(const std::unordered_map<long long, EntityInfo> &);

	//修改实体图标目录
	void setSvgDir(const std::string &);

	//设置能力圈的参数 add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	//删除能力圈 add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	//设置扫描参数 add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

	//停止线程
	void stopThread();

signals:
	void sigNewEntity(GisEntity);		//发送新添加的实体
	void sigRemoveEntity(long long);	//发送删除的实体
	void sigUpdateEntity(GisEntity);	//发送更新的实体
	void sigNewCircle(long long, AbilityCircleParam);	//设置实体能力圈
	void sigRmCircle(long long, std::string);			//删除实体能力圈
	void sigNewScan(long long, ScanParam);				//发送扫描参数

private:
	// 消息过滤器
	void pushTaskFilter(const long long &entity_id, const std::string &func, const Task &taskData);
	
	void addEntityFunc(void *);		//添加实体
	void removeEntityFunc(void *);	//删除实体
	void updateEntityFunc(void *);	//更新实体
	void changeSvgDirFunc(void *);	//修改实体图标路径

	//设置能力圈的参数 add by gwk 2019/11/07
	void setCircleParamFunc(void *);

	//删除能力圈 add by gwk 2019/11/07
	void removeCircleFunc(void *);

	//设置扫描参数 add by gwk 2019/11/07
	void setScanParamFunc(void *);

private:
	bool setEntityImage(GisEntity &);	//设置实体图标
	void setEntityPos(GisEntity &);		//设置实体位置

	// 加载本地图标
	QSvgRenderer * loadSvgIcon(const std::string & filePath, Color color);

protected:
	void run();

private:
	std::atomic_bool isRun;	//线程运行状态

	std::mutex taskMutex;
	//std::list<Task> mTask;	//处理实体的任务队列
	std::unordered_map<std::string, Task> mTask;	//处理实体的任务队列 modify by gwk 2019/11/26 消息过滤 

	std::unordered_map<long long, GisEntity> allEntity;	//加载好的实体数据

	std::string iconDir;	//图标路径
	std::unordered_map<std::string, QByteArray> mAllLoadImage;	//所有加载好的图标 <key - 图标路径 value - 图标文件数据 >>
	std::unordered_map<std::string, QSvgRenderer *> colorIcon;	//带颜色的svg图标
};
