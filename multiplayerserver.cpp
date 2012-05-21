#include "multiplayerserver.h"

#include <QTcpSocket>
#include "game.h"

MultiplayerServer::MultiplayerServer(Game *game, QObject *parent) :
    QTcpServer(parent),
    m_game(game),
    m_nextPlayerId(1),
    m_nextPlanetId(1)
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
    Q_ASSERT(client);
    Player *player = client->player;

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_disconnected(): %s (%s:%i)",
           qPrintable(player ? QString("'%1' (%2)").arg(player->name()).arg(client->id) : "[no player object]"),
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    if (client->isConnected()) {
        m_game->removePlayer(player);
        m_idPlayerMap.remove(client->id);
        m_playerIdMap.remove(player);

        MultiplayerPacket playerDisconnectedPacket(MultiplayerPacket::PlayerDisconnected);
        playerDisconnectedPacket.stream() << client->id;
        playerDisconnectedPacket.pack();
        sendPacketToOtherClients(playerDisconnectedPacket, socket);
    }
    m_clients.remove(socket);
    delete client;
}

void MultiplayerServer::client_readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

    Client *client = m_clients.value(socket);
    Q_ASSERT(client);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_readyRead(): %li bytes available", (long)socket->bytesAvailable());
#endif

    QDataStream in(socket);
    in.setVersion(MultiplayerPacket::StreamVersion);
    while (socket->bytesAvailable() > 0) {
        if (client->packetSize == 0) {
            if (socket->bytesAvailable() < (int)sizeof(PacketSize))
                return;
            in >> client->packetSize;
        }
        if (socket->bytesAvailable() < client->packetSize)
            return;
        client->packetSize = 0; // reset packet size

        // read packet type
        qint32 packetType; // MultiplayerPacket::PacketType value
        in >> packetType;

#ifdef MULTIPLAYERSERVER_DEBUG
        qDebug("PacketType %i (%s) from %s (%s:%i)", packetType,
               qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)),
               qPrintable(client->player ? QString("'%1' (%2)").arg(client->player->name()).arg(client->id) : "[no player object]"),
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
                m_idPlayerMap.insert(client->id, client->player);
                m_playerIdMap.insert(client->player, client->id);

                // accept the player
                MultiplayerPacket connectAcceptedPacket(MultiplayerPacket::PlayerConnectAccepted);
                connectAcceptedPacket.stream() << client->id;
                connectAcceptedPacket.packAndSend(socket);

                // send the client the other players data
                foreach (Client *otherClient, m_clients.values()) {
                    if (otherClient != client && otherClient->isConnected()) {
                        MultiplayerPacket otherPlayerPacket(MultiplayerPacket::PlayerConnected); // send PlayerList in one packet?
                        otherPlayerPacket.stream() << otherClient->id << *otherClient->player;
                        otherPlayerPacket.packAndSend(socket);
                    }
                }

                // inform the other clients about the new player
                MultiplayerPacket playerConnectedPacket(MultiplayerPacket::PlayerConnected);
                playerConnectedPacket.stream() << client->id << *player;
                playerConnectedPacket.pack();
                sendPacketToOtherClients(playerConnectedPacket, socket);
            } else {
                MultiplayerPacket(MultiplayerPacket::PlayerConnectDenied).send(socket);
            }
            break;
        }
        case MultiplayerPacket::PlayerDisconnect:
            socket->disconnectFromHost();
            break;
        case MultiplayerPacket::Chat: {
            QString msg;
            in >> msg;
            MultiplayerPacket chatPacket(MultiplayerPacket::Chat);
            chatPacket.stream() << msg;
            chatPacket.pack();
            sendPacketToAllClients(chatPacket);
            break;
        }
        case MultiplayerPacket::PlanetAdded: {
            PlanetID tempPlanetId;
            Planet *planet = new Planet;
            in >> tempPlanetId >> *planet;
            if (m_game->addPlanet(planet)) {
                PlanetID planetId = m_nextPlanetId++;
                // TODO: save id -> planet map
                // send real id to the creator
                MultiplayerPacket planetIdPacket(MultiplayerPacket::PlanetId);
                planetIdPacket.stream() << tempPlanetId << planetId;
                planetIdPacket.packAndSend(socket);
                // add id and resend packet to all other clients
                MultiplayerPacket planetAddedPacket(MultiplayerPacket::PlanetAdded);
                planetAddedPacket.stream() << planetId << *planet;
                planetAddedPacket.pack();
                sendPacketToOtherClients(planetAddedPacket, socket); // FIXME: network loop!?
            }
            break;
        }
        default:
            qWarning("MultiplayerServer::client_readyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerServer::sendPacketToOtherClients(const MultiplayerPacket &packet, const QTcpSocket *sender)
{
    QHash<QTcpSocket*, Client*>::const_iterator it;
    for (it = m_clients.constBegin(); it != m_clients.constEnd(); ++it) {
        QTcpSocket *clientSocket = it.key();
        const Client *client = it.value();
        if (clientSocket != sender && client->isConnected()) {
            packet.send(clientSocket);
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
