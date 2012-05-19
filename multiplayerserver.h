#ifndef MULTIPLAYERSERVER_H
#define MULTIPLAYERSERVER_H

#include <QTcpServer>
#include <QHash>

class Game;
class Player;

class MultiplayerServer : public QTcpServer
{
    Q_OBJECT

public:
    explicit MultiplayerServer(Game *game, QObject *parent = NULL);
    ~MultiplayerServer();

private slots:
    void client_disconnected();
    void client_readyRead();
    void client_error(QAbstractSocket::SocketError error);

private:
    Game *m_game;

    struct Client {
        explicit inline Client(Player *p = NULL) : player(p), packetSize(0) { }
        Player *player;
        quint32 packetSize;
    };
    QHash<QTcpSocket*, Client*> m_clients;

    void incomingConnection(int socketDescriptor);

};

#endif // MULTIPLAYERSERVER_H
