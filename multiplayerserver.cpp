#include "multiplayerserver.h"

#include <QTcpSocket>
#include "multiplayerpacket.h"

MultiplayerServer::MultiplayerServer(QObject *parent) :
    QTcpServer(parent)
{
}

void MultiplayerServer::incomingConnection(int socketDescriptor)
{
    QTcpSocket *socket = new QTcpSocket(this);
    socket->setSocketDescriptor(socketDescriptor);
    // addPendingConnection(socket); // not needed!

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::incomingConnection(%i): %s:%i", socketDescriptor,
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    connect(socket, SIGNAL(disconnected()), SLOT(client_disconnected()));
    connect(socket, SIGNAL(readyRead()), SLOT(client_readyRead()));
    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(client_error(QAbstractSocket::SocketError)));

    Client *client = new Client;
    m_clients.insert(socket, client);

    MultiplayerPacket(MultiplayerPacket::ConnectionAccepted).send(socket); // Multiplayer::ConnectionRefused

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("ConnectionAccepted");
#endif
}

void MultiplayerServer::client_disconnected()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_disconnected(): %s:%i",
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    Client *client = m_clients.value(socket);
    m_clients.remove(socket);
    delete client;
}

void MultiplayerServer::client_readyRead()
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_readyRead(): %li bytes available", (long)socket->bytesAvailable());
#endif

    Client *client = m_clients.value(socket);
    QDataStream in(socket);
    in.setVersion(QDataStream::Qt_4_8);
    if (client->packetSize == 0) {
        if (socket->bytesAvailable() < (int)sizeof(quint32)) // packet size
            return;
        in >> client->packetSize;
    }
    if (socket->bytesAvailable() < client->packetSize)
        return;
    client->packetSize = 0; // reset packet size

    // read packet type
    qint32 packetType; // MultiplayerPacket::PacketType value
    in >> packetType;

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("PacketType %i (%s) from %s:%i", packetType,
           qPrintable(MultiplayerPacket::typeString((MultiplayerPacket::PacketType)packetType)),
           qPrintable(socket->peerAddress().toString()), socket->peerPort());
#endif

    // TODO: read packet data
    switch ((MultiplayerPacket::PacketType)packetType) {
    default:
        qWarning("MultiplayerServer::client_readyRead(): Illegal PacketType %i", packetType);
    }
}

void MultiplayerServer::client_error(QAbstractSocket::SocketError error)
{
    QTcpSocket *socket = qobject_cast<QTcpSocket*>(sender());
    Q_ASSERT(socket);

#ifdef MULTIPLAYERSERVER_DEBUG
    qDebug("MultiplayerServer::client_error(%i) => %s", error, qPrintable(socket->errorString()));
#endif
}
