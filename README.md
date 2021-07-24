## 介绍
1. 完全使用Qt开发的二维地图，不依赖Qt外的其他东西

2. 最初是用在仿真模拟的GIS

3. 开发环境 VS2015+Qt5.12.8，VS至少在2013版本，Qt至少在5.6版本，VS工程文件自行修改

4. 模块：
	class GisViewer  外部窗口 QGraphicsView
			|
			|--- GisLayerItem	 地图管理图层 QGraphicsItem
			|		|
			|		|--- TitleLayer	 瓦片图层
			|		|
			|		|--- RouteLayer	 航线图层
			|		|
			|		|--- EntityLayer  实体图层（放飞机、坦克等实体）支持svg向量图标和军标切换
			|		|
			|		|--- DrawLayer	 绘图层 （目前只有测距功能）
			|
			|--- MouseLonLatItem     经纬度图层
			|
			|--- GeoMath	 地理计算（包括瓦片映射到Qt窗口）
			|
			|--- DataManager 地图上的数据管理
			|
			|--- Config 	配置文件读取（在config.ini中，字段都有说明）
		
	class GisSignals  地图上的操作对外发出的信号
	
	瓦片目录结构
	

5. 这个版本针对单个实体的业务较多，所以一个实体对应一个item，由于Qt性能跟不上，支持的实体数量有限，最好不超过100个

- - -

如果有大量实体的需求请联系作者邮箱975500206@qq.com，能够支持1w以上实体，同样完全是Qt开发
