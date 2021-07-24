#pragma once
#include <map>
#include <mutex>
#include <iostream>

#include <GisStruct.h>

/*
 * 地图内部数据管理
 * 实体数据和航线数据
 */
template <typename T>
class DataManager
{
	DataManager<T>() {}
	DataManager<T>(const DataManager<T> &) = delete;
	DataManager<T> & operator= (const DataManager<T> &) = delete;
	~DataManager<T>() {}
public:
	static inline DataManager &instance();

	/***** 数据 增删改查 *****/

	//清空数据
	void clearData();

	//添加数据
	bool addData(const T &);
	int addData(const std::vector<T> &);

	//删除数据
	bool removeData(const long long &);
	int removeData(const std::vector<long long> &);

	//更新数据
	bool updateData(const T &);
	int updateData(const std::vector<T> &);
	void updateData(const std::unordered_map<long long, T> &);

	//获取数据
	bool getData(const long long &, T &);	//根据id获取数据
	void getData(const std::vector<long long> &, std::unordered_map<long long, T> &);
	std::unordered_map<long long, T> getAllData();	//获取所有数据

private:
	std::mutex dataMutex;
	std::unordered_map<long long, T> allData;	//数据
};


template <typename T>
void DataManager<T>::clearData()
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	allData.clear();
}

template <typename T>
DataManager<T> & DataManager<T>::instance()
{
	static DataManager<T> _s;
	return _s;
}

// 添加数据
template <typename T>
bool DataManager<T>::addData(const T &data)
{
	//id为空 直接return
	if (!data.ID)
		return false;

	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	//判断id是否已经存在
	if (allData.find(data.ID) != allData.end())
		return false;

	//添加数据
	allData[data.ID] = data;

	return true;
}

// 批量添加数据
template <typename T>
int DataManager<T>::addData(const std::vector<T> & data)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);
	int num = 0;
	for (auto & entity : data)
	{
		if (allData.find(entity.ID) == allData.end())
		{
			allData[entity.ID] = entity;
			++num;
		}
		else
			std::cout << __FUNCTION__ << " ---> ID已存在:" << entity.ID << std::endl;
	}

	//返回添加的数量
	return num;
}

// 删除数据
template <typename T>
bool DataManager<T>::removeData(const long long & id)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	auto _iter = allData.find(id);

	//如果不存在 返回false
	if (_iter == allData.end())
		return false;

	//删除数据
	allData.erase(_iter);

	return true;
}

// 批量删除数据
template <typename T>
int DataManager<T>::removeData(const std::vector<long long> & rmID)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);
	int num = 0;
	for (auto &id : rmID)
	{
		auto _iter = allData.find(id);
		if (_iter != allData.end())
		{
			++num;
			allData.erase(_iter);
		}
		else
			std::cout << __FUNCTION__ << " ---> ID不存在:" << id << std::endl;
	}
	//返回删除的数量
	return num;
}

// 更新数据
template <typename T>
bool DataManager<T>::updateData(const T &data)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	auto _iter = allData.find(data.ID);

	//如果不存在 返回false
	if (_iter == allData.end())
		return false;

	_iter->second = data;
	return true;
}

// 批量更新数据
template <typename T>
int DataManager<T>::updateData(const std::vector<T> &data)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	int num = 0;

	for (auto & entity : data)
	{
		auto _iter = allData.find(entity.ID);

		//如果存在 更新实体
		if (_iter != allData.end())
		{
			++num;
			_iter->second = entity;
		}
	}

	return num;
}


template <typename T>
void DataManager<T>::updateData(const std::unordered_map<long long, T> &data)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	allData.insert(data.begin(), data.end());
}

//获取数据
template <typename T>
bool DataManager<T>::getData(const long long &id, T & outData)
{
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	auto _iter = allData.find(id);
	if (_iter == allData.end())
		return false;

	outData = _iter->second;

	return true;
}

template <typename T>
void DataManager<T>::getData(const std::vector<long long> &id, std::unordered_map<long long, T> &entity)
{
	entity.clear();
	//加锁 避免多线程数据竞争
	std::lock_guard<std::mutex> lock(dataMutex);

	for (auto & i : id)
	{
		auto _iter = allData.find(i);
		if (_iter != allData.end())
		{
			entity[_iter->first] = _iter->second;
		}
		else
			std::cout << __FUNCTION__ << " ---> ID不存在:" << i << std::endl;
	}

}

template <typename T>
std::unordered_map<long long, T> DataManager<T>::getAllData()
{
	std::lock_guard<std::mutex> lock(dataMutex);

	return allData;
}

