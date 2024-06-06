#include "DealEntityMsgThread.h"
#include "ReadGisConfig.h"
#include "TileSystem.h"
#include "GisStatus.h"
#include <QCoreApplication>
#include <iostream>
#include <QDomDocument>
#include <QFile>
#include <QColor>

//设置能力圈 add by gwk 2019/11/07
struct SetCircleParamTask
{
	SetCircleParamTask(long long id, const AbilityCircleParam &param)
		: entity_id(id), circleParam(param) { }
	long long entity_id;
	AbilityCircleParam circleParam;
};

//删除能力圈 add by gwk 2019/11/07
struct RemoveCircleTask
{
	RemoveCircleTask(long long id, const std::string &type)
		: entity_id(id), circleType(type) { }
	long long entity_id;
	std::string circleType;
};

//设置扫描参数 add by gwk 2019/11/07
struct SetScanParamTask
{
	SetScanParamTask(long long id, const ScanParam &param)
		: entity_id(id), scanParam(param) { }
	long long entity_id;
	ScanParam scanParam;
};


DealEntityMsgThread::DealEntityMsgThread()
	: isRun(true), iconDir(ReadGisConfig::instance().getIconDir())
{
}

DealEntityMsgThread::~DealEntityMsgThread()
{
	clearData();
	for (auto & svg : colorIcon)
	{
		delete svg.second;
	}
	colorIcon.clear();
}

void DealEntityMsgThread::clearData()
{
	std::unique_lock<std::mutex> lock(taskMutex);

	//清空内存
	for (auto & task : mTask)
	{
		delete task.second.param;
	}
	//删除所有的任务
	mTask.clear();

	//清空所有实体
	allEntity.clear();
}

void DealEntityMsgThread::addEntity(const EntityInfo &data)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	Task task;
	task.param = new EntityInfo(data);
	task.func = &DealEntityMsgThread::addEntityFunc;
	
	pushTaskFilter(data.ID, __FUNCTION__, task);
	//mTask.push_back(task);
}

void DealEntityMsgThread::addEntity(const std::vector<EntityInfo> &data)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	for (auto & entitiy : data)
	{
		Task task;
		task.param = new EntityInfo(entitiy);
		task.func = &DealEntityMsgThread::addEntityFunc;
		
		pushTaskFilter(entitiy.ID, __FUNCTION__, task);
		//mTask.push_back(task);
	}

}

void DealEntityMsgThread::removeEntity(const long long &id)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	Task task;
	task.param = new long long(id);
	task.func = &DealEntityMsgThread::removeEntityFunc;

	pushTaskFilter(id, __FUNCTION__, task);
	//mTask.push_back(task);
}

void DealEntityMsgThread::removeEntity(const std::vector<long long> &ids)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	for (auto &id : ids)
	{
		Task task;
		task.param = new long long(id);
		task.func = &DealEntityMsgThread::removeEntityFunc;

		pushTaskFilter(id, __FUNCTION__, task);
		//mTask.push_back(task);
	}
	
}

void DealEntityMsgThread::updateEntity(const EntityInfo &entity)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	Task task;
	task.param = new EntityInfo(entity);
	task.func = &DealEntityMsgThread::updateEntityFunc;

	pushTaskFilter(entity.ID, __FUNCTION__, task);
	//mTask.push_back(task);
}

void DealEntityMsgThread::updateEntity(const std::vector<EntityInfo> &entitys)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	for (auto & entity : entitys)
	{
		Task task;
		task.param = new EntityInfo(entity);
		task.func = &DealEntityMsgThread::updateEntityFunc;

		pushTaskFilter(entity.ID, __FUNCTION__, task);
		//mTask.push_back(task);
	}

}

void DealEntityMsgThread::updateEntity(const std::unordered_map<long long, EntityInfo> &entitys)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	for (auto & _iter : entitys)
	{
		Task task;
		task.param = new EntityInfo(_iter.second);
		task.func = &DealEntityMsgThread::updateEntityFunc;

		pushTaskFilter(_iter.second.ID, __FUNCTION__, task);
		//mTask.push_back(task);
	}
}

void DealEntityMsgThread::setSvgDir(const std::string &dir)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	if (dir == iconDir)
		return;

	iconDir = dir;

	//Task task;
	//task.param = new std::string(dir);
	//task.func = &DealEntityMsgThread::changeSvgDirFunc;
	//mTask.push_back(task);

	for (auto & _iter : allEntity)
	{
		if (setEntityImage(_iter.second))
		{
			emit sigUpdateEntity(_iter.second);
		}
	}
}

