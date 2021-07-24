#pragma once
#include <map>
#include <mutex>
#include <iostream>

#include <GisStruct.h>

/*
 * ��ͼ�ڲ����ݹ���
 * ʵ�����ݺͺ�������
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

	/***** ���� ��ɾ�Ĳ� *****/

	//�������
	void clearData();

	//�������
	bool addData(const T &);
	int addData(const std::vector<T> &);

	//ɾ������
	bool removeData(const long long &);
	int removeData(const std::vector<long long> &);

	//��������
	bool updateData(const T &);
	int updateData(const std::vector<T> &);
	void updateData(const std::unordered_map<long long, T> &);

	//��ȡ����
	bool getData(const long long &, T &);	//����id��ȡ����
	void getData(const std::vector<long long> &, std::unordered_map<long long, T> &);
	std::unordered_map<long long, T> getAllData();	//��ȡ��������

private:
	std::mutex dataMutex;
	std::unordered_map<long long, T> allData;	//����
};


template <typename T>
void DataManager<T>::clearData()
{
	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	allData.clear();
}

template <typename T>
DataManager<T> & DataManager<T>::instance()
{
	static DataManager<T> _s;
	return _s;
}

// �������
template <typename T>
bool DataManager<T>::addData(const T &data)
{
	//idΪ�� ֱ��return
	if (!data.ID)
		return false;

	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	//�ж�id�Ƿ��Ѿ�����
	if (allData.find(data.ID) != allData.end())
		return false;

	//�������
	allData[data.ID] = data;

	return true;
}

// �����������
template <typename T>
int DataManager<T>::addData(const std::vector<T> & data)
{
	//���� ������߳����ݾ���
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
			std::cout << __FUNCTION__ << " ---> ID�Ѵ���:" << entity.ID << std::endl;
	}

	//������ӵ�����
	return num;
}

// ɾ������
template <typename T>
bool DataManager<T>::removeData(const long long & id)
{
	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	auto _iter = allData.find(id);

	//��������� ����false
	if (_iter == allData.end())
		return false;

	//ɾ������
	allData.erase(_iter);

	return true;
}

// ����ɾ������
template <typename T>
int DataManager<T>::removeData(const std::vector<long long> & rmID)
{
	//���� ������߳����ݾ���
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
			std::cout << __FUNCTION__ << " ---> ID������:" << id << std::endl;
	}
	//����ɾ��������
	return num;
}

// ��������
template <typename T>
bool DataManager<T>::updateData(const T &data)
{
	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	auto _iter = allData.find(data.ID);

	//��������� ����false
	if (_iter == allData.end())
		return false;

	_iter->second = data;
	return true;
}

// ������������
template <typename T>
int DataManager<T>::updateData(const std::vector<T> &data)
{
	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	int num = 0;

	for (auto & entity : data)
	{
		auto _iter = allData.find(entity.ID);

		//������� ����ʵ��
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
	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	allData.insert(data.begin(), data.end());
}

//��ȡ����
template <typename T>
bool DataManager<T>::getData(const long long &id, T & outData)
{
	//���� ������߳����ݾ���
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
	//���� ������߳����ݾ���
	std::lock_guard<std::mutex> lock(dataMutex);

	for (auto & i : id)
	{
		auto _iter = allData.find(i);
		if (_iter != allData.end())
		{
			entity[_iter->first] = _iter->second;
		}
		else
			std::cout << __FUNCTION__ << " ---> ID������:" << i << std::endl;
	}

}

template <typename T>
std::unordered_map<long long, T> DataManager<T>::getAllData()
{
	std::lock_guard<std::mutex> lock(dataMutex);

	return allData;
}

