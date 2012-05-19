#include "multiplayerclient.h"

#include "multiplayerpacket.h"

MultiplayerClient::MultiplayerClient(QObject *parent) :
    QTcpSocket(parent),
    m_packetSize(0)
{
    connect(this, SIGNAL(connected()), SLOT(socket_connected()));
    connect(this, SIGNAL(disconnected()), SLOT(socket_disconnected()));
    connect(this, SIGNAL(readyRead()), SLOT(socket_readyRead()));
    connect(this, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(socket_error(QAbstractSocket::SocketError)));
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

#ifdef DEBUG
    qDebug("PacketType = %i (%s)", packetType, qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)));
#endif

    // TODO: read packet data
    switch ((MultiplayerPacket::PacketType)packetType) {
    case MultiplayerPacket::ConnectionAccepted:
        // TODO
        disconnectFromHost();
        break;
    case MultiplayerPacket::ConnectionRefused:
        // TODO
        break;
    default:
        qWarning("MultiplayerClient::socket_readyRead(): Illegal PacketType %i", packetType);
    }
}

void MultiplayerClient::socket_error(QAbstractSocket::SocketError error)
{
#ifdef MULTIPLAYERCLIENT_DEBUG
    qDebug("MultiplayerClient::socket_error(%i) => %s", error, qPrintable(errorString()));
#endif
}
