#include <QtGui/QApplication>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "canvas.h"

#include "playerintelligence.h"

int main(int argc, char *argv[])
{
//    QApplication::setGraphicsSystem("opengl");
    QApplication a(argc, argv);

    qRegisterMetaType<Player*>("Player*");
    qRegisterMetaType<PlayerIntelligence*>("PlayerIntelligence*");

    QWidget *w;
    if (a.arguments().contains("-noui")) {
        w = new Canvas;
    } else {
        w = new MainWindow;
    }

    w->resize(1024, 768);
    // center window
    QDesktopWidget *desktop = QApplication::desktop();
    QRect desktopRect = desktop->availableGeometry();
    w->move(desktopRect.width() / 2 - w->width() / 2,
            desktopRect.height() / 2 - w->height() / 2);
    w->show();

    return a.exec();
}
