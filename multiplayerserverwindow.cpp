#include "multiplayerserverwindow.h"
#include "ui_multiplayerserverwindow.h"

#include "game.h"

MultiplayerServerWindow::MultiplayerServerWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MultiplayerServerWindow),
    m_address(QHostAddress::Any),
    m_port(0)
{
    ui->setupUi(this);
    m_statusLabel = new QLabel;
    ui->statusbar->addWidget(m_statusLabel);
    m_game = new Game(this);
    m_server = new MultiplayerServer(m_game, this);
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
