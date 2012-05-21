#include "multiplayerclient.h"

#include "game.h"

MultiplayerClient::MultiplayerClient(Game *game, Player *player, QObject *parent) :
    QTcpSocket(parent),
    m_game(game),
    m_player(player),
    m_packetSize(0),
    m_playerId(-1),
    m_nextTempPlanetId(1)
{
    connect(this, SIGNAL(connected()), SLOT(socket_connected()));
    connect(this, SIGNAL(disconnected()), SLOT(socket_disconnected()));
    connect(this, SIGNAL(readyRead()), SLOT(socket_readyRead()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socket_error(QAbstractSocket::SocketError)));

    connect(m_game, SIGNAL(planetAdded(Planet*)), SLOT(game_planetAdded(Planet*)));
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
    in.setVersion(QDataStream::Qt_4_8);
    while (bytesAvailable() > 0) {
        if (m_packetSize == 0) {
            if (bytesAvailable() < (int)sizeof(quint32)) // packet size
                return;
            in >> m_packetSize;
        }
        if (bytesAvailable() < m_packetSize)
            return;
        m_packetSize = 0; // reset packet size

        // read packet type
        qint32 packetType; // MultiplayerPacket::PacketType value
        in >> packetType;

#ifdef MULTIPLAYERCLIENT_DEBUG
        qDebug("PacketType %i (%s)", packetType, qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)));
#endif

        // read and handle packet data
        switch ((MultiplayerPacket::PacketType)packetType) {
        case MultiplayerPacket::ConnectionAccepted: {
            MultiplayerPacket packet(MultiplayerPacket::PlayerConnect);
            packet.stream() << *m_player;
            packet.send(this);
            break;
        }
        case MultiplayerPacket::ConnectionRefused:
            // TODO
            break;
        case MultiplayerPacket::PlayerConnectAccepted: {
            in >> m_playerId;
            m_idPlayerMap.insert(m_playerId, m_player);
            m_playerIdMap.insert(m_player, m_playerId);
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
                m_playerIdMap.insert(player, playerId);
                qDebug("playerId = %d, name = %s", playerId, qPrintable(player->name()));
            }
            break;
        }
        case MultiplayerPacket::PlayerDisconnected: {
            PlayerID playerId;
            in >> playerId;
            Player *player = m_idPlayerMap.take(playerId);
            Q_ASSERT(player);
            qDebug("playerId = %d, name = %s", playerId, qPrintable(player->name()));
            m_playerIdMap.remove(player);
            m_game->removePlayer(player);
            break;
        }
        case MultiplayerPacket::PlanetAdded: {
            PlanetID planetId;
            Planet *planet = new Planet;
            in >> planetId >> *planet;
            if (m_game->addPlanet(planet)) {
                m_idPlanetMap.insert(planetId, planet);
                m_planetIdMap.insert(planet, planetId);
            }
            break;
        }
        case MultiplayerPacket::PlanetId: {
            PlanetID tempPlanetId, planetId;
            in >> tempPlanetId >> planetId;
            Planet *planet = m_tempIdPlanetMap.take(tempPlanetId);
            Q_ASSERT(planet);
            m_idPlanetMap.insert(planetId, planet);
            m_planetIdMap.insert(planet, planetId); // replace temp id
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

void MultiplayerClient::game_planetAdded(Planet *planet)
{
    // only send if the planet has no id (newly created planet)
    if (!m_planetIdMap.contains(planet)) {
        PlanetID tempPlanetId = m_nextTempPlanetId++;
        m_tempIdPlanetMap.insert(tempPlanetId, planet);
        m_planetIdMap.insert(planet, tempPlanetId);
        MultiplayerPacket packet(MultiplayerPacket::PlanetAdded);
        packet.stream() << tempPlanetId << *planet;
        packet.send(this);
    }
}
