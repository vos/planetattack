#ifndef MULTIPLAYERPACKET_H
#define MULTIPLAYERPACKET_H

#include <QByteArray>
#include <QDataStream>

QT_FORWARD_DECLARE_CLASS(QTcpSocket)

typedef quint32 PacketSize;

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
        Chat, // client -> server -> other clients, server -> all clients
        PlanetAdded, // client -> server -> other clients
        PlanetId, // server -> client
        IllegalPacketType
    };
    inline static QString typeString(PacketType type) { return MultiplayerPacket::PacketTypeNames[type]; }
    static const int StreamVersion = QDataStream::Qt_4_8;

    MultiplayerPacket(PacketType type);
    MultiplayerPacket(PacketType type, const QByteArray &data);

    inline PacketType type() const { return m_type; }
    inline QString typeString() const { return MultiplayerPacket::PacketTypeNames[m_type]; }
    inline QByteArray& data() { return m_data; }
    inline QDataStream& stream() { return m_stream; }
    inline int size() const { return m_data.size() - (int)sizeof(PacketSize); }

    const MultiplayerPacket& pack(); // NOTE: don't change the data after pack (or use reopen)
    bool send(QTcpSocket *socket) const; // NOTE: pack before send (packets without extra data can be send directly)!
    inline bool packAndSend(QTcpSocket *socket) { return pack().send(socket); }
    MultiplayerPacket& reopen();

private:
    static const char *PacketTypeNames[];

    PacketType m_type;
    QByteArray m_data;
    QDataStream m_stream;

};

#endif // MULTIPLAYERPACKET_H
