#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>
#include <QInputDialog>

#include "canvas.h"
#include "computerplayer.h"
#include "playerlistmodel.h"
#include "stringpropertyeditor.h"
#include "vector2dpropertyeditor.h"
#include "playerpropertyeditor.h"
#include "playerintelligencepropertyeditor.h"

#include "xmlscenarioserializer.h"
#include "multiplayerserver.h"
#include "multiplayerclient.h"

#include "qlog.h"

#include <QDebug>
#include <QScriptEngineDebugger>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_server(NULL),
    m_client(NULL),
    m_selectedObject(NULL)
{
    ui->setupUi(this);
    qRegisterLogView(ui->logView);

    m_canvas = new Canvas(this);
    m_game = m_canvas->game();
    setCentralWidget(m_canvas);
    setWindowTitle(m_canvas->windowTitle());

    ui->menuBar->addMenu(m_game->scriptEngineDebugger()->createStandardMenu());
    addToolBar(Qt::TopToolBarArea, m_game->scriptEngineDebugger()->createStandardToolBar());

    ui->modeComboBox->addItem(Game::modeString(Game::EditorMode));
    ui->modeComboBox->addItem(Game::modeString(Game::GameMode));
    ui->modeComboBox->setCurrentIndex(m_game->mode());

    QLayout *layout = new QVBoxLayout;
    ui->editorWidget->setLayout(layout);

    connect(m_game, SIGNAL(modeChanged(Game::Mode)), SLOT(game_modeChanged(Game::Mode)));
    connect(m_game, SIGNAL(playerAdded(Player*)), SLOT(game_playerAdded(Player*)));
    connect(m_game, SIGNAL(playerRemoved(Player*)), SLOT(game_playerRemoved(Player*)), Qt::DirectConnection);

    m_playerListModel = new PlayerListModel(this);
    ui->playerComboBox->setModel(m_playerListModel);

    connect(m_canvas, SIGNAL(selectionChanged(QObject*)), SLOT(canvas_selectionChanged(QObject*)));

    m_canvas->setFocus();
    connect(ui->action_setFocus, SIGNAL(triggered()), m_canvas, SLOT(setFocus()));

    // init player list
    foreach (Player *player, m_game->players()) {
        game_playerAdded(player);
    }

    qLog("PlanetAttack started");
}

MainWindow::~MainWindow()
{
    if (m_client != NULL)
        delete m_client;
    if (m_server != NULL)
        delete m_server;
    qDeleteAll(m_propertyEditorMap.values());
    delete m_canvas;
    delete ui;
}

void MainWindow::game_modeChanged(Game::Mode mode)
{
    ui->modeComboBox->setCurrentIndex(mode);
}

void MainWindow::game_playerAdded(Player *player)
{
    m_playerListModel->addPlayer(player);
    if (m_client == NULL) {
        m_canvas->setActivePlayer(player);
        ui->playerComboBox->setCurrentIndex(ui->playerComboBox->count()-1);
        canvas_selectionChanged(player);
    }
}

void MainWindow::game_playerRemoved(Player *player)
{
    m_playerListModel->removePlayer(player);
    if (m_selectedObject == player) {
        canvas_selectionChanged(NULL);
    }
}

void MainWindow::clearPropertyEditor()
{
    QVBoxLayout *layout = (QVBoxLayout*)ui->editorWidget->layout();
    // delete all widgets
    QLayoutItem *item;
    while ((item = layout->takeAt(0)) != NULL) {
        delete item->widget();
        delete item;
    }
    // FIXME: qDeleteAll(m_propertyEditorMap.values()); // crash!?
    m_propertyEditorMap.clear();
}

void MainWindow::canvas_selectionChanged(QObject *o)
{
    if (!o) {
        // selection set to NULL -> clear editor
        clearPropertyEditor();
        m_selectedObject = NULL;
        return;
    }

    const QMetaObject *metaObject = o->metaObject();
    if (m_selectedObject == NULL || m_selectedObject->metaObject() != metaObject) {
        // type has changed -> create new view
        clearPropertyEditor();
        QVBoxLayout *layout = (QVBoxLayout*)ui->editorWidget->layout();
        layout->addWidget(new QLabel(metaObject->className()));
        for (int i = 1; i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            if (!property.isDesignable())
                continue;
            const char *typeName = property.typeName();
            layout->addWidget(new QLabel(QString("%1 (%2)").arg(property.name(), typeName)));
            PropertyEditor *editor;
            switch (property.type()) {
            case QVariant::Vector2D:
                editor = new Vector2DPropertyEditor;
                break;
            case QVariant::UserType:
                if (strcmp(typeName, "Player*") == 0) {
                    editor = new PlayerPropertyEditor;
                }
                else if (strcmp(typeName, "PlayerIntelligence*") == 0) {
                    editor = new PlayerIntelligencePropertyEditor;
                } else {
                    qWarning("no property edtor for type %s", typeName);
                }
                break;
            default:
                editor = new StringPropertyEditor;
            }
            editor->setValue(property.read(o).value<QVariant>());
            QWidget *editorWidget = editor->widget();
            if (!property.isWritable())
                editorWidget->setEnabled(false);
            layout->addWidget(editorWidget);
            m_propertyEditorMap.insert(property.name(), editor);
        }
        layout->addStretch();
        QPushButton *saveButton = new QPushButton("Save");
        connect(saveButton, SIGNAL(clicked()), SLOT(saveButton_clicked()));
        layout->addWidget(saveButton);
    } else {
        // type has not changed -> just update the view
        for (int i = 1; i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            if (!property.isDesignable())
                continue;
            PropertyEditor *editor = m_propertyEditorMap.value(property.name());
            editor->setValue(property.read(o).value<QVariant>());
        }
    }
    m_selectedObject = o;
}

