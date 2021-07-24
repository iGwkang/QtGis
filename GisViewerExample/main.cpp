#include <QtWidgets/QApplication>
#include "../GisViewer/GisViewer.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

	GisViewer view;
	view.show();

    return a.exec();
}
