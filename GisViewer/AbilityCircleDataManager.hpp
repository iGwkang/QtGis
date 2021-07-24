#pragma once
#include <GisStruct.h>
#include <mutex>
#include <map>

/**
@brief 实体能力圈数据维护
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

	// 清空数据
	void clearData();

	// 删除实体
	void removeEntity(long long entity_id);
	void removeEntity(const std::vector<long long> & entity_id);

	// 添加能力圈的参数数据
	void addCircleParamData(long long entity_id, const AbilityCircleParam &param);

	// 删除能力圈数据
	void removeCircle(long long entity_id, const std::string &circleType);

	// 设置扫描参数
	void updateScanParam(long long entity_id, const ScanParam &param);

	// 获取能力圈参数
	bool getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData);
	std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> getAbilityCircleData();

private:
	std::mutex dataMutex;
	//所有实体里的能力圈
	std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> mAllEntityACP;
};

// 清空数据
void AbilityCircleDataManager::clearData()
{
	std::unique_lock<std::mutex> lock(dataMutex);
	mAllEntityACP.clear();
}

//删除实体
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

//设置能力圈的参数
void AbilityCircleDataManager::addCircleParamData(long long entity_id, const AbilityCircleParam &param)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	//if (mAllEntityACP.find(entity_id) == mAllEntityACP.end())
	//	return false;
	mAllEntityACP[entity_id][param.abilityType] = param;
	//return true;
}

//删除能力圈
void AbilityCircleDataManager::removeCircle(long long entity_id, const std::string &circleType)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	mAllEntityACP[entity_id].erase(circleType);

	// 如果实体里面没有了能力圈，维护这个实体能力圈的数据也就不需要了
	if (mAllEntityACP[entity_id].empty())
	{
		mAllEntityACP.erase(entity_id);
	}
}

//设置扫描参数
void AbilityCircleDataManager::updateScanParam(long long entity_id, const ScanParam &param)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	
	mAllEntityACP[entity_id][param.abilityType].scanPatam = param;
}

// 根据实体id 获取实体能力圈数据
bool AbilityCircleDataManager::getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData)
{
	std::unique_lock<std::mutex> lock(dataMutex);
	// 如果实体存在 返回true
	if (mAllEntityACP.find(entity_id) != mAllEntityACP.end())
	{
		acData = mAllEntityACP[entity_id];
		return true;
	}
	return false;
}

// 获取能力圈参数
std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> AbilityCircleDataManager::getAbilityCircleData()
{
	std::unique_lock<std::mutex> lock(dataMutex);
	return mAllEntityACP;
}