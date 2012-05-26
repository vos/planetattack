#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <QTcpSocket>

#include "game.h"
#include "multiplayer.h"
#include "bihash.h"

class MultiplayerClient : public QTcpSocket
{
    Q_OBJECT

public:
    explicit MultiplayerClient(Game *game, Player *player, QObject *parent = NULL);

signals:
    void chatMessageReceived(const QString &msg, Player *player);

private slots:
    void socket_connected();
    void socket_disconnected();
    void socket_readyRead();
    void socket_error(QAbstractSocket::SocketError error);

    void game_modeChanged(Game::Mode mode);
    void game_planetAdded(Planet *planet);
    void game_planetRemoved(Planet *planet);
    void game_planetChanged(Planet *planet, Planet::ChangeType changeType);

private:
    Game *m_game;
    Player *m_player;
    PacketSize m_packetSize;
    PlayerID m_playerId; // own player id

    BiHash<PlayerID, Player*> m_idPlayerMap;
    BiHash<PlanetID, Planet*> m_idPlanetMap;

    PlanetID m_nextTempPlanetId;
    QHash<PlanetID, Planet*> m_tempIdPlanetMap;

};

#endif // MULTIPLAYERCLIENT_H