void DealEntityMsgThread::setCircleParam(long long entity_id, const AbilityCircleParam &param)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	Task task;
	task.param = new SetCircleParamTask(entity_id, param);
	task.func = &DealEntityMsgThread::setCircleParamFunc;

	pushTaskFilter(entity_id, __FUNCTION__, task);
	//mTask.push_back(task);
}

void DealEntityMsgThread::removeCircle(long long entity_id, const std::string &circleType)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	Task task;
	task.param = new RemoveCircleTask(entity_id, circleType);
	task.func = &DealEntityMsgThread::removeCircleFunc;

	pushTaskFilter(entity_id, __FUNCTION__, task);
	//mTask.push_back(task);
}

void DealEntityMsgThread::setScanParam(long long entity_id, const ScanParam &param)
{
	std::unique_lock<std::mutex> lock(taskMutex);

	Task task;
	task.param = new SetScanParamTask(entity_id, param);
	task.func = &DealEntityMsgThread::setScanParamFunc;

	//mTask.push_back(task);
	pushTaskFilter(entity_id, __FUNCTION__, task);
}

void DealEntityMsgThread::stopThread()
{
	isRun = false;
}

void DealEntityMsgThread::pushTaskFilter(const long long &entity_id, const std::string &func, const Task &taskData)
{
	//this->*taskData.func;
	// 生成一个唯一标识
	std::string key = std::to_string(entity_id) + func;

	auto _find = mTask.find(key);

	// 如果key不存在，添加进去 
	if (_find == mTask.end())
	{
		mTask[key] = taskData;
	}
	// 如果key存在，需要删除原来的task，把新的task 存入到map中
	else
	{
		delete _find->second.param;
		_find->second = taskData;
	}
}

void DealEntityMsgThread::addEntityFunc(void *param)
{
	EntityInfo * entity = reinterpret_cast<EntityInfo *>(param);
	if (!entity)
		return;

	if (allEntity.find(entity->ID) != allEntity.end())
		return;

	GisEntity newEntity(*entity);

	//加载图片
	if (!setEntityImage(newEntity))
		return;

	//设置位置
	setEntityPos(newEntity);
	
	allEntity[entity->ID] = newEntity;
	//发送信号
	emit sigNewEntity(newEntity);
}

void DealEntityMsgThread::removeEntityFunc(void *param)
{
	long long *id = reinterpret_cast<long long *>(param);
	if (!id)
		return;

	if (allEntity.find(*id) == allEntity.end())
		return;

	allEntity.erase(*id);

	emit sigRemoveEntity(*id);
}

void DealEntityMsgThread::updateEntityFunc(void *param)
{
	EntityInfo * entity = reinterpret_cast<EntityInfo *>(param);
	if (!entity)
		return;

	auto _iter = allEntity.find(entity->ID);
	if (_iter == allEntity.end())
		return;

	GisEntity newEntity(*entity);

	//加载图片
	if (!setEntityImage(newEntity))
		return;

	//设置位置
	setEntityPos(newEntity);

	_iter->second = newEntity;

	//发送信号
	emit sigUpdateEntity(newEntity);
}

void DealEntityMsgThread::changeSvgDirFunc(void *param)
{
	std::string * dir = reinterpret_cast<std::string *>(param);
	if (!dir)
		return;

	iconDir = *dir;

	for (auto & _iter : allEntity)
	{
		if (setEntityImage(_iter.second))
		{
			emit sigUpdateEntity(_iter.second);
		}
	}
}

void DealEntityMsgThread::setCircleParamFunc(void *param)
{
	SetCircleParamTask * task = reinterpret_cast<SetCircleParamTask *>(param);
	if (!task)
		return;

	//判断实体是否存在
	auto _iter = allEntity.find(task->entity_id);
	if (_iter == allEntity.end())
		return;

	emit sigNewCircle(task->entity_id, task->circleParam);
}

void DealEntityMsgThread::removeCircleFunc(void *param)
{
	RemoveCircleTask * task = reinterpret_cast<RemoveCircleTask *>(param);
	if (!task)
		return;

	//判断实体是否存在
	auto _iter = allEntity.find(task->entity_id);
	if (_iter == allEntity.end())
		return;

	emit sigRmCircle(task->entity_id, task->circleType);
}

void DealEntityMsgThread::setScanParamFunc(void *param)
{
	SetScanParamTask * task = reinterpret_cast<SetScanParamTask *>(param);
	if (!task)
		return;

	//判断实体是否存在
	auto _iter = allEntity.find(task->entity_id);
	if (_iter == allEntity.end())
		return;

	emit sigNewScan(task->entity_id, task->scanParam);
}

