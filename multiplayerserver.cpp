#include "multiplayerserver.h"

#include <QTcpSocket>
#include "game.h"

MultiplayerServer::MultiplayerServer(Game *game, QObject *parent) :
    QObject(parent),
    m_game(game),
    m_nextPlayerId(1),
    m_nextPlanetId(1)
{
    connect(&m_tcpServer, SIGNAL(newConnection()), SLOT(client_newConnection()));
    connect(&m_udpSocket, SIGNAL(readyRead()), SLOT(client_udpReadyRead()));
}

MultiplayerServer::~MultiplayerServer()
{
    foreach (QTcpSocket *socket, m_tcpClientMap.keys()) {
        socket->disconnectFromHost();
    }
    qDeleteAll(m_clients.values());
}

bool MultiplayerServer::listen(const QHostAddress &address, quint16 port)
{
    return m_tcpServer.listen(address, port) && m_udpSocket.bind(address, port);
}

void MultiplayerServer::close()
{
    m_tcpServer.close();
    m_udpSocket.close();
}

QAbstractSocket::SocketError MultiplayerServer::serverError() const
{
    return m_tcpServer.serverError() != QAbstractSocket::UnknownSocketError ? m_tcpServer.serverError() : m_udpSocket.error();
}

QString MultiplayerServer::errorString() const
{
    return m_tcpServer.serverError() != QAbstractSocket::UnknownSocketError ? m_tcpServer.errorString() : m_udpSocket.errorString();
}

void MultiplayerServer::client_newConnection()
{
    QTcpSocket *socket = m_tcpServer.nextPendingConnection();
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_newConnection(): %s:%i", qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    connect(socket, SIGNAL(disconnected()), SLOT(client_disconnected()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(client_error(QAbstractSocket::SocketError)));
    connect(socket, SIGNAL(readyRead()), SLOT(client_tcpReadyRead()));

    Client *client = new Client;
    m_clients.insert(qMakePair(socket->peerAddress(), socket->peerPort()), client);
    m_tcpClientMap.insert(socket, client);

    MultiplayerPacket(MultiplayerPacket::ConnectionAccepted).send(socket); // Multiplayer::ConnectionRefused

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("ConnectionAccepted");
#endif
}

void MultiplayerServer::client_disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

    Client *client = m_tcpClientMap.value(socket);
    Q_ASSERT(client);
    Player *player = client->player;

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_disconnected(): %s (%s:%i)",
           qPrintable(player ? QString("'%1' (%2)").arg(player->name()).arg(client->id) : "[no player object]"),
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    if (client->isConnected()) {
        m_game->removePlayer(player);
        m_idPlayerMap.removeKey(client->id);

        MultiplayerPacket playerDisconnectedPacket(MultiplayerPacket::PlayerDisconnected);
        playerDisconnectedPacket.stream() << client->id;
        playerDisconnectedPacket.pack();
        sendTcpPacketToOtherClients(playerDisconnectedPacket, socket);
    }
    m_clients.removeValue(client);
    delete client;
}

void MultiplayerServer::client_error(QAbstractSocket::SocketError error)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_error(%i) => %s", error, qPrintable(socket->errorString()));
#endif
}

void MultiplayerServer::client_tcpReadyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

    Client *client = m_tcpClientMap.value(socket);
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
        EnumType packetType; // MultiplayerPacket::PacketType
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
                sendTcpPacketToOtherClients(playerConnectedPacket, socket);
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
            emit chatMessageReceived(msg, client->player);
            MultiplayerPacket chatPacket(MultiplayerPacket::Chat);
            chatPacket.stream() << msg << client->id;
            chatPacket.pack();
            sendTcpPacketToOtherClients(chatPacket, socket);
            break;
        }
        case MultiplayerPacket::ModeChanged: {
            EnumType mode; // Game::Mode
            in >> mode;
            qDebug("mode = %s", qPrintable(Game::modeString((Game::Mode)mode)));
            m_game->setMode((Game::Mode)mode);
            MultiplayerPacket modeChangedPacket(MultiplayerPacket::ModeChanged);
            modeChangedPacket.stream() << mode;
            sendTcpPacketToOtherClients(modeChangedPacket, socket);
            break;
        }
        case MultiplayerPacket::PlanetAdded: {
            PlanetID tempPlanetId;
            Planet *planet = new Planet;
            in >> tempPlanetId >> *planet;
            if (m_game->addPlanet(planet)) {
                PlanetID planetId = m_nextPlanetId++;
                m_idPlanetMap.insert(planetId, planet);
                // send real id to the creator
                MultiplayerPacket planetIdPacket(MultiplayerPacket::PlanetId);
                planetIdPacket.stream() << tempPlanetId << planetId;
                planetIdPacket.packAndSend(socket);
                // add id and resend packet to all other clients
                MultiplayerPacket planetAddedPacket(MultiplayerPacket::PlanetAdded);
                planetAddedPacket.stream() << planetId << *planet << client->id;
                planetAddedPacket.pack();
                sendTcpPacketToOtherClients(planetAddedPacket, socket);
            }
            break;
        }
        case MultiplayerPacket::PlanetRemoved: {
            PlanetID planetId;
            in >> planetId;
            Planet *planet = m_idPlanetMap.takeValue(planetId);
            Q_ASSERT(planet);
            if (m_game->removePlanet(planet)) {
                MultiplayerPacket planetRemovedPacket(MultiplayerPacket::PlanetRemoved);
                planetRemovedPacket.stream() << planetId;
                planetRemovedPacket.pack();
                sendTcpPacketToOtherClients(planetRemovedPacket, socket);
            }
            break;
        }
        default:
            qWarning("MultiplayerServer::client_readyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerServer::client_udpReadyRead()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerServer::client_udpReadyRead(): %li bytes available", (long)m_udpSocket.bytesAvailable());
