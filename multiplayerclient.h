#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <QTcpSocket>
#include <QHash>

#include "multiplayer.h"

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
    PacketSize m_packetSize;
    PlayerID m_playerId; // own player id

    // bimap wrapper?
    QHash<PlayerID, Player*> m_idPlayerMap;
    QHash<Player*, PlayerID> m_playerIdMap;

    QHash<PlanetID, Planet*> m_idPlanetMap;
    QHash<Planet*, PlanetID> m_planetIdMap;

    PlanetID m_nextTempPlanetId;
    QHash<PlanetID, Planet*> m_tempIdPlanetMap;

};

#endif // MULTIPLAYERCLIENT_H
