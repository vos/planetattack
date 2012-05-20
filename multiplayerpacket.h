#ifndef MULTIPLAYERPACKET_H
#define MULTIPLAYERPACKET_H

#include <QByteArray>
#include <QDataStream>

QT_FORWARD_DECLARE_CLASS(QTcpSocket)

class MultiplayerPacket
{
public:
    enum PacketType {
        ConnectionAccepted, // server -> client
        ConnectionRefused, // server -> client
        PlayerConnect, // client -> server
        PlayerConnectAccepted, // server -> client
        PlayerConnectDenied, // server -> client
        PlayerConnected, // server -> other clients
        PlayerDisconnect, // client -> server
        PlayerDisconnected, // server -> other clients
        PlanetAdded, // client -> server -> other clients
        PlanetId, // server -> client
        IllegalPacketType
    };
    static QString typeString(PacketType type);

    MultiplayerPacket(PacketType type);

    inline PacketType type() const { return m_type; }
    inline QString typeString() const { return MultiplayerPacket::PacketTypeNames[m_type]; }
    inline QByteArray& data() { return m_data; }
    inline QDataStream& stream() { return m_stream; }
    inline int size() const { return m_data.size() - (int)sizeof(quint32); }
    bool send(QTcpSocket *socket);

private:
    static const char *PacketTypeNames[];

    PacketType m_type;
    QByteArray m_data;
    QDataStream m_stream;

};

#endif // MULTIPLAYERPACKET_H