#endif

    while (m_udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket.pendingDatagramSize());
        QHostAddress senderAddress;
        quint16 senderPort;
        m_udpSocket.readDatagram(datagram.data(), datagram.size(), &senderAddress, &senderPort);

        // TODO: get client, udp port is not constant!?
        Client *client = m_clients.value(qMakePair(senderAddress, senderPort));
        Q_ASSERT(client);

        QDataStream in(datagram);
        in.setVersion(MultiplayerPacket::StreamVersion);

        // skip packet size (ignored with datagrams over UDP)
        in.skipRawData(sizeof(PacketSize));

        // read packet type
        EnumType packetType; // MultiplayerPacket::PacketType
        in >> packetType;

#ifdef MULTIPLAYERCLIENT_DEBUG
        qDebug("PacketType %i (%s)", packetType, qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)));
#endif

        // read and handle packet data
        switch ((MultiplayerPacket::PacketType)packetType) {
        case MultiplayerPacket::PlanetChanged: {
            PlanetID planetId;
            EnumType changeType; // Planet::ChangeType
            in >> planetId >> changeType;
            Planet *planet = m_idPlanetMap.value(planetId);
            Q_ASSERT(planet);
            MultiplayerPacket planetChangedPacket(MultiplayerPacket::PlanetChanged);
            planetChangedPacket.stream() << planetId << changeType;
            switch ((Planet::ChangeType)changeType) {
            case Planet::PositionChange: {
                QVector2D position;
                in >> position;
                planet->setPosition(position);
                planetChangedPacket.stream() << position;
                break;
            }
            case Planet::RadiusChange: {
                qreal radius;
                in >> radius;
                planet->setRadius(radius);
                planetChangedPacket.stream() << radius;
                break;
            }
            default:
                qWarning("invalid Planet::ChangeType: %d", changeType);
            }
            planetChangedPacket.pack();
            sendUdpPacketToOtherClients(planetChangedPacket, client);
            break;
        }
        default:
            qWarning("MultiplayerServer::client_udpReadyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerServer::sendChatMessage(const QString &msg)
{
    MultiplayerPacket packet(MultiplayerPacket::Chat);
    packet.stream() << msg << PlayerID(0); // server has PlayerID 0
    packet.pack();
    sendTcpPacketToAllClients(packet);
}

void MultiplayerServer::sendTcpPacketToOtherClients(const MultiplayerPacket &packet, const QTcpSocket *sender)
{
    QHash<QTcpSocket*, Client*>::const_iterator it;
    for (it = m_tcpClientMap.constBegin(); it != m_tcpClientMap.constEnd(); ++it) {
        QTcpSocket *clientSocket = it.key();
        const Client *client = it.value();
        if (clientSocket != sender && client->isConnected()) {
            packet.send(clientSocket);
        }
    }
}

void MultiplayerServer::sendUdpPacketToOtherClients(const MultiplayerPacket &packet, const Client *sender)
{
    QHash<QPair<QHostAddress, quint16>, Client*>::const_iterator it;
    for (it = m_clients.hash().constBegin(); it != m_clients.hash().constEnd(); ++it) {
        const QPair<QHostAddress, quint16> &clientAddress = it.key();
        const Client *client = it.value();
        if (client != sender && client->isConnected()) {
            m_udpSocket.writeDatagram(packet.data(), clientAddress.first, clientAddress.second);
        }
    }
}
