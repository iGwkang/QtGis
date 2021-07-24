#pragma once

#include <QGraphicsObject>
#include <unordered_map>
#include "GisEntity.h"

class EntityItem;
class DealEntityMsgThread;

struct EntityInfo;

/**
 * @brief 实体层。所有操作实体的接口都在这里实现
 */
class EntityLayer : public QGraphicsObject
{
	Q_OBJECT

public:
	EntityLayer(QGraphicsItem *parent);
	~EntityLayer();

	QRectF boundingRect() const;

	//地图缩放 更新等级
	void updateLevel();

	//清空场景(清除实体数据)
	void clearData();

	// 设置是否可编辑
	void setEditMode(bool mode);

	/******** 显隐控制 ********/

	/**
		@brief 设置实体图标的显隐 (同时会隐藏实体上的能力圈和扫描效果)
		@param[in] show 显隐
		@param[in] entity_id 实体的id, 默认为0, 控制所有
		@return void
		@date 2019-11-18 16:50:59
	*/
	void setEntityVisible(bool show, const long long &entity_id = 0);

	/**
		@brief 设置实体能力圈的显隐
		@param[in] show 显隐
		@param[in] entity_id 实体的id
		@param[in] circleType 实体的上能力圈的类型
		@return void
		@date 2019-11-18 16:38:13
	*/
	void setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief 设置实体能力圈扫描开关
		@param[in] show 显隐
		@param[in] entity_id 实体的id
		@param[in] circleType 实体的上能力圈的类型
		@return void
		@date 2019-11-18 16:38:29
	*/
	void setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief 设置实体标牌的显隐
		@param[in] show 标牌的显隐
		@param[in] entity_id 实体的id, 默认为0, 控制所有实体
		@return void
		@date 2019-11-21 08:45:01
	*/
	void setEntityScutcheonVisible(bool show, const long long & entity_id = 0);

	/***************  操作实体的都在业务线程处理，最终把数据返回到实体层处理  ****************/
	//设置实体图标路径
	void setSvgDir(const std::string &);

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

	//设置能力圈的参数 add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	//删除能力圈 add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	//设置扫描参数 add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

	void dragEnterEvent(QGraphicsSceneDragDropEvent *event);

	void dropEvent(QGraphicsSceneDragDropEvent *event);


private slots:
	// 可以换成指针 接受完数据之后释放
	void sltNewEntity(GisEntity);		//新建实体
	void sltRemoveEntity(long long);	//删除实体
	void sltUpdateEntity(GisEntity);	//更新实体
	void sltNewCircle(long long, AbilityCircleParam);	//设置实体能力圈
	void sltRmCircle(long long, std::string);			//删除实体能力圈
	void sltNewScan(long long, ScanParam);				//设置扫描参数

private:
	std::unordered_map<long long, EntityItem *> mAllEntity;	//所有的实体
	DealEntityMsgThread * mLoadThread;		//处理实体图消息的线程
};
