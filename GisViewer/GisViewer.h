#pragma once

# ifdef GISVIEWER_LIB
#  define GISVIEWER_EXPORT __declspec(dllexport)
# else
#  define GISVIEWER_EXPORT __declspec(dllimport)
# endif

#include <QGraphicsView>
#include <GisStruct.h>

class GisViewerPrivate;

/**
	@file GisViewer.h
	@class GisViewer
	@brief ��ͼ�ӿڣ���ͼ�϶����ṩ�Ĳ����ӿ�
	@author gwk
*/
class GISVIEWER_EXPORT GisViewer : public QGraphicsView
{
	Q_OBJECT

	GisViewer(const GisViewer &);
	GisViewer& operator=(const GisViewer &);
public:
	GisViewer(QWidget * parent = nullptr);
	~GisViewer();

public slots:
	// ��ճ���(�����ͼ��ʵ��ͺ�������)
	void clearGisScene();

	// ���õ�ǰ����ʱ��
	void setCurrentSimulationTime(double simSec);
	
	// �����Ƿ�������״̬ ���������״̬ʱ��ʵ�塢���߶������ƶ��༭��
	void setSimStatus(bool mode);

	/***** ��ͼ�������� *****/
	
	//����/�˳����
	void setMeasureMode(bool mode);

	//����/�˳����ƺ��� true:���룬false:�˳�
	void setDrawRouteMode(bool mode);

	/***** ͼ����� *****/

	//���ò�������
	void setMeasureVisible(bool show);

	/**
		@brief ���ú��ߵ�����
		@param[in] show ����
		@param[in] route_id ���ߵ�id, Ĭ��Ϊ0, ��������
		@return void
		@date 2019-11-18 16:38:04
	*/
	void setRouteVisible(bool show, const long long &route_id = 0);

	/**
		@brief ���ú��ߵ������
		@param[in] show ����
		@param[in] route_id ���ߵ�id, Ĭ��Ϊ0, �������к��ߵĵ�
		@return void
		@date 2019-11-22 13:44:58
	*/
	void setRoutePointVisible(bool show, const long long &route_id = 0);

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

	/***** ���ݽ��� *****/

	/**** ʵ�����(�̰߳�ȫ) ****/


	// ����ʵ��ͼ��Ŀ¼ add by gwk 2019/11/06
	void setSvgDir(const std::string &);

	// ���ʵ��
	bool addEntity(const EntityInfo &);
	bool addEntity(const std::vector<EntityInfo> &);

	// ɾ��ʵ��
	bool removeEntity(const long long &);
	int removeEntity(const std::vector<long long> &);

	// ����ʵ��
	bool updateEntity(const EntityInfo &);
	int updateEntity(const std::vector<EntityInfo> &);
	void updateEntity(const std::unordered_map<long long, EntityInfo> &);

	// ��ȡʵ��
	bool getEntity(const long long &, EntityInfo &);	//����id��ȡʵ��
	void getEntity(const std::vector<long long> &, std::unordered_map<long long, EntityInfo> &);
	std::unordered_map<long long, EntityInfo> getAllEntity();	//��ȡ����ʵ��

	// ��������Ȧ�Ĳ��� add by gwk 2019/11/07
	void setCircleParam(long long entity_id, const AbilityCircleParam &param);

	// ɾ������Ȧ add by gwk 2019/11/07
	void removeCircle(long long entity_id, const std::string &circleType);

	// ����ɨ����� add by gwk 2019/11/07
	void setScanParam(long long entity_id, const ScanParam &param);

	/**
		@brief ��ȡ��ʵ�����ù�������Ȧ����
		@param[in] entity_id: ʵ��id
		@param[out] acData : ����ʵ���������е�����Ȧ { key - ����Ȧ���� value - ����Ȧ���� }
		@return bool ʵ����ڲ���ʵ����������Ȧ ���� true�����򷵻�false
		@date 2019-11-25 09:05:57
	*/
	bool getEntityAbilityCircle(long long entity_id, std::unordered_map<std::string, AbilityCircleParam> &acData);

	/**
		@brief ��ȡ�������ù�������Ȧ����
		@param void
		@return std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> { key - ʵ��id value - { key - ����Ȧ���� value - ����Ȧ�������� } }
		@date 2019-11-25 09:05:57
	*/
	std::unordered_map<long long, std::unordered_map<std::string, AbilityCircleParam>> getAbilityCircleData();

	/**** ����(ֻ�������̵߳���) ****/
	// ��Ӻ���
	bool addRoute(const RouteInfo &);
	bool addRoute(const std::vector<RouteInfo> &);

	// ɾ������
	bool removeRoute(const long long &);
	int removeRoute(const std::vector<long long> &);

	// ���º���
	bool updateRoute(const RouteInfo &);
	int updateRoute(const std::vector<RouteInfo> &);

	// ��ȡ����
	bool getRoute(const long long &, RouteInfo &);	//����id��ȡ����
	void getRoute(const std::vector<long long> &, std::unordered_map<long long, RouteInfo> &);
	std::unordered_map<long long, RouteInfo> getAllRoute();	//��ȡ���к���

protected:
	// ���ڴ�С�ı䴥���¼�
	void resizeEvent(QResizeEvent *event);

	// ����ƶ��¼�
	void mouseMoveEvent(QMouseEvent *event);

	// �������¼� ��������
	void wheelEvent(QWheelEvent *event);

private:
	void zoom(int delta);	//����
	void initUi();
	void initSigAndSlt();	//��ʼ���źŲ�

private:
	GisViewerPrivate * mPtr;
};
