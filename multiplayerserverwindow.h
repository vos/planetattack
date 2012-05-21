#ifndef MULTIPLAYERSERVERWINDOW_H
#define MULTIPLAYERSERVERWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QAbstractListModel>

#include "multiplayerserver.h"

class PlayerListModel;

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

private slots:
    void on_inputLineEdit_returnPressed();

private:
    Ui::MultiplayerServerWindow *ui;
    QLabel *m_statusLabel;

    QHostAddress m_address;
    quint16 m_port;

    Game *m_game;
    MultiplayerServer *m_server;
    PlayerListModel *m_playerListModel;

};


class PlayerListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PlayerListModel(MultiplayerServer *server, QObject *parent = NULL);

    inline int rowCount(const QModelIndex & = QModelIndex()) const {
        return m_playerList.count();
    }

    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;

public slots:
    int addPlayer(Player *player);
    int removePlayer(Player *player);

private:
    QList<Player*> m_playerList;
    MultiplayerServer *m_server;

};

#endif // MULTIPLAYERSERVERWINDOW_H
