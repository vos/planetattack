#include "multiplayerclient.h"

MultiplayerClient::MultiplayerClient(Game *game, Player *player, QObject *parent) :
    QObject(parent),
    m_game(game),
    m_player(player),
    m_packetSize(0),
    m_playerId(0),
    m_nextTempPlanetId(1)
{
    connect(&m_tcpSocket, SIGNAL(connected()), SLOT(socket_connected()));
    connect(&m_tcpSocket, SIGNAL(disconnected()), SLOT(socket_disconnected()));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socket_error(QAbstractSocket::SocketError)));
    connect(&m_tcpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SIGNAL(error(QAbstractSocket::SocketError)));
    connect(&m_tcpSocket, SIGNAL(readyRead()), SLOT(tcpSocket_readyRead()));

    connect(&m_udpSocket, SIGNAL(connected()), SLOT(socket_connected()));
    connect(&m_udpSocket, SIGNAL(disconnected()), SLOT(socket_disconnected()));
    connect(&m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socket_error(QAbstractSocket::SocketError)));
    connect(&m_udpSocket, SIGNAL(error(QAbstractSocket::SocketError)), SIGNAL(error(QAbstractSocket::SocketError)));
    connect(&m_udpSocket, SIGNAL(readyRead()), SLOT(udpSocket_readyRead()));

    connect(m_game, SIGNAL(modeChanged(Game::Mode)), SLOT(game_modeChanged(Game::Mode)));
    connect(m_game, SIGNAL(planetAdded(Planet*)), SLOT(game_planetAdded(Planet*)));
    connect(m_game, SIGNAL(planetRemoved(Planet*)), SLOT(game_planetRemoved(Planet*)), Qt::DirectConnection);
    connect(m_game, SIGNAL(planetChanged(Planet*,Planet::ChangeType)), SLOT(game_planetChanged(Planet*,Planet::ChangeType)));

    connect(m_player, SIGNAL(changed(Player::ChangeType)), SLOT(player_changed(Player::ChangeType)));
}

void MultiplayerClient::connectToHost(const QString &hostName, quint16 port)
{
    m_tcpSocket.connectToHost(hostName, port);
    m_udpSocket.connectToHost(hostName, port);
}

void MultiplayerClient::disconnectFromHost()
{
    m_tcpSocket.disconnectFromHost();
    m_udpSocket.disconnectFromHost();
}

QAbstractSocket::SocketError MultiplayerClient::error() const
{
    return m_tcpSocket.error() != QAbstractSocket::UnknownSocketError ? m_tcpSocket.error() : m_udpSocket.error();
}

QString MultiplayerClient::errorString() const
{
    return m_tcpSocket.error() != QAbstractSocket::UnknownSocketError ? m_tcpSocket.errorString() : m_udpSocket.errorString();
}

void MultiplayerClient::socket_connected()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_connected()");
#endif

    if (m_tcpSocket.state() == QAbstractSocket::ConnectedState && m_udpSocket.state() == QAbstractSocket::ConnectedState) {
        emit connected();
    }
}

void MultiplayerClient::socket_disconnected()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_disconnected()");
#endif

    if (m_tcpSocket.state() == QAbstractSocket::UnconnectedState && m_udpSocket.state() == QAbstractSocket::UnconnectedState) {
        emit disconnected();
    }
}

void MultiplayerClient::socket_error(QAbstractSocket::SocketError error)
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_error(%i) => %s", error, qPrintable(errorString()));
#endif
}

void MultiplayerClient::tcpSocket_readyRead()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::tcpSocket_readyRead(): %li bytes available", (long)m_tcpSocket.bytesAvailable());
#endif

    QDataStream in(&m_tcpSocket);
    in.setVersion(MultiplayerPacket::StreamVersion);
    while (m_tcpSocket.bytesAvailable() > 0) {
        if (m_packetSize == 0) {
            if (m_tcpSocket.bytesAvailable() < (int)sizeof(PacketSize))
                return;
            in >> m_packetSize;
        }
        if (m_tcpSocket.bytesAvailable() < m_packetSize)
            return;
        m_packetSize = 0; // reset packet size

        // read packet type
        EnumType packetType; // MultiplayerPacket::PacketType
        in >> packetType;

#ifdef MULTIPLAYERCLIENT_DEBUG
        qDebug("PacketType %i (%s)", packetType, qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)));
