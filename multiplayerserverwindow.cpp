#include "multiplayerserverwindow.h"
#include "ui_multiplayerserverwindow.h"

#include "game.h"

PlayerListModel::PlayerListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

QVariant PlayerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_playerList.count())
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
        return m_playerList.at(index.row())->name();
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
    return index;
}

MultiplayerServerWindow *g_serverWindowInstance = NULL;
void msgHandler(QtMsgType type, const char *msg)
{
    switch (type) {
    case QtDebugMsg:
        g_serverWindowInstance->ui->console->appendPlainText(msg);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s\n", msg);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s\n", msg);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s\n", msg);
        abort();
    }
}

MultiplayerServerWindow::MultiplayerServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultiplayerServerWindow),
    m_address(QHostAddress::Any),
    m_port(0)
{
    ui->setupUi(this);
    m_statusLabel = new QLabel;
    ui->statusbar->addWidget(m_statusLabel);

    g_serverWindowInstance = this;
    qInstallMsgHandler(msgHandler);

    m_game = new Game(this);
    m_server = new MultiplayerServer(m_game, this);

    m_playerListModel = new PlayerListModel(this);
    connect(m_game, SIGNAL(playerAdded(Player*)), m_playerListModel, SLOT(addPlayer(Player*)));
    connect(m_game, SIGNAL(playerRemoved(Player*)), m_playerListModel, SLOT(removePlayer(Player*)), Qt::DirectConnection);
    ui->playerListView->setModel(m_playerListModel);
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
        m_statusLabel->setText(QString("Server is listening on interface %1, port %2.")
                               .arg(m_server->serverAddress().toString()).arg(m_server->serverPort()));
        ui->action_toggleListening->setText("&Stop listening");
    } else {
        m_statusLabel->setText(QString("Failed to start the server on interface %1, port %2: %3.")
                               .arg(m_server->serverAddress().toString()).arg(m_server->serverPort())
                               .arg(m_server->errorString()));
    }
    return listening;
}

void MultiplayerServerWindow::stopListening()
{
    m_server->close();
    m_statusLabel->setText("Server is not listening.");
    statusBar()->showMessage("Server stopped listening.");
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
    // TODO: handle command
    ui->console->appendPlainText(command);
    ui->inputLineEdit->clear();
}
