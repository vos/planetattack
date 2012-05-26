#include "multiplayerclient.h"

MultiplayerClient::MultiplayerClient(Game *game, Player *player, QObject *parent) :
    QTcpSocket(parent),
    m_game(game),
    m_player(player),
    m_packetSize(0),
    m_playerId(0),
    m_nextTempPlanetId(1)
{
    connect(this, SIGNAL(connected()), SLOT(socket_connected()));
    connect(this, SIGNAL(disconnected()), SLOT(socket_disconnected()));
    connect(this, SIGNAL(readyRead()), SLOT(socket_readyRead()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socket_error(QAbstractSocket::SocketError)));

    connect(m_game, SIGNAL(modeChanged(Game::Mode)), SLOT(game_modeChanged(Game::Mode)));
    connect(m_game, SIGNAL(planetAdded(Planet*)), SLOT(game_planetAdded(Planet*)));
    connect(m_game, SIGNAL(planetRemoved(Planet*)), SLOT(game_planetRemoved(Planet*)), Qt::DirectConnection);
    connect(m_game, SIGNAL(planetChanged(Planet*,Planet::ChangeType)), SLOT(game_planetChanged(Planet*,Planet::ChangeType)));
}

void MultiplayerClient::socket_connected()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_connected()");
#endif
}

void MultiplayerClient::socket_disconnected()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_disconnected()");
#endif
}

void MultiplayerClient::socket_readyRead()
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_readyRead(): %li bytes available", (long)bytesAvailable());
#endif

    QDataStream in(this);
    in.setVersion(MultiplayerPacket::StreamVersion);
    while (bytesAvailable() > 0) {
        if (m_packetSize == 0) {
            if (bytesAvailable() < (int)sizeof(PacketSize))
                return;
            in >> m_packetSize;
        }
        if (bytesAvailable() < m_packetSize)
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
            packet.stream() << *m_player;
            packet.packAndSend(this);
            break;
        }
        case MultiplayerPacket::ConnectionRefused:
            // TODO
            break;
        case MultiplayerPacket::PlayerConnectAccepted: {
            in >> m_playerId;
            m_idPlayerMap.insert(m_playerId, m_player);
            qDebug("playerId = %d", m_playerId);
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
        case MultiplayerPacket::PlanetChanged: {
            PlanetID planetId;
            EnumType changeType; // Planet::ChangeType
            in >> planetId >> changeType;
            Planet *planet = m_idPlanetMap.value(planetId);
            Q_ASSERT(planet);
            switch ((Planet::ChangeType)changeType) {
            case Planet::PositionChange: {
//                QVector2D position;
                in >> planet->m_position;
//                planet->setPosition(position);
                break;
            }
            case Planet::RadiusChange:
                in >> planet->m_radius;
                break;
            default:
                qWarning("invalid Planet::ChangeType: %d", changeType);
            }
            break;
        }
        default:
            qWarning("MultiplayerClient::socket_readyRead(): Illegal PacketType %i", packetType);
            return;
        }
    }
}

void MultiplayerClient::socket_error(QAbstractSocket::SocketError error)
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_error(%i) => %s", error, qPrintable(errorString()));
#endif
}

void MultiplayerClient::game_modeChanged(Game::Mode mode)
{
    MultiplayerPacket packet(MultiplayerPacket::ModeChanged);
    packet.stream() << (EnumType)mode;
    packet.packAndSend(this);
}

void MultiplayerClient::game_planetAdded(Planet *planet)
{
    // only send if the planet has no id (newly created planet)
    if (!m_idPlanetMap.containsValue(planet)) {
        PlanetID tempPlanetId = m_nextTempPlanetId++;
        m_tempIdPlanetMap.insert(tempPlanetId, planet);
        MultiplayerPacket packet(MultiplayerPacket::PlanetAdded);
        packet.stream() << tempPlanetId << *planet;
        packet.packAndSend(this);
    }
}

void MultiplayerClient::game_planetRemoved(Planet *planet)
{
    // only send if the planet is not yet removed (locally removed)
    if (m_idPlanetMap.containsValue(planet)) {
        PlanetID planetId = m_idPlanetMap.takeKey(planet);
        Q_ASSERT(planetId > 0);
        MultiplayerPacket packet(MultiplayerPacket::PlanetRemoved);
        packet.stream() << planetId;
        packet.packAndSend(this);
    }
}

void MultiplayerClient::game_planetChanged(Planet *planet, Planet::ChangeType changeType)
{
    if (changeType != Planet::PositionChange && changeType != Planet::RadiusChange)
        return; // don't send other change types yet

    if (m_idPlanetMap.containsValue(planet)) {
        PlanetID planetId = m_idPlanetMap.key(planet);
        Q_ASSERT(planetId > 0);
        MultiplayerPacket packet(MultiplayerPacket::PlanetChanged);
        packet.stream() << planetId << (EnumType)changeType;
        switch (changeType) {
        case Planet::PositionChange:
            packet.stream() << planet->position();
            break;
        case Planet::RadiusChange:
            packet.stream() << planet->radius();
            break;
        }
        packet.packAndSend(this);
    }
}
