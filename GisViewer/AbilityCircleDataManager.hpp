#pragma once
#include <GisStruct.h>
#include <mutex>
#include <map>

/**
@brief ʵ������Ȧ����ά��
*/

class AbilityCircleDataManager
{
	AbilityCircleDataManager() {}
public:
	static inline AbilityCircleDataManager & instance()
	{
		static AbilityCircleDataManager _instance;
		return _instance;
	}

	// �������
	void clearData();

	// ɾ��ʵ��
	void removeEntity(long long entity_id);
	void removeEntity(const std::vector<long long> & entity_id);

	// �������Ȧ�Ĳ�������
	void addCircleParamData(long long entity_id, const AbilityCircleParam &param);

	// ɾ������Ȧ����
	void removeCircle(long long entity_id, const std::string &circleType);

	// ����ɨ�����
	void updateScanParam(long long entity_id, const ScanParam &param);

	// ��ȡ����Ȧ����
	bool getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData);
	std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> getAbilityCircleData();

private:
	std::mutex dataMutex;
	//����ʵ���������Ȧ
	std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> mAllEntityACP;
};

// �������
void AbilityCircleDataManager::clearData()
{
	std::unique_lock<std::mutex> lock(dataMutex);
	mAllEntityACP.clear();
}

//ɾ��ʵ��
void AbilityCircleDataManager::removeEntity(long long entity_id)
{
	std::unique_lock<std::mutex> lock(dataMutex);

	mAllEntityACP.erase(entity_id);
}

void AbilityCircleDataManager::removeEntity(const std::vector<long long> & entity_id)
{
	std::unique_lock<std::mutex> lock(dataMutex);

	for (auto & id : entity_id)
	{
		mAllEntityACP.erase(id);
	}
}

//��������Ȧ�Ĳ���
void AbilityCircleDataManager::addCircleParamData(long long entity_id, const AbilityCircleParam &param)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	//if (mAllEntityACP.find(entity_id) == mAllEntityACP.end())
	//	return false;
	mAllEntityACP[entity_id][param.abilityType] = param;
	//return true;
}

//ɾ������Ȧ
void AbilityCircleDataManager::removeCircle(long long entity_id, const std::string &circleType)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	mAllEntityACP[entity_id].erase(circleType);

	// ���ʵ������û��������Ȧ��ά�����ʵ������Ȧ������Ҳ�Ͳ���Ҫ��
	if (mAllEntityACP[entity_id].empty())
	{
		mAllEntityACP.erase(entity_id);
	}
}

//����ɨ�����
void AbilityCircleDataManager::updateScanParam(long long entity_id, const ScanParam &param)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	
	mAllEntityACP[entity_id][param.abilityType].scanPatam = param;
}

// ����ʵ��id ��ȡʵ������Ȧ����
bool AbilityCircleDataManager::getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	// ���ʵ����� ����true
	if (mAllEntityACP.find(entity_id) != mAllEntityACP.end())
	{
		acData = mAllEntityACP[entity_id];
		return true;
	}
	return false;
}

// ��ȡ����Ȧ����
std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> AbilityCircleDataManager::getAbilityCircleData()
{
	std::unique_lock<std::mutex> lock(dataMutex);
	return mAllEntityACP;
}