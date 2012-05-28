#include "multiplayerpacket.h"

const char* MultiplayerPacket::PacketTypeNames[] = {
    "ConnectionAccepted",
    "ConnectionRefused",
    "UdpRegister",
    "PlayerConnect",
    "PlayerConnectAccepted",
    "PlayerConnectDenied",
    "PlayerConnected",
    "PlayerDisconnect",
    "PlayerDisconnected",
    "PlayerChanged",
    "Chat",
    "ModeChanged",
    "PlanetAdded",
    "PlanetId",
    "PlanetRemoved",
    "PlanetChanged",
    "ResourcesTransferInitiated",
    "IllegalPacketType"
};

MultiplayerPacket::MultiplayerPacket(PacketType type) :
    m_type(type),
    m_stream(&m_data, QIODevice::WriteOnly)
{
    m_stream.setVersion(StreamVersion);
    m_stream << (PacketSize)sizeof(EnumType); // min size (packet type only)
    m_stream << (EnumType)type;
}

MultiplayerPacket::MultiplayerPacket(MultiplayerPacket::PacketType type, const QByteArray &data) :
    m_type(type),
    m_stream(&m_data, QIODevice::WriteOnly)
{
    m_stream.setVersion(StreamVersion);
    m_stream << PacketSize(sizeof(EnumType) + data.size()); // current size (packet type + data)
    m_stream << (EnumType)type;
    m_stream << data;
}

const MultiplayerPacket& MultiplayerPacket::pack()
{
    m_stream.device()->seek(0);
    m_stream << (PacketSize)size();
    return *this;
}

bool MultiplayerPacket::send(QIODevice *device) const
{
    if (device == NULL)
        return false;
    return device->write(m_data) == m_data.size();
}

MultiplayerPacket& MultiplayerPacket::reopen()
{
    m_stream.device()->seek(m_data.size());
    return *this;
}
