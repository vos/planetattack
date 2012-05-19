#ifndef MULTIPLAYERCLIENT_H
#define MULTIPLAYERCLIENT_H

#include <QTcpSocket>

class MultiplayerClient : public QTcpSocket
{
    Q_OBJECT

public:
    explicit MultiplayerClient(QObject *parent = NULL);

private slots:
    void socket_connected();
    void socket_disconnected();
    void socket_readyRead();
    void socket_error(QAbstractSocket::SocketError error);

private:
    quint32 m_packetSize;

};

#endif // MULTIPLAYERCLIENT_H
