#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <QTcpSocket>
#include <QHash>

class Game;
class Player;
class Planet;

typedef quint8 PlayerID;

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

    void game_planetAdded(Planet *planet);

private:
    Game *m_game;
    Player *m_player;
    quint32 m_packetSize;

    PlayerID m_playerId;
    QHash<PlayerID, Player*> m_idPlayerMap;

};

#endif // MULTIPLAYERCLIENT_H
