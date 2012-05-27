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

    m_playerListModel = new ServerPlayerListModel(m_server, this);
    connect(m_game, SIGNAL(playerAdded(Player*)), m_playerListModel, SLOT(addPlayer(Player*)));
    connect(m_game, SIGNAL(playerRemoved(Player*)), m_playerListModel, SLOT(removePlayer(Player*)), Qt::DirectConnection);
    ui->playerListView->setModel(m_playerListModel);

    connect(m_server, SIGNAL(chatMessageReceived(QString,Player*)), SLOT(server_chatMessageReceived(QString,Player*)));

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
        QString msg = QString("Server is listening on interface %1, port %2")
                .arg(m_server->serverAddress().toString()).arg(m_server->serverPort());
        m_statusLabel->setText(msg);
        qLog(msg);
        ui->action_toggleListening->setText("&Stop listening");
    } else {
        QString msg = QString("Failed to start the server on interface %1, port %2: %3")
                .arg(address.toString()).arg(port).arg(m_server->errorString());
        m_statusLabel->setText(msg);
        qCritical(qPrintable(msg));
    }
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
    qLog(command, Qt::magenta);
    m_server->sendChatMessage(command);
    ui->inputLineEdit->clear();
}

void MultiplayerServerWindow::server_chatMessageReceived(const QString &msg, Player *player)
{
    qLog(QString("%1: %2").arg(player->name(), msg), player->color());
}

ServerPlayerListModel::ServerPlayerListModel(MultiplayerServer *server, QObject *parent) :
    PlayerListModel(parent),
    m_server(server)
{
}

QVariant ServerPlayerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_playerList.count())
        return QVariant();
    if (role == Qt::DisplayRole) {
        Player *player = m_playerList.at(index.row());
        return QString("%1 (%2)").arg(player->name()).arg(m_server->playerId(player));
    }
    return PlayerListModel::data(index, role);
}

int ServerPlayerListModel::addPlayer(Player *player)
{
    qLog(QString("Player joined: \"%1\"").arg(player->name()), player->color());
    return PlayerListModel::addPlayer(player);
}

int ServerPlayerListModel::removePlayer(Player *player)
{
    qLog(QString("Player left: \"%1\"").arg(player->name()), player->color());
    return PlayerListModel::removePlayer(player);
}
