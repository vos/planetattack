#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <QTcpSocket>
#include <QUdpSocket>

#include "game.h"
#include "ship.h"
#include "multiplayer.h"
#include "bihash.h"

class MultiplayerClient : public QObject
{
    Q_OBJECT

public:
    explicit MultiplayerClient(Game *game, Player *player, QObject *parent = NULL);

//    inline QTcpSocket& tcpSocket() { return m_tcpSocket; }
//    inline QUdpSocket& udpSocket() { return m_udpSocket; }

    inline Player* player() const { return m_player; }

    void connectToHost(const QString &hostName, quint16 port);
    void disconnectFromHost();
    inline QHostAddress peerAddress() const { return m_tcpSocket.peerAddress(); }
    inline QString peerName() const { return m_tcpSocket.peerName(); }
    inline quint16 peerPort() const { return m_tcpSocket.peerPort(); }
    inline bool isValid() const { return m_tcpSocket.isValid() && m_udpSocket.isValid(); }
    QAbstractSocket::SocketError error() const;
    QString errorString() const;

signals:
    void connected();
    void disconnected();
    void error(QAbstractSocket::SocketError error);
    void chatMessageReceived(const QString &msg, Player *player);

public slots:
    void sendChatMessage(const QString &msg);

private slots:
    void socket_connected();
    void socket_disconnected();
    void socket_error(QAbstractSocket::SocketError error);
    void tcpSocket_readyRead();
    void udpSocket_readyRead();

    void game_modeChanged(Game::Mode mode);
    void game_planetAdded(Planet *planet);
    void game_planetRemoved(Planet *planet);
    void game_planetChanged(Planet *planet, Planet::ChangeType changeType);
    void game_resourcesTransferInitiated(Planet *origin, qreal resourceFactor, Ship *ship);

    void player_changed(Player::ChangeType changeType);

private:
    QTcpSocket m_tcpSocket;
    QUdpSocket m_udpSocket;

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
