#ifndef MULTIPLAYERSERVER_H
#define MULTIPLAYERSERVER_H

#include <QTcpServer>
#include <QUdpSocket>

#include "multiplayer.h"
#include "bihash.h"

class MultiplayerServer : public QObject
{
    Q_OBJECT

public:
    static const quint16 DEFAULT_PORT = 54321;

    explicit MultiplayerServer(Game *game, QObject *parent = NULL);
    ~MultiplayerServer();

    bool listen(const QHostAddress &address = QHostAddress::Any, quint16 port = DEFAULT_PORT);
    void close();
    inline bool isListening() const { return m_tcpServer.isListening() && m_udpSocket.isValid(); }
    inline QHostAddress serverAddress() const { return m_tcpServer.serverAddress(); }
    inline quint16 serverPort() const { return m_tcpServer.serverPort(); }
    QAbstractSocket::SocketError serverError() const;
    QString errorString() const;

    inline PlayerID playerId(Player *player) const { return m_idPlayerMap.key(player); }
    inline Player* player(PlayerID id) const { return m_idPlayerMap.value(id); }

    inline void sendTcpPacketToAllClients(const MultiplayerPacket &packet) { sendTcpPacketToOtherClients(packet, NULL); }
    inline void sendUdpPacketToAllClients(const MultiplayerPacket &packet) { sendUdpPacketToOtherClients(packet, 0); }

signals:
    void chatMessageReceived(const QString &msg, Player *player);

public slots:
    void sendChatMessage(const QString &msg);

private slots:
    void client_newConnection();
    void client_disconnected();
    void client_error(QAbstractSocket::SocketError error);
    void client_tcpReadyRead();
    void client_udpReadyRead();

private:
    QTcpServer m_tcpServer;
    QUdpSocket m_udpSocket;

    Game *m_game;

    struct Client {
        explicit inline Client(Player *p = NULL) : player(p), id(0), packetSize(0) { }
        Player *player;
        PlayerID id;
        PacketSize packetSize;
        inline bool isConnected() const { return player != NULL; }
    };
    QHash<QTcpSocket*, Client*> m_tcpClientMap;
    BiHash<PlayerID, QPair<QHostAddress, quint16> > m_udpClientMap; // PlayerID -> <Address, Port>

    PlayerID m_nextPlayerId;
    PlanetID m_nextPlanetId;

    BiHash<PlayerID, Player*> m_idPlayerMap;
    BiHash<PlanetID, Planet*> m_idPlanetMap;

    void sendTcpPacketToOtherClients(const MultiplayerPacket &packet, const QTcpSocket *sender);
    void sendUdpPacketToOtherClients(const MultiplayerPacket &packet, const PlayerID senderId);

};

#endif // MULTIPLAYERSERVER_H
