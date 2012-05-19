#ifndef MULTIPLAYERSERVER_H
#define MULTIPLAYERSERVER_H

#include <QTcpServer>
#include <QHash>

class MultiplayerServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MultiplayerServer(QObject *parent = NULL);

private slots:
    void client_disconnected();
    void client_readyRead();
    void client_error(QAbstractSocket::SocketError error);

private:
    struct Client {
        explicit inline Client() : packetSize(0) { }
        quint32 packetSize;
    };
    QHash<QTcpSocket*, Client*> m_clients;

    void incomingConnection(int socketDescriptor);

};

#endif // MULTIPLAYERSERVER_H
