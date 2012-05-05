#include <QtGui/QApplication>
#include <QDesktopWidget>

#include "canvas.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    Canvas canvas;
    canvas.resize(1024, 768);
    // center window
    QDesktopWidget *desktop = QApplication::desktop();
    QRect desktopRect = desktop->availableGeometry();
    canvas.move(desktopRect.width() / 2 - canvas.width() / 2,
                desktopRect.height() / 2 - canvas.height() / 2);
    canvas.show();

    return a.exec();
}
