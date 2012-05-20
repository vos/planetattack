#ifndef MULTIPLAYERSERVER_H
#define MULTIPLAYERSERVER_H

#include <QTcpServer>
#include <QHash>

#include "multiplayerpacket.h"

class Game;
class Player;

typedef quint8 PlayerID; // uint8 enough?

class MultiplayerServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MultiplayerServer(Game *game, QObject *parent = NULL);
    ~MultiplayerServer();

private slots:
    void client_disconnected();
    void client_readyRead();
    void client_error(QAbstractSocket::SocketError error);

private:
    Game *m_game;

    struct Client {
        explicit inline Client(Player *p = NULL) : player(p), id(-1), packetSize(0) { }
        Player *player;
        PlayerID id;
        quint32 packetSize;
        inline bool isConnected() const { return player != NULL; }
    };
    QHash<QTcpSocket*, Client*> m_clients;

    PlayerID m_nextPlayerId;

    void incomingConnection(int socketDescriptor);
    void sendPacketToOtherClients(MultiplayerPacket &packet, const QTcpSocket *sender);

};

#endif // MULTIPLAYERSERVER_H
