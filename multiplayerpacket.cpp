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
    "PlanetAdded",
    "PlanetId",
    "IllegalPacketType"
};

QString MultiplayerPacket::typeString(PacketType type)
{
    return MultiplayerPacket::PacketTypeNames[type];
}

MultiplayerPacket::MultiplayerPacket(PacketType type) :
    m_type(type),
    m_stream(&m_data, QIODevice::WriteOnly)
{
    m_stream.setVersion(QDataStream::Qt_4_8);
    m_stream << (quint32)0;
    m_stream << (qint32)type;
}

bool MultiplayerPacket::send(QTcpSocket *socket)
{
    if (socket == NULL || m_data.length() < int(sizeof(quint32) + sizeof(qint32))) // size + type
        return false;

    m_stream.device()->seek(0);
    m_stream << (quint32)size();

    return socket->write(m_data) == m_data.size();
}
