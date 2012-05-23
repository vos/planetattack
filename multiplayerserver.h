#ifndef MULTIPLAYERSERVER_H
#define MULTIPLAYERSERVER_H

#include <QTcpServer>
#include <QHash>

#include "multiplayer.h"

class MultiplayerServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MultiplayerServer(Game *game, QObject *parent = NULL);
    ~MultiplayerServer();

    inline PlayerID playerId(Player *player) const { return m_playerIdMap.value(player); }
    inline Player* player(PlayerID id) const { return m_idPlayerMap.value(id); }

    inline void sendPacketToAllClients(const MultiplayerPacket &packet) { sendPacketToOtherClients(packet, NULL); }

signals:
    void chatMessageReceived(const QString &msg, Player *player);

private slots:
    void client_disconnected();
    void client_readyRead();
    void client_error(QAbstractSocket::SocketError error);

private:
    Game *m_game;

    struct Client {
        explicit inline Client(Player *p = NULL) : player(p), id(0), packetSize(0) { }
        Player *player;
        PlayerID id;
        PacketSize packetSize;
        inline bool isConnected() const { return player != NULL; }
    };
    QHash<QTcpSocket*, Client*> m_clients;

    PlayerID m_nextPlayerId;
    PlanetID m_nextPlanetId;

    QHash<PlayerID, Player*> m_idPlayerMap;
    QHash<Player*, PlayerID> m_playerIdMap;

    QHash<PlanetID, Planet*> m_idPlanetMap;
    QHash<Planet*, PlanetID> m_planetIdMap;

    void incomingConnection(int socketDescriptor);
    void sendPacketToOtherClients(const MultiplayerPacket &packet, const QTcpSocket *sender);

};

#endif // MULTIPLAYERSERVER_H
