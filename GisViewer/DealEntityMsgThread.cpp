#include "DealEntityMsgThread.h"
#include "ReadGisConfig.h"
#include "TileSystem.h"
#include "GisStatus.h"
#include <QCoreApplication>
#include <iostream>
#include <QDomDocument>
#include <QFile>
#include <QColor>

//��������Ȧ add by gwk 2019/11/07
struct SetCircleParamTask
{
	SetCircleParamTask(long long id, const AbilityCircleParam &param)
		: entity_id(id), circleParam(param) { }
	long long entity_id;
	AbilityCircleParam circleParam;
};

//ɾ������Ȧ add by gwk 2019/11/07
struct RemoveCircleTask
{
	RemoveCircleTask(long long id, const std::string &type)
		: entity_id(id), circleType(type) { }
	long long entity_id;
	std::string circleType;
};

//����ɨ����� add by gwk 2019/11/07
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

	//����ڴ�
	for (auto & task : mTask)
	{
		delete task.second.param;
	}
	//ɾ�����е�����
	mTask.clear();

	//�������ʵ��
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
	// ����һ��Ψһ��ʶ
	std::string key = std::to_string(entity_id) + func;

	auto _find = mTask.find(key);

	// ���key�����ڣ���ӽ�ȥ 
	if (_find == mTask.end())
	{
		mTask[key] = taskData;
	}
	// ���key���ڣ���Ҫɾ��ԭ����task�����µ�task ���뵽map��
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

	//����ͼƬ
	if (!setEntityImage(newEntity))
		return;

	//����λ��
	setEntityPos(newEntity);
	
	allEntity[entity->ID] = newEntity;
	//�����ź�
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

	//����ͼƬ
	if (!setEntityImage(newEntity))
		return;

	//����λ��
	setEntityPos(newEntity);

	_iter->second = newEntity;

	//�����ź�
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

	//�ж�ʵ���Ƿ����
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

	//�ж�ʵ���Ƿ����
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

	//�ж�ʵ���Ƿ����
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

	//�鿴�Ƿ��Ѿ�����
	if (mAllLoadImage.find(filePath) != mAllLoadImage.end())
	{
		//�ж��Ƿ��Ѿ����ú���ɫ
		if (colorIcon.find(key) != colorIcon.end())
		{
			return colorIcon[key];
		}

		//���û�����ú���ɫ 
		QDomDocument doc;

		if (!doc.setContent(mAllLoadImage[filePath]))
			return nullptr;

		auto docEle = doc.documentElement();
		//������ɫ
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
			std::cout << "ʵ��ͼ�����ʧ�ܣ� File:" << filePath << __FUNCTION__ << " line:" << __LINE__  << std::endl;
			delete svg;
			return nullptr;
		}
	}
	else
	{
		QFile file(filePath.c_str());
		if (!file.open(QIODevice::ReadOnly))
		{
			std::cout << "ʵ��ͼ�����ʧ�ܣ� File:" << filePath << __FUNCTION__ << " line:" << __LINE__ << std::endl;
			return nullptr;
		}
		QDomDocument doc;
		if (!doc.setContent(&file)) {
			file.close();
			std::cout << "ʵ��ͼ�����ʧ�ܣ� File:" << filePath << __FUNCTION__ << " line:" << __LINE__ << std::endl;
			return nullptr;
		}
		file.close();

		QString alpha = QString().setNum(color.a / 255.0);

		auto docEle = doc.documentElement();

		//����͸����
		setAttrOpacityValue(docEle, "path", alpha);
		setAttrOpacityValue(docEle, "ellipse", alpha);
		setAttrOpacityValue(docEle, "circle", alpha);
		setAttrOpacityValue(docEle, "rect", alpha);
		setAttrOpacityValue(docEle, "line", alpha);
		setAttrOpacityValue(docEle, "polygon", alpha);

		mAllLoadImage[filePath] = doc.toByteArray();

		//������ɫ
		setAttrValue(docEle, "path", "fill", colorName);
		setAttrValue(docEle, "ellipse", "fill", colorName);
		setAttrValue(docEle, "circle", "fill", colorName);
		setAttrValue(docEle, "rect", "fill", colorName);
		setAttrValue(docEle, "line", "fill", colorName);
		setAttrValue(docEle, "polygon", "fill", colorName);

		// ���
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
			std::cout << "ʵ��ͼ�����ʧ�ܣ� File:" << filePath << __FUNCTION__ << " line:" << __LINE__ << std::endl;
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
			//ѭ������ �߳�����
			while (!mTask.empty())
			{
				auto & _begin = mTask.front();
				(inst->*_begin.func)(_begin.param);
				delete _begin.param;	//delete void* ��������������� ���ָ���л�����ָ��ָ������ڴ棬�����޷��ͷţ����ܴ����ڴ�й©��by gwk
				mTask.pop_front();
			}
		}

		//QCoreApplication::processEvents();
		long cpu_use_time = clock() - st;	//����ѭ��cpu��ʱ ms
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
	// ��������߳��Էǳ�����ٶȵ��ýӿڣ��ᵼ��Qt��Ϣ���ͣ����̴߳�������������Ῠ��
	// ��Ҫ����Ϣ���˵Ĵ���
	// Ψһ�Է�����ÿһ��task����param���涼����һ��id��Ȼ��ÿһ��task�ֻ��Ӧһ���ص�������ͨ����������������������һ��Ψһ�Ե�id�������ظ�������
	while (isRun)
	{
		long st = clock();

		{
			std::unique_lock<std::mutex> lock(taskMutex);
			//ѭ������ �߳�����

			// @date 2019/11/26 û����Ϣ���˻���
			//while (!mTask.empty())
			//{
			//	auto & _begin = mTask.front();
			//	(this->*_begin.func)(_begin.param);
			//	delete _begin.param;	//delete void* ��������������� ���ָ���л�����ָ��ָ������ڴ棬�����޷��ͷţ����ܴ����ڴ�й©��by gwk
			//	mTask.pop_front();
			//}
			
			// @date 2019/11/26 begin ʹ��map������Ϣ����
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
		long cpu_use_time = clock() - st;	//����ѭ��cpu��ʱ ms
		//std::cout << ">>>>>>> cpu_use_time:" << clock() - st << std::endl;
		if (cpu_use_time < 10)
		{
			msleep(10 - cpu_use_time);
		}
	}
}
