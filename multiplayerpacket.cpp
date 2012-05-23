#include "multiplayerpacket.h"

#include <QTcpSocket>

const char* MultiplayerPacket::PacketTypeNames[] = {
    "ConnectionAccepted",
    "ConnectionRefused",
    "PlayerConnect",
    "PlayerConnectAccepted",
    "PlayerConnectDenied",
    "PlayerConnected",
    "PlayerDisconnect",
    "PlayerDisconnected",
    "Chat",
    "PlanetAdded",
    "PlanetId",
    "PlanetRemoved",
    "IllegalPacketType"
};

MultiplayerPacket::MultiplayerPacket(PacketType type) :
    m_type(type),
    m_stream(&m_data, QIODevice::WriteOnly)
{
    m_stream.setVersion(StreamVersion);
    m_stream << (PacketSize)sizeof(qint32); // min size (packet type only)
    m_stream << (qint32)type;
}

MultiplayerPacket::MultiplayerPacket(MultiplayerPacket::PacketType type, const QByteArray &data) :
    m_type(type),
    m_stream(&m_data, QIODevice::WriteOnly)
{
    m_stream.setVersion(StreamVersion);
    m_stream << PacketSize(sizeof(qint32) + data.size()); // current size (packet type + data)
    m_stream << (qint32)type;
    m_stream << data;
}

const MultiplayerPacket& MultiplayerPacket::pack()
{
    m_stream.device()->seek(0);
    m_stream << (PacketSize)size();
    return *this;
}

bool MultiplayerPacket::send(QTcpSocket *socket) const
{
    if (socket == NULL)
        return false;
    return socket->write(m_data) == m_data.size();
}

MultiplayerPacket& MultiplayerPacket::reopen()
{
    m_stream.device()->seek(m_data.size());
    return *this;
}
