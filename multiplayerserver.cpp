#include "multiplayerserver.h"

#include <QTcpSocket>
#include "multiplayerpacket.h"
#include "game.h"

MultiplayerServer::MultiplayerServer(Game *game, QObject *parent) :
    QTcpServer(parent),
    m_game(game),
    m_nextPlayerId(1)
{
}

MultiplayerServer::~MultiplayerServer()
{
    foreach (QTcpSocket *socket, m_clients.keys()) {
        socket->disconnectFromHost();
    }
}

void MultiplayerServer::incomingConnection(int socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    // addPendingConnection(socket); // not needed!

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::incomingConnection(%i): %s:%i", socketDescriptor,
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    connect(socket, SIGNAL(disconnected()), SLOT(client_disconnected()));
    connect(socket, SIGNAL(readyRead()), SLOT(client_readyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(client_error(QAbstractSocket::SocketError)));

    Client *client = new Client;
    m_clients.insert(socket, client);

    MultiplayerPacket(MultiplayerPacket::ConnectionAccepted).send(socket); // Multiplayer::ConnectionRefused

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("ConnectionAccepted");
#endif
}

void MultiplayerServer::client_disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

    Client *client = m_clients.value(socket);
    Player *player = client->player;

#ifdef MULTIPLAYERSERVER_DEBUG
    QString name = player ? player->name() : "[no player object]";
    qDebug("MultiplayerServer::client_disconnected(): '%s' (%s:%i)", qPrintable(name),
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    if (client->isConnected()) {
        m_game->removePlayer(player);

        MultiplayerPacket playerDisconnectedPacket(MultiplayerPacket::PlayerDisconnected);
        playerDisconnectedPacket.stream() << client->id;
        sendPacketToOtherClients(playerDisconnectedPacket, socket);
    }
    m_clients.remove(socket);
    delete client;
}

void MultiplayerServer::client_readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_readyRead(): %li bytes available", (long)socket->bytesAvailable());
#endif

    Client *client = m_clients.value(socket);
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_8);
    qint64 bytesAvailable;
    while ((bytesAvailable = socket->bytesAvailable()) > 0) {
        if (client->packetSize == 0) {
            if (bytesAvailable < (int)sizeof(quint32)) // packet size
                return;
            in >> client->packetSize;
        }
        if (bytesAvailable < client->packetSize)
            return;
        client->packetSize = 0; // reset packet size

        // read packet type
        qint32 packetType; // MultiplayerPacket::PacketType value
        in >> packetType;

#ifdef MULTIPLAYERSERVER_DEBUG
        qDebug("PacketType %i (%s) from '%s' (%s:%i)", packetType,
               qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)),
               qPrintable(client->player ? client->player->name() : "[no player object]"),
               qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

        // read and handle packet data
        switch ((MultiplayerPacket::PacketType)packetType) {
        case MultiplayerPacket::PlayerConnect: {
            Player *player = new Player;
            in >> *player;
            if (m_game->addPlayer(player)) {
                client->player = player;
                client->id = m_nextPlayerId++;

                // accept the player
                MultiplayerPacket connectAcceptedPacket(MultiplayerPacket::PlayerConnectAccepted);
                connectAcceptedPacket.stream() << client->id;
                connectAcceptedPacket.send(socket);

                // send the client the other players data
                foreach (Client *otherClient, m_clients.values()) {
                    if (otherClient != client && otherClient->isConnected()) {
                        MultiplayerPacket otherPlayerPacket(MultiplayerPacket::PlayerConnected); // send PlayerList in one packet?
                        otherPlayerPacket.stream() << otherClient->id << *otherClient->player;
                        otherPlayerPacket.send(socket);
                    }
                }

                // inform the other clients about the new player
                MultiplayerPacket playerConnectedPacket(MultiplayerPacket::PlayerConnected);
                playerConnectedPacket.stream() << client->id << *player;
                sendPacketToOtherClients(playerConnectedPacket, socket);
            } else {
                MultiplayerPacket(MultiplayerPacket::PlayerConnectDenied).send(socket);
            }
            break;
        }
        case MultiplayerPacket::PlayerDisconnect:
            socket->disconnectFromHost();
            break;
        case MultiplayerPacket::PlanetAdded: {
            Planet *planet = new Planet;
            in >> *planet;
            if (m_game->addPlanet(planet)) {
                // resend packet to all other clients
                MultiplayerPacket packet(MultiplayerPacket::PlanetAdded);
                packet.stream() << *planet;
                sendPacketToOtherClients(packet, socket);
            }
            break;
        }
        default:
            qWarning("MultiplayerServer::client_readyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerServer::sendPacketToOtherClients(MultiplayerPacket &packet, const QTcpSocket *sender)
{
    foreach (QTcpSocket *client, m_clients.keys()) {
        if (client != sender) {
            packet.send(client);
        }
    }
}

void MultiplayerServer::client_error(QAbstractSocket::SocketError error)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_error(%i) => %s", error, qPrintable(socket->errorString()));
#endif
}
