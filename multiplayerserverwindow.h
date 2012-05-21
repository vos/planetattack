#ifndef MULTIPLAYERSERVERWINDOW_H
#define MULTIPLAYERSERVERWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include "multiplayerserver.h"

namespace Ui {
class MultiplayerServerWindow;
}

class MultiplayerServerWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MultiplayerServerWindow(QWidget *parent = NULL);
    ~MultiplayerServerWindow();

    bool isListening() const;

public slots:
    bool startListening(const QHostAddress &address = QHostAddress::Any, quint16 port = 0);
    void stopListening();
    bool toggleListening();

private:
    Ui::MultiplayerServerWindow *ui;
    QLabel *m_statusLabel;

    QHostAddress m_address;
    quint16 m_port;

    Game *m_game;
    MultiplayerServer *m_server;

};

#endif // MULTIPLAYERSERVERWINDOW_H