bool DealEntityMsgThread::setEntityImage(GisEntity &entity)
{
	QSvgRenderer *svg = loadSvgIcon(iconDir + entity.realData.iconPath, entity.realData.color);
	if (svg)
	{
		entity.svgIcon = svg;
		return true;
	}
	
	return false;
}

void DealEntityMsgThread::setEntityPos(GisEntity &entity)
{
	TileSystem::LonLatToPixelXY(entity.realData.position.longitude, entity.realData.position.latitude, GisStatus::instance().getGisLevel(), entity.pixelPos.rx(), entity.pixelPos.ry());
}

void setAttrOpacityValue(QDomElement &ele, const QString &targetName, const QString &attrValue)
{
	if (ele.tagName() == targetName)
	{
		if (ele.hasAttribute("fill-opacity"))
		{
			bool ok = false;
			ele.attribute("fill-opacity").toInt(&ok);
			if (!ok)
			{
				ele.setAttribute("fill-opacity", attrValue);
			}
		}
		else
		{
			ele.setAttribute("fill-opacity", attrValue);
		}
	}

	for (int i = 0; i < ele.childNodes().size(); ++i)
	{
		if (ele.childNodes().at(i).isElement())
		{
			QDomNode child = ele.childNodes().at(i);
			QDomElement element = child.toElement();
			setAttrOpacityValue(element, targetName, attrValue);
		}
	}
}

void setAttrValue(QDomElement &ele, const QString &targetName, const QString &attr, const QString &attrValue)
{
	if (ele.tagName() == targetName)
	{
		ele.setAttribute(attr, attrValue);
	}

	for (int i = 0; i < ele.childNodes().size(); ++i)
	{
		if (ele.childNodes().at(i).isElement())
		{
			QDomNode child = ele.childNodes().at(i);
			QDomElement element = child.toElement();
			setAttrValue(element, targetName, attr, attrValue);
		}
	}
}

QSvgRenderer * DealEntityMsgThread::loadSvgIcon(const std::string & filePath, Color color)
{
	QString colorName = QColor(color.r, color.g, color.b).name();
	std::string key = filePath + ':' + colorName.toStdString() + std::to_string(color.a);

	//查看是否已经加载
	if (mAllLoadImage.find(filePath) != mAllLoadImage.end())
	{
		//判断是否已经设置好颜色
		if (colorIcon.find(key) != colorIcon.end())
		{
			return colorIcon[key];
		}

		//如果没有设置好颜色 
		QDomDocument doc;

		if (!doc.setContent(mAllLoadImage[filePath]))
			return nullptr;

		auto docEle = doc.documentElement();
		//设置颜色
		setAttrValue(docEle, "path", "fill", colorName);
		setAttrValue(docEle, "ellipse", "fill", colorName);
		setAttrValue(docEle, "circle", "fill", colorName);
		setAttrValue(docEle, "rect", "fill", colorName);
		setAttrValue(docEle, "line", "fill", colorName);
		setAttrValue(docEle, "polygon", "fill", colorName);

		QSvgRenderer * svg = new QSvgRenderer;

		if (svg->load(doc.toByteArray()))
		{
			colorIcon[key] = svg;
			return svg;
		}
		else
		{
			std::cout << "实体图标加载失败！ File:" << filePath << __FUNCTION__ << " line:" << __LINE__  << std::endl;
			delete svg;
			return nullptr;
		}
	}
	else
	{
		QFile file(filePath.c_str());
		if (!file.open(QIODevice::ReadOnly))
		{
			std::cout << "实体图标加载失败！ File:" << filePath << __FUNCTION__ << " line:" << __LINE__ << std::endl;
			return nullptr;
		}
		QDomDocument doc;
		if (!doc.setContent(&file)) {
			file.close();
			std::cout << "实体图标加载失败！ File:" << filePath << __FUNCTION__ << " line:" << __LINE__ << std::endl;
			return nullptr;
		}
		file.close();

		QString alpha = QString().setNum(color.a / 255.0);

		auto docEle = doc.documentElement();

		//矫正透明度
		setAttrOpacityValue(docEle, "path", alpha);
		setAttrOpacityValue(docEle, "ellipse", alpha);
		setAttrOpacityValue(docEle, "circle", alpha);
		setAttrOpacityValue(docEle, "rect", alpha);
		setAttrOpacityValue(docEle, "line", alpha);
		setAttrOpacityValue(docEle, "polygon", alpha);

		mAllLoadImage[filePath] = doc.toByteArray();

		//设置颜色
		setAttrValue(docEle, "path", "fill", colorName);
		setAttrValue(docEle, "ellipse", "fill", colorName);
		setAttrValue(docEle, "circle", "fill", colorName);
		setAttrValue(docEle, "rect", "fill", colorName);
		setAttrValue(docEle, "line", "fill", colorName);
		setAttrValue(docEle, "polygon", "fill", colorName);

		// 描边
		setAttrValue(docEle, "path", "stroke", "#000000");
		setAttrValue(docEle, "path", "stroke-opacity", "1");
		setAttrValue(docEle, "path", "stroke-width", "2px");

		setAttrValue(docEle, "ellipse", "stroke", "#000000");
		setAttrValue(docEle, "ellipse", "stroke-opacity", "1");
		setAttrValue(docEle, "ellipse", "stroke-width", "2px");

		setAttrValue(docEle, "circle", "stroke", "#000000");
		setAttrValue(docEle, "circle", "stroke-opacity", "1");
		setAttrValue(docEle, "circle", "stroke-width", "2px");

		setAttrValue(docEle, "rect", "stroke", "#000000");
		setAttrValue(docEle, "rect", "stroke-opacity", "1");
		setAttrValue(docEle, "rect", "stroke-width", "2px");

		setAttrValue(docEle, "line", "stroke", "#000000");
		setAttrValue(docEle, "line", "stroke-opacity", "1");
		setAttrValue(docEle, "line", "stroke-width", "2px");

		setAttrValue(docEle, "polygon", "stroke", "#000000");
		setAttrValue(docEle, "polygon", "stroke-opacity", "1");
		setAttrValue(docEle, "polygon", "stroke-width", "2px");

		QSvgRenderer * svg = new QSvgRenderer;

		if (svg->load(doc.toByteArray()))
		{
			colorIcon[key] = svg;
			return svg;
		}
		else
		{
			std::cout << "实体图标加载失败！ File:" << filePath << __FUNCTION__ << " line:" << __LINE__ << std::endl;
			delete svg;
			return nullptr;
		}
	}
}