#endif

        // read and handle packet data
        switch ((MultiplayerPacket::PacketType)packetType) {
        case MultiplayerPacket::ConnectionAccepted: {
            MultiplayerPacket packet(MultiplayerPacket::PlayerConnect);
            packet << *m_player;
            packet.packAndSend(&m_tcpSocket);
            break;
        }
        case MultiplayerPacket::ConnectionRefused:
            // TODO
            break;
        case MultiplayerPacket::PlayerConnectAccepted: {
            in >> m_playerId;
            m_idPlayerMap.insert(m_playerId, m_player);
            qDebug("playerId = %d", m_playerId);
            // send UDP packet to register a socket to this client at the server
            MultiplayerPacket udpRegisterPacket(MultiplayerPacket::UdpRegister);
            udpRegisterPacket << m_playerId;
            udpRegisterPacket.packAndSend(&m_udpSocket);
            break;
        }
        case MultiplayerPacket::PlayerConnectDenied:
            // TODO
            break;
        case MultiplayerPacket::PlayerConnected: {
            PlayerID playerId;
            Player *player = new Player;
            in >> playerId >> *player;
            if (m_game->addPlayer(player)) {
                m_idPlayerMap.insert(playerId, player);
                qDebug("playerId = %d, name = %s", playerId, qPrintable(player->name()));
            }
            break;
        }
        case MultiplayerPacket::PlayerDisconnected: {
            PlayerID playerId;
            in >> playerId;
            Player *player = m_idPlayerMap.takeValue(playerId);
            Q_ASSERT(player);
            qDebug("playerId = %d, name = %s", playerId, qPrintable(player->name()));
            m_game->removePlayer(player);
            break;
        }
        case MultiplayerPacket::PlayerChanged: {
            PlayerID playerId;
            EnumType changeType; // Player::ChangeType
            in >> playerId >> changeType;
            Player *player = m_idPlayerMap.value(playerId);
            Q_ASSERT(player);
            switch ((Player::ChangeType)changeType) {
            case Player::NameChange: {
                QString name;
                in >> name;
                player->setName(name);
                break;
            }
            case Player::ColorChange: {
                QColor color;
                in >> color;
                player->setColor(color);
                break;
            }
            case Player::ResourceFactorChange: {
                qreal resourceFactor;
                in >> resourceFactor;
                player->setResourceFactor(resourceFactor);
                break;
            }
            default:
                qWarning("invalid Player::ChangeType: %d", changeType);
            }
            break;
        }
        case MultiplayerPacket::Chat: {
            QString msg;
            PlayerID senderId;
            in >> msg >> senderId;
            Player *player = senderId > 0 ? m_idPlayerMap.value(senderId) : NULL; // server has PlayerID 0
            emit chatMessageReceived(msg, player);
            break;
        }
        case MultiplayerPacket::ModeChanged: {
            EnumType mode; // Game::Mode
            in >> mode;
            m_game->setMode((Game::Mode)mode);
            break;
        }
        case MultiplayerPacket::PlanetId: {
            PlanetID tempPlanetId, planetId;
            in >> tempPlanetId >> planetId;
            Planet *planet = m_tempIdPlanetMap.take(tempPlanetId);
            Q_ASSERT(planet);
            m_idPlanetMap.insert(planetId, planet);
            break;
        }
        case MultiplayerPacket::PlanetAdded: {
            PlanetID planetId;
            Planet *planet = new Planet;
            PlayerID playerId;
            in >> planetId >> *planet >> playerId;
            Player *player = m_idPlayerMap.value(playerId);
            Q_ASSERT(player);
            planet->setPlayer(player);
            // add the id first (to avoid a network loop)
            m_idPlanetMap.insert(planetId, planet);
            if (!m_game->addPlanet(planet)) {
                // should never fail, just in case remove the planet again
                m_idPlanetMap.removeKey(planetId);
            }
            break;
        }
        case MultiplayerPacket::PlanetRemoved: {
            PlanetID planetId;
            in >> planetId;
            Planet *planet = m_idPlanetMap.takeValue(planetId);
            Q_ASSERT(planet);
            m_game->removePlanet(planet);
            break;
        }
        default:
            qWarning("MultiplayerClient::socket_readyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerClient::udpSocket_readyRead()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::udpSocket_readyRead(): %li bytes available", (long)m_udpSocket.bytesAvailable());
#endif

    while (m_udpSocket.hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(m_udpSocket.pendingDatagramSize());
        m_udpSocket.readDatagram(datagram.data(), datagram.size());

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
            switch ((Planet::ChangeType)changeType) {
            case Planet::PositionChange:
                in >> planet->m_position;
                break;
            case Planet::RadiusChange:
                in >> planet->m_radius;
                break;
            case Planet::ResourcesChange:
                in >> planet->m_resources;
                break;
            case Planet::ProductionFactorChange:
                in >> planet->m_productionFactor;
                break;
            default:
                qWarning("invalid Planet::ChangeType: %d", changeType);
            }
            break;
        }
        default:
            qWarning("MultiplayerClient::udpSocket_readyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerClient::sendChatMessage(const QString &msg)
{
    MultiplayerPacket packet(MultiplayerPacket::Chat);
    packet << msg;
    packet.packAndSend(&m_tcpSocket);
}

void MultiplayerClient::game_modeChanged(Game::Mode mode)
{
    MultiplayerPacket packet(MultiplayerPacket::ModeChanged);
    packet << (EnumType)mode;
    packet.packAndSend(&m_tcpSocket);
}

void MultiplayerClient::game_planetAdded(Planet *planet)
{
    // only send if the planet has no id (newly created planet)
    if (!m_idPlanetMap.containsValue(planet)) {
        PlanetID tempPlanetId = m_nextTempPlanetId++;
        m_tempIdPlanetMap.insert(tempPlanetId, planet);
        MultiplayerPacket packet(MultiplayerPacket::PlanetAdded);
        packet << tempPlanetId << *planet;
        packet.packAndSend(&m_tcpSocket);
    }
}

void MultiplayerClient::game_planetRemoved(Planet *planet)
{
    // only send if the planet is not yet removed (locally removed)
    if (m_idPlanetMap.containsValue(planet)) {
        PlanetID planetId = m_idPlanetMap.takeKey(planet);
        Q_ASSERT(planetId > 0);
        MultiplayerPacket packet(MultiplayerPacket::PlanetRemoved);
        packet << planetId;
        packet.packAndSend(&m_tcpSocket);
    }
}

void MultiplayerClient::game_planetChanged(Planet *planet, Planet::ChangeType changeType)
{
    if (m_game->mode() != Game::EditorMode)
        return; // only send in editor mode

    if (m_idPlanetMap.containsValue(planet)) {
        PlanetID planetId = m_idPlanetMap.key(planet);
        Q_ASSERT(planetId > 0);
        MultiplayerPacket packet(MultiplayerPacket::PlanetChanged);
        packet << planetId << (EnumType)changeType;
        switch (changeType) {
        case Planet::PositionChange:
            packet << planet->position();
            break;
        case Planet::RadiusChange:
            packet << planet->radius();
            break;
        case Planet::ResourcesChange:
            packet << planet->resources();
            break;
        case Planet::ProductionFactorChange:
            packet << planet->productionFactor();
            break;
        default:
            return; // abort packet transmission
        }
        packet.packAndSend(&m_udpSocket);
    }
}

void MultiplayerClient::player_changed(Player::ChangeType changeType)
{
    MultiplayerPacket packet(MultiplayerPacket::PlayerChanged);
    packet << (EnumType)changeType;
    switch (changeType) {
    case Player::NameChange:
        packet << m_player->name();
        break;
    case Player::ColorChange:
        packet << m_player->color();
        break;
    case Player::ResourceFactorChange:
        packet << m_player->resourceFactor();
        // TODO: use UDP instead?
        break;
    }
    packet.packAndSend(&m_tcpSocket);
}
