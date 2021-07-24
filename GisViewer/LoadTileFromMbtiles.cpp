#include "LoadTileFromMbtiles.h"
#include <iostream>

#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlError>
#include <QVariant>

//#include <SQLiteCpp/SQLiteCpp.h>
//#if NDEBUG
//#pragma comment(lib, "sqlite3")
//#pragma comment(lib, "SQLiteCpp")
//#else
//#pragma comment(lib, "sqlite3d")
//#pragma comment(lib, "SQLiteCppd")
//#endif

LoadTileFromMbtiles::LoadTileFromMbtiles()
{
	//db = nullptr;
}


LoadTileFromMbtiles::~LoadTileFromMbtiles()
{
	//delete db;
	//db = nullptr;
}

LoadTileFromMbtiles & LoadTileFromMbtiles::instance()
{
	static LoadTileFromMbtiles _instance;
	return _instance;
}

void LoadTileFromMbtiles::setTileFilePath(const std::string &path)
{
	dbPath = path;
	
	//if (db)
	//{
	//	delete db;
	//}
	//db = new SQLite::Database(dbPath);
	database.close();

	database = QSqlDatabase::addDatabase("QSQLITE", dbPath.c_str());
	database.setDatabaseName(dbPath.c_str());
	if (!database.open())
	{
		std::cout << database.lastError().text().toStdString() << std::endl;
	}
}

bool LoadTileFromMbtiles::getTile(int level, int tileX, int tileY, QImage &image)
{
	//if (!db)
	//	return false;
	
	//SQLite::Database db(dbPath);
	char buf[128] = {0};
	std::sprintf(buf, "SELECT tile_data FROM tiles WHERE zoom_level=%d AND tile_column=%d AND tile_row=%d", level, tileX, tileY);
	//SQLite::Statement query(*db, buf);
	//	
	//if (query.executeStep())
	//{
	//	//const std::string col1 = query.getColumn(0).getName(); // col1 = "zoom_level" z
	//	//const std::string col2 = query.getColumn(1).getName(); // col2 = "tile_column" x
	//	//const std::string col3 = query.getColumn(2).getName(); // col3 = "tile_row" y
	//	//const std::string col4 = query.getColumn(3).getName(); // col4 = "tile_data" data

	//	int len = query.getColumn("tile_data").getBytes();
	//	const char *data = query.getColumn("tile_data");
	//	image = QImage::fromData((uchar *)data, len);
	//}

	QSqlQuery query = database.exec(buf);
	//QSqlQuery query(database);
	//query.prepare("SELECT tile_data FROM tiles WHERE zoom_level=:z AND tile_column=:tx AND tile_row=:ty");
	//query.bindValue(":z", level);
	//query.bindValue(":tx", tileX);
	//query.bindValue(":ty", tileY);
	//query.exec();

	if (query.next())
	{
		QByteArray data = query.value("tile_data").toByteArray();
		image = QImage::fromData(data);
	}
	return !image.isNull();
}
