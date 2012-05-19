#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <QTcpSocket>

class Game;
class Player;

class MultiplayerClient : public QTcpSocket
{
    Q_OBJECT

public:
    explicit MultiplayerClient(Game *game, Player *player, QObject *parent = NULL);

private slots:
    void socket_connected();
    void socket_disconnected();
    void socket_readyRead();
    void socket_error(QAbstractSocket::SocketError error);

private:
    Game *m_game;
    Player *m_player;
    quint32 m_packetSize;

};

#endif // MULTIPLAYERCLIENT_H
