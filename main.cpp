#include <QtGui/QApplication>
#include "canvas.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Canvas canvas;
    canvas.resize(1024, 768);
    canvas.show();

    return a.exec();
}