void MainWindow::saveButton_clicked()
{
    if (!m_selectedObject) return;

    const QMetaObject *metaObject = m_selectedObject->metaObject();
    for (int i = 1; i < metaObject->propertyCount(); ++i) {
        QMetaProperty property = metaObject->property(i);
        if (!property.isDesignable())
            continue;
        PropertyEditor *editor = m_propertyEditorMap.value(property.name());
        if (!property.write(m_selectedObject, editor->value())) {
            qWarning("property write failed: %s::%s with value=%s", metaObject->className(), property.name(),
                     qPrintable(editor->value().toString()));
        }
    }
}

void MainWindow::on_modeComboBox_currentIndexChanged(int index)
{
    m_game->setMode((Game::Mode)index);
}

void MainWindow::on_playerComboBox_activated(int index)
{
    Player *player = m_playerListModel->playerAt(index);
    m_canvas->setActivePlayer(player);
    canvas_selectionChanged(player);
}

void MainWindow::on_removePlayerButton_clicked()
{
    if (m_game->playerCount() <= 1) {
        qDebug("the last player cannot be removed");
        return;
    }
    int index = ui->playerComboBox->currentIndex();
    Player *player = m_playerListModel->takePlayerAt(index);
    m_game->removePlayer(player);
}

void MainWindow::on_addPlayerButton_clicked()
{
    Player *player = new ComputerPlayer("Player");
    m_game->addPlayer(player);
}

void MainWindow::on_globalAccessCheckBox_toggled(bool checked)
{
    m_canvas->setGlobalAccess(checked);
}

void MainWindow::on_action_newScenario_triggered()
{
    foreach (Planet *planet, m_game->planets()) {
        if ((QObject*)planet == m_selectedObject)
            canvas_selectionChanged(NULL);
        m_game->removePlanet(planet);
    }
    m_scenarioFileName.clear();
    updateTitle("<unsaved scenario>");
    QString msg("New Scenario created");
    statusBar()->showMessage(msg, 5000);
    qLog(msg);
}

void MainWindow::on_action_openScenario_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Scenario File",
                                                    QDir::current().filePath("scenarios"),
                                                    "Scenario Files (*.xml);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        XmlScenarioSerializer serializer;
        ScenarioSerializer::Scenario scenario;
        if (serializer.deserialize(fileName, scenario)) {
            canvas_selectionChanged(NULL);
            m_game->clearPlayers();
            m_game->clearPlanets();
            foreach (Player *player, scenario.players) {
                m_game->addPlayer(player);
            }
            foreach (Planet *planet, scenario.planets) {
                m_game->addPlanet(planet);
            }
            m_scenarioFileName = fileName;
            m_canvas->setActivePlayer(scenario.activePlayer);
            // select active player
            ui->playerComboBox->setCurrentIndex(m_playerListModel->indexOf(m_canvas->activePlayer()));
            canvas_selectionChanged(m_canvas->activePlayer());
            QString relativeFilePath = QDir::current().relativeFilePath(fileName);
            updateTitle(relativeFilePath);
            QString msg = QString("Scenario \"%1\" successfully loaded").arg(relativeFilePath);
            statusBar()->showMessage(msg, 5000);
            qLog(msg);
        } else {
            QMessageBox::warning(this, "Open Scenario Error", QString("Cannot open scenario file \"%1\".").arg(fileName));
        }
    }
}

void MainWindow::on_action_saveScenario_triggered()
{
    if (m_scenarioFileName.isEmpty()) {
        on_action_saveScenarioAs_triggered();
        return;
    }
    XmlScenarioSerializer serializer;
    ScenarioSerializer::Scenario scenario = {
        m_game->players(),
        m_canvas->activePlayer(),
        m_game->planets()
    };
    if (serializer.serialize(scenario, m_scenarioFileName)) {
        QString relativeFilePath = QDir::current().relativeFilePath(m_scenarioFileName);
        updateTitle(relativeFilePath);
        QString msg = QString("Scenario successfully saved as \"%1\"").arg(relativeFilePath);
        statusBar()->showMessage(msg, 5000);
        qLog(msg);
    } else {
        QMessageBox::warning(this, "Save Scenario Error", QString("Cannot save scenario file \"%1\".").arg(m_scenarioFileName));
    }
}

