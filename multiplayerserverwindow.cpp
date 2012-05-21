#include "multiplayerserverwindow.h"
#include "ui_multiplayerserverwindow.h"

#include "game.h"
#include "qlog.h"

MultiplayerServerWindow::MultiplayerServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultiplayerServerWindow),
    m_address(QHostAddress::Any),
    m_port(0)
{
    ui->setupUi(this);
    m_statusLabel = new QLabel;
    ui->statusbar->addWidget(m_statusLabel);

    qRegisterLogView(ui->logView);

    m_game = new Game(this);
    m_server = new MultiplayerServer(m_game, this);

    m_playerListModel = new PlayerListModel(m_server, this);
    connect(m_game, SIGNAL(playerAdded(Player*)), m_playerListModel, SLOT(addPlayer(Player*)));
    connect(m_game, SIGNAL(playerRemoved(Player*)), m_playerListModel, SLOT(removePlayer(Player*)), Qt::DirectConnection);
    ui->playerListView->setModel(m_playerListModel);

    qLog("Dedicated Multiplayer Server started");
}

MultiplayerServerWindow::~MultiplayerServerWindow()
{
    if (m_server->isListening()) {
        m_server->close();
    }
    delete ui;
}

bool MultiplayerServerWindow::isListening() const
{
    return m_server->isListening();
}

bool MultiplayerServerWindow::startListening(const QHostAddress &address, quint16 port)
{
    if (m_server->isListening())
        return false;
    m_address = address;
    m_port = port;
    bool listening = m_server->listen(address, port);
    if (listening) {
        m_statusLabel->setText(QString("Server is listening on interface %1, port %2")
                               .arg(m_server->serverAddress().toString()).arg(m_server->serverPort()));
        ui->action_toggleListening->setText("&Stop listening");
    } else {
        m_statusLabel->setText(QString("Failed to start the server on interface %1, port %2: %3")
                               .arg(address.toString()).arg(port)
                               .arg(m_server->errorString()));
    }
    qLog(m_statusLabel->text());
    return listening;
}

void MultiplayerServerWindow::stopListening()
{
    m_server->close();
    qLog("Server stopped listening");
    m_statusLabel->setText("Server is not listening");
    ui->action_toggleListening->setText("&Start listening");
}

bool MultiplayerServerWindow::toggleListening()
{
    if (m_server->isListening()) {
        stopListening();
        return false;
    } else {
        return startListening(m_address, m_port);
    }
}

void MultiplayerServerWindow::on_inputLineEdit_returnPressed()
{
    QString command = ui->inputLineEdit->text();
    if (command.isEmpty())
        return;
    // TODO: proccess command
    qLog(command, Qt::blue);
    MultiplayerPacket packet(MultiplayerPacket::Chat);
    packet.stream() << command;
    packet.pack();
    m_server->sendPacketToAllClients(packet);
    ui->inputLineEdit->clear();
}


PlayerListModel::PlayerListModel(MultiplayerServer *server, QObject *parent) :
    QAbstractListModel(parent),
    m_server(server)
{
}

QVariant PlayerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_playerList.count())
        return QVariant();
    switch (role) {
    case Qt::DisplayRole: {
        Player *player = m_playerList.at(index.row());
        return QString("%1 (%2)").arg(player->name()).arg(m_server->playerId(player));
        break;
    }
    case Qt::DecorationRole:
        return m_playerList.at(index.row())->color();
        break;
    }
    return QVariant();
}

int PlayerListModel::addPlayer(Player *player)
{
    int index = m_playerList.count();
    beginInsertRows(QModelIndex(), index, index);
    m_playerList.append(player);
    endInsertRows();
    qLog(QString("Player joined: \"%1\"").arg(player->name()));
    return index;
}

int PlayerListModel::removePlayer(Player *player)
{
    int index = m_playerList.indexOf(player);
    if (index < 0)
        return index;
    beginRemoveRows(QModelIndex(), index, index);
    m_playerList.removeAt(index);
    endRemoveRows();
    qLog(QString("Player left: \"%1\"").arg(player->name()));
    return index;
}
