#pragma once
#include <QImage>
#include <QSqlDatabase>

//namespace SQLite {
//	class Database;
//}

class LoadTileFromMbtiles
{
	LoadTileFromMbtiles();
	~LoadTileFromMbtiles();
public:
	static LoadTileFromMbtiles &instance();

	//设置瓦片路径
	void setTileFilePath(const std::string &);

	//根据z/x/y获取瓦片
	bool getTile(int level, int tileX, int tileY, QImage &);

private:
	//SQLite::Database *db;
	QSqlDatabase database;
	std::string dbPath;
};

