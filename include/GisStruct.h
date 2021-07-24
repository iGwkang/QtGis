#pragma once

#if defined(QT_VERSION) && defined(WIN32) && defined(_MSC_VER)
# if QT_VERSION >= QT_VERSION_CHECK(5,0,0) 
# pragma execution_character_set("UTF-8")
# endif
#endif

#include <Serialization.h>

//地图操作状态
enum EnumMapMode
{
	/* 仿真运行 */	SimRunMode,
	/* 移动地图 */	MoveMapMode,
	/* 绘制航线 */	DrawRouteLineMode,
	/* 测距 */		MeasureMode,

};

//地理坐标点
struct GeoPoint
{
	explicit GeoPoint(double lon = 0.0, double lat = 0.0, double alt = 0.0)
		: longitude(lon), latitude(lat), altitude(alt) { }

	//经纬高
	double longitude;	// 经度 [-180, 180]
	double latitude;	// 纬度 [-90, 90]
	double altitude;	// 高度
};

//实体姿态
struct Attitude
{
	explicit Attitude(double _bearing = 0.0, double _pitch = 0.0, double _roll = 0.0)
		: bearing(_bearing), pitch(_pitch), roll(_roll) { }

	double bearing;	//方位角 [0, 360] 正北为0度
	double pitch;	//俯仰角 [-180, 180]
	double roll;	//旋转角(翻滚角) [-180, 180]
};

//航线点数据
//TODO 航线点除了经纬度，可能还有其他信息
struct RoutePointInfo
{
	RoutePointInfo() { }
	RoutePointInfo(const GeoPoint &p)
		: position(p) { }

	GeoPoint position;	//地理坐标

	operator const GeoPoint &() const { return position; }
};

//颜色
struct Color
{
	explicit Color(unsigned char _r = 0, unsigned char _g = 0, unsigned char _b = 0, unsigned char _a = 255)
		: r(_r), g(_g), b(_b), a(_a) {}
	// [0-255]
	unsigned char r, g, b, a;
};

//航线
//TODO 航线可能有名称, 距离信息
struct RouteInfo
{
	RouteInfo() : ID(0), isClose(false), color(255) { }

	std::string routeName;	//航线名称
	long long ID;	//航线id (唯一)
	Color color;	//航线颜色 默认红色
	bool isClose;	//首尾是否闭合
	std::vector<RoutePointInfo> points;	//航线上的点
};


//扫描样式
enum ScanStyle
{
	/* 圆周扫描 */ CircleScan,
	/* 扇形扫描 */ SectorScan,
};

//扫描参数
struct ScanParam
{
	ScanParam() : scanAngleSize(6.), scanSpeed(6.), pitchAngleSize(0), radius(0), curBearing(0), scanStyle(CircleScan) { }

	std::string abilityType;	//所在能力圈

	unsigned int radius;		//扫描角的半径 单位:m 
	float scanAngleSize;		//扫描水平张角大小 单位: deg
	float pitchAngleSize;		//俯仰张角大小
	float scanSpeed;			//扫描速度 单位:deg/s
	float curBearing;			//当前扫描光束相对实体的角度位置 (圆周扫描是方位角，上下扫描是俯仰角)
	ScanStyle scanStyle;		//扫描样式
	Color scanColor;			//颜色
};

// 实体单个能力圈参数
// modify by gwk 2019-11-25 08:46:13
// 修改前: 实体能力圈和扫描参数是分开的
// 修改后: 实体能力圈参数包含扫描参数
struct AbilityCircleParam
{
	explicit AbilityCircleParam()
		:  radius(0), startAngle(0), /*acID(0), */endAngle(0), minPitchAngle(0), maxPitchAngle(0)
	{ }

	std::string abilityType;	//能力类型 (一个实体 可能多种能力)
	//int acID;					// id
	unsigned int radius;		//半径 单位:m
	int startAngle;				//起始角度 (相对于实体的方向角) 单位: deg  (-180 ~ 180)
	int endAngle;				//结束角度 (相对于实体的方向角)

	int minPitchAngle;			//最小俯仰角 正前方为0，朝下为负 朝上为正
	int maxPitchAngle;			//最大俯仰角张角

	Color circleColor;			//圆的颜色

	ScanParam scanPatam;		//扫描参数
};


//实体
struct EntityInfo
{
	explicit EntityInfo() : ID(0), nType(0), campID(0), speed(0), nPlatform(0){ }

	long long ID;	//实体id	(唯一)
	int campID;		//阵营id

	float speed;		//实体速度 单位:m/s
	Color color;		//实体颜色

	std::string entityName;	//实体型号(名字) 不唯一

	// 参见 三维 Data/config 文件夹中 CustomModelConfig.xml 文件
	int nPlatform;	//平台 三维上用来标识海陆空等类型
	int nType;		//实体型号标识	三维用来寻找对应模型

	std::string iconPath;	// 二维实体svg图标路径

	GeoPoint position;	//实体位置
	Attitude attitude;	//实体姿态

	friend std::iostream & operator<< (std::iostream &in, const EntityInfo &a);
	friend std::iostream & operator>> (std::iostream &out, EntityInfo &a);
};

// 平台数据 包含实体和雷达信息
struct PlatformData
{
	PlatformData() {}
	explicit PlatformData(const EntityInfo & _entity) : entity(_entity) { }

	EntityInfo entity;
	std::unordered_map<std::string, AbilityCircleParam> radarData;
};

// 序列化实体
static std::iostream & operator<< (std::iostream &in, const EntityInfo &a)
{
	in << a.ID << a.campID << a.speed << a.color.r << a.color.g << a.color.b << a.color.a << a.entityName \
		<< a.nPlatform << a.nType << a.iconPath << a.position.longitude << a.position.latitude << a.position.altitude \
		<< a.attitude.bearing << a.attitude.pitch << a.attitude.roll;
	return in;
}

static std::iostream & operator>> (std::iostream &out, EntityInfo &a)
{
	out >> a.ID >> a.campID >> a.speed >> a.color.r >> a.color.g >> a.color.b >> a.color.a >> a.entityName \
		>> a.nPlatform >> a.nType >> a.iconPath >> a.position.longitude >> a.position.latitude >> a.position.altitude \
		>> a.attitude.bearing >> a.attitude.pitch >> a.attitude.roll;
	return out;
}