void MainWindow::on_action_saveScenarioAs_triggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Save Scenario",
                                                    QDir::current().filePath("scenarios"),
                                                    "Scenario Files (*.xml)");
    if (fileName.isEmpty())
        return;
    m_scenarioFileName = fileName;
    on_action_saveScenario_triggered();
}

void MainWindow::on_action_createServer_triggered()
{
    if (m_server == NULL) {
        m_server = new MultiplayerServer(m_game, this);
        if (m_server->listen(QHostAddress::Any, MultiplayerServer::DEFAULT_PORT)) {
            QString msg = QString("The server is listening on interface %1, port %2")
                    .arg(m_server->serverAddress().toString()).arg(m_server->serverPort());
            statusBar()->showMessage(msg);
            qLog(msg);
            ui->action_createServer->setText("Close Server");
        } else {
            qCritical("Failed to start the server on interface %s, port %i: %s (error code %i)",
                      qPrintable(m_server->serverAddress().toString()), m_server->serverPort(),
                      qPrintable(m_server->errorString()), m_server->serverError());
        }
    } else {
        if (m_server->isListening()) {
            m_server->close();
            delete m_server;
            QString msg("Multiplayer server closed");
            statusBar()->showMessage(msg, 5000);
            qLog(msg);
        }
        m_server = NULL;
        ui->action_createServer->setText("Create Server");
    }
}

void MainWindow::on_action_connectToServer_triggered()
{
    if (m_client == NULL) {
        bool ok;
        QString server = QInputDialog::getText(this, "Connect to Server", "Host Address", QLineEdit::Normal, "127.0.0.1", &ok);
        if (ok) {
            QStringList serverSplit = server.split(':');
            QString host = serverSplit.at(0);
            quint16 port = serverSplit.length() < 2 ? MultiplayerServer::DEFAULT_PORT : serverSplit.at(1).toUShort();
            m_client = new MultiplayerClient(m_game, m_canvas->activePlayer(), this);
            connect(m_client, SIGNAL(connected()), SLOT(client_connected()));
            connect(m_client, SIGNAL(disconnected()), SLOT(client_disconnected()));
            connect(m_client, SIGNAL(error(QAbstractSocket::SocketError)), SLOT(client_error(QAbstractSocket::SocketError)));
            connect(m_client, SIGNAL(chatMessageReceived(QString,Player*)), SLOT(client_chatMessageReceived(QString,Player*)));
            ui->action_connectToServer->setText("Connecting...");
            ui->action_connectToServer->setEnabled(false);
            qLog(QString("Connecting to Server %1:%2...").arg(host).arg(port));
            m_client->connectToHost(host, port);
        }
    } else {
        if (m_client->isOpen()) {
            ui->action_connectToServer->setText("Disconnecting...");
            ui->action_connectToServer->setEnabled(false);
            m_client->disconnectFromHost(); // TODO: send proper packet
        }
    }
}

void MainWindow::client_connected()
{
    qLog(QString("Connected to server %1:%2").arg(m_client->peerName()).arg(m_client->peerPort()));
    ui->action_connectToServer->setText("Disconnect from Server");
    ui->action_connectToServer->setEnabled(true);
}

void MainWindow::client_disconnected()
{
    qLog("Disconnected from server");
    m_client->deleteLater();
    m_client = NULL;
    ui->action_connectToServer->setText("Connect to Server");
    ui->action_connectToServer->setEnabled(true);
}

void MainWindow::client_error(QAbstractSocket::SocketError error)
{
    Q_UNUSED(error)
    QString errorString = m_client->errorString();
    QMessageBox::warning(this, "Network Error", errorString);
    qWarning("Network Error: %s", qPrintable(errorString));
    m_client = NULL;
    ui->action_connectToServer->setText("Connect to Server");
    ui->action_connectToServer->setEnabled(true);
}

void MainWindow::on_inputLineEdit_returnPressed()
{
    QString command = ui->inputLineEdit->text();
    if (command.isEmpty())
        return;
    // TODO: proccess command
    if (m_client != NULL) {
        qLog(QString("You: %1").arg(command), m_canvas->activePlayer()->color());
        MultiplayerPacket packet(MultiplayerPacket::Chat);
        packet.stream() << command;
        packet.packAndSend(m_client);
    }
    ui->inputLineEdit->clear();
}

void MainWindow::client_chatMessageReceived(const QString &msg, Player *player)
{
    QString name = player != NULL ? player->name() : "SERVER";
    QColor color = player != NULL ? player->color() : Qt::magenta;
    qLog(QString("%1: %2").arg(name, msg), color);
}

void MainWindow::updateTitle(const QString &subTitle)
{
    setWindowTitle(QString(m_canvas->windowTitle() + " - %1").arg(subTitle));
}
