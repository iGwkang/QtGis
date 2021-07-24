#pragma once
#include <GisStruct.h>

//����뾶
const double EarthRadius = 6378137.0;	//���
const double EarthPolar = 6356752.3142;	//�ϱ���

//���γ��
const double MaxLatitude = 85.05112878;
//��Сγ��
const double MinLatitude = -MaxLatitude;

//��󾭶�
const double MaxLongitude = 180.0;
//��С����
const double MinLongitude = -MaxLongitude;

//Բ����
const double PI = 3.14159265358979323846;	// Pi
const double PI_2 = 1.57079632679489661923;	// Pi / 2

/**
	@file GeoMath.h
	@class GeoMath
	@brief �����ϵ�һЩ�ռ����
	@author gwk
	@date 2019-11-18 10:42:16
*/
class GeoMath
{
	GeoMath();
public:
	// �Ƕ�ת����
	static inline double DegreesToRadians(double angle) { return angle*PI / 180.0; }
	// ����ת�Ƕ�
	static inline double RadiansToDegrees(double angle) { return angle*180.0 / PI; }
	
	/**
		@brief �����������֮��ľ��� (�Ƕȵ�λ)
		@note ���ݵ�������ʼ����յ�ľ�γ�ȣ���������֮��ľ���
		@param[in] lon1 : ��ʼ��γ��
		@param[in] lat1 : ��ʼ�㾭��
		@param[in] lon2 : ������γ��
		@param[in] lat2 : �����㾭��
		@return double : ���������ľ���
	*/
	static double distance(double lon1, double lat1, double lon2, double lat2);


	/**
		@brief �����ֵ�� (���ȵ�λ)
		@note ������ʼ����յ�ľ�γ�ȣ�������Բ���м�Ĳ�ֵ��
		@param[in] lat1Rad : ��ʼ��γ��
		@param[in] lon1Rad : ��ʼ�㾭��
		@param[in] lat2Rad : ������γ��
		@param[in] lon2Rad : �����㾭��
		@param[in] t : ��ֵ��ٷֱ�λ�ã���Χ [0, 1]
		@param[out] out_latRad : ��ֵ���γ��
		@param[out] out_lonRad : ��ָ��ľ���
		@return void
	*/
	static void interpolate(double lat1Rad, double lon1Rad,
		double lat2Rad, double lon2Rad,
		double t,
		double& out_latRad, double& out_lonRad);

	/**
		@brief ����һ��·���еĲ�ֵ�� (�Ƕȵ�λ)
		@note ����һ������, ���մ��Բ���������Ӧ�Ĳ�ֵ��
		@param[in] linearPath : ���������еĵ�
		@param[out] out_linearPath : �������ֵ֮�󷵻صĺ��ߵ�
		@param[in] spacing : �����ֵ�ļ�� ��λ: m��ÿ�� spacing �׼���һ����ֵ�㣩
		@param[in] isClose : �����Ƿ�պ�
		@return bool ��linearPathΪ��ʱ ����false
	*/
	static bool tessellateGeo(const std::vector<GeoPoint> & linearPath, std::vector<GeoPoint> & out_linearPath, double spacing, bool isClose = false);


	//����ת��  û�п���ƫ�� (���ȵ�λ)
	static void convertLatLongHeightToXYZ(double latitudeRad, double longitudeRad, double height,
		double& X, double& Y, double& Z);

	//����ת��  û�п���ƫ�� (���ȵ�λ)
	static void convertXYZToLatLongHeight(double X, double Y, double Z,
		double& latitudeRad, double& longitudeRad, double& height);
};

