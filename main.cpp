#include <QtGui/QApplication>
#include <QDesktopWidget>

#include "mainwindow.h"
#include "randomutil.h"
#include "canvas.h"
#include "playerintelligence.h"
#include "multiplayerserverwindow.h"

quint16 readUShort(const QStringList &arguments, const QString &name, quint16 defaultValue = 0)
{
    if (arguments.isEmpty())
        return defaultValue;

    int index = arguments.indexOf(name);
    if (index < 0 || index + 1 >= arguments.count())
        return defaultValue;

    bool ok;
    quint16 value = arguments.at(index + 1).toUShort(&ok);
    return ok ? value : defaultValue;
}

int main(int argc, char *argv[])
{
//    QApplication::setGraphicsSystem("opengl");
    QApplication a(argc, argv);

    qRegisterMetaType<RandomUtil*>("RandomUtil*");
    qRegisterMetaType<GameTime*>("GameTime*");
    qRegisterMetaType<Player*>("Player*");
    qRegisterMetaType<Planet*>("Planet*");
    qRegisterMetaType<Ship*>("Ship*");
    qRegisterMetaType<PlayerIntelligence*>("PlayerIntelligence*");

    RandomUtil::init();
    QStringList arguments = a.arguments();

#ifdef DEBUG
    qDebug() << "PlanetAttack started with arguments:" << arguments;
#endif

    QWidget *w;
    if (arguments.contains("-server")) {
        QHostAddress address = QHostAddress::Any;
        quint16 port = readUShort(arguments, "-port", MultiplayerServer::DEFAULT_PORT);

        MultiplayerServerWindow *server = new MultiplayerServerWindow;
        server->startListening(address, port);
        w = server;
    } else {
        if (arguments.contains("-noui")) {
            w = new Canvas;
        } else {
            w = new MainWindow;
        }
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
