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

	//������Ƭ·��
	void setTileFilePath(const std::string &);

	//����z/x/y��ȡ��Ƭ
	bool getTile(int level, int tileX, int tileY, QImage &);

private:
	//SQLite::Database *db;
	QSqlDatabase database;
	std::string dbPath;
};

