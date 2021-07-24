#pragma once

#include <QGraphicsObject>
#include <unordered_map>
#include "GisEntity.h"

class EntityItem;
class DealEntityMsgThread;

struct EntityInfo;

/**
 * @brief ʵ��㡣���в���ʵ��Ľӿڶ�������ʵ��
 */
class EntityLayer : public QGraphicsObject
{
	Q_OBJECT

public:
	EntityLayer(QGraphicsItem *parent);
	~EntityLayer();

	QRectF boundingRect() const;

	//��ͼ���� ���µȼ�
	void updateLevel();

	//��ճ���(���ʵ������)
	void clearData();

	// �����Ƿ�ɱ༭
	void setEditMode(bool mode);

	/******** �������� ********/

	/**
		@brief ����ʵ��ͼ������� (ͬʱ������ʵ���ϵ�����Ȧ��ɨ��Ч��)
		@param[in] show ����
		@param[in] entity_id ʵ���id, Ĭ��Ϊ0, ��������
		@return void
		@date 2019-11-18 16:50:59
	*/
	void setEntityVisible(bool show, const long long &entity_id = 0);

	/**
		@brief ����ʵ������Ȧ������
		@param[in] show ����
		@param[in] entity_id ʵ���id
		@param[in] circleType ʵ���������Ȧ������
		@return void
		@date 2019-11-18 16:38:13
	*/
	void setEntityAbilityCircleVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief ����ʵ������Ȧɨ�迪��
		@param[in] show ����
		@param[in] entity_id ʵ���id
		@param[in] circleType ʵ���������Ȧ������
		@return void
		@date 2019-11-18 16:38:29
	*/
	void setEntityAbilityCircleScanVisible(bool show, const long long & entity_id, const std::string &circleType);

	/**
		@brief ����ʵ����Ƶ�����
		@param[in] show ���Ƶ�����
		@param[in] entity_id ʵ���id, Ĭ��Ϊ0, ��������ʵ��
		@return void
		@date 2019-11-21 08:45:01
	*/
	void setEntityScutcheonVisible(bool show, const long long & entity_id = 0);

	/***************  ����ʵ��Ķ���ҵ���̴߳������հ����ݷ��ص�ʵ��㴦��  ****************/
	//����ʵ��ͼ��·��
	void setSvgDir(const std::string &);

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

	//��������Ȧ�Ĳ��� add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	//ɾ������Ȧ add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	//����ɨ����� add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

protected:
	void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = Q_NULLPTR);

	void dragEnterEvent(QGraphicsSceneDragDropEvent *event);

	void dropEvent(QGraphicsSceneDragDropEvent *event);


private slots:
	// ���Ի���ָ�� ����������֮���ͷ�
	void sltNewEntity(GisEntity);		//�½�ʵ��
	void sltRemoveEntity(long long);	//ɾ��ʵ��
	void sltUpdateEntity(GisEntity);	//����ʵ��
	void sltNewCircle(long long, AbilityCircleParam);	//����ʵ������Ȧ
	void sltRmCircle(long long, std::string);			//ɾ��ʵ������Ȧ
	void sltNewScan(long long, ScanParam);				//����ɨ�����

private:
	std::unordered_map<long long, EntityItem *> mAllEntity;	//���е�ʵ��
	DealEntityMsgThread * mLoadThread;		//����ʵ��ͼ��Ϣ���߳�
};