#if 0
#include <thread>
void handleFunc(DealEntityMsgThread *inst, std::atomic_bool & isRun, std::list<DealEntityMsgThread::Task> &mTask, std::mutex &taskMutex)
{
	while (isRun)
	{
		long st = clock();
		{
			std::unique_lock<std::mutex> lock(taskMutex);
			//循环处理 线程任务
			while (!mTask.empty())
			{
				auto & _begin = mTask.front();
				(inst->*_begin.func)(_begin.param);
				delete _begin.param;	//delete void* 不会调用析构函数 如果指针中还含有指针指向堆区内存，可能无法释放（可能存在内存泄漏）by gwk
				mTask.pop_front();
			}
		}

		//QCoreApplication::processEvents();
		long cpu_use_time = clock() - st;	//本次循环cpu用时 ms
											//std::cout << ">>>>>>> cpu_use_time:" << clock() - st << std::endl;
		if (cpu_use_time < 10)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(10 - cpu_use_time));
		}
		
	}	
}
#endif

void DealEntityMsgThread::run()
{
//	std::thread thr(handleFunc, this, std::ref(isRun), std::ref(mTask) , std::ref(taskMutex));
//	thr.join();
//	return;

	// by gwk 2019-11-26 09:42:36
	// 如果其他线程以非常快的速度调用接口，会导致Qt消息膨胀，主线程处理不过来，程序会卡死
	// 需要做消息过滤的处理
	// 唯一性分析：每一个task数据param里面都会有一个id，然后每一个task又会对应一个回调函数，通过这两个参数，可以生成一个唯一性的id，过滤重复的数据
	while (isRun)
	{
		long st = clock();

		{
			std::unique_lock<std::mutex> lock(taskMutex);
			//循环处理 线程任务

			// @date 2019/11/26 没有消息过滤机制
			//while (!mTask.empty())
			//{
			//	auto & _begin = mTask.front();
			//	(this->*_begin.func)(_begin.param);
			//	delete _begin.param;	//delete void* 不会调用析构函数 如果指针中还含有指针指向堆区内存，可能无法释放（可能存在内存泄漏）by gwk
			//	mTask.pop_front();
			//}
			
			// @date 2019/11/26 begin 使用map进行消息过滤
			for (auto _iter = mTask.begin(); _iter != mTask.end(); ++_iter)
			{
				(this->*_iter->second.func)(_iter->second.param);
				delete _iter->second.param;
			}
			mTask.clear();
		}
		//msleep(40);
		//continue;
		//QCoreApplication::processEvents();
		long cpu_use_time = clock() - st;	//本次循环cpu用时 ms
		//std::cout << ">>>>>>> cpu_use_time:" << clock() - st << std::endl;
		if (cpu_use_time < 10)
		{
			msleep(10 - cpu_use_time);
		}
	}
}
