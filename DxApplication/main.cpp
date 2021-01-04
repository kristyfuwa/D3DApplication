#include "DxApplication.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DxApplication w;
	w.resize(800, 600);
	w.Init();
    w.show();
    return a.exec();
}
