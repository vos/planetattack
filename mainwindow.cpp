#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QFileDialog>

#include "canvas.h"
#include "computerplayer.h"
#include "stringpropertyeditor.h"
#include "vector2dpropertyeditor.h"
#include "playerpropertyeditor.h"
#include "playerintelligencepropertyeditor.h"

#include "xmlscenarioserializer.h"
#include "multiplayerserver.h"
#include "multiplayerclient.h"

#include <QDebug>
#include <QScriptEngineDebugger>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_server(NULL),
    m_selectedObject(NULL)
{
    ui->setupUi(this);
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
    connect(m_canvas, SIGNAL(selectionChanged(QObject*)), SLOT(canvas_selectionChanged(QObject*)));

    m_canvas->setFocus();
    connect(ui->action_setFocus, SIGNAL(triggered()), m_canvas, SLOT(setFocus()));

    updatePlayerComboBox();
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_propertyEditorMap.values());
    delete m_canvas;
    delete ui;
}

void MainWindow::game_modeChanged(Game::Mode mode)
{
    ui->modeComboBox->setCurrentIndex(mode);
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

void MainWindow::player_nameChanged(const QString &oldName, const QString &newName)
{
    ui->playerComboBox->setItemText(ui->playerComboBox->findText(oldName), newName);
}

void MainWindow::on_modeComboBox_currentIndexChanged(int index)
{
    m_game->setMode((Game::Mode)index);
}

void MainWindow::updatePlayerComboBox() {
    ui->playerComboBox->clear();
    foreach (Player *player, m_game->players()) {
        QVariant playerVar;
        playerVar.setValue(player);
        ui->playerComboBox->addItem(player->name(), playerVar);
        connect(player, SIGNAL(nameChanged(QString,QString)), SLOT(player_nameChanged(QString,QString)));
    }
    QVariant playerVar;
    playerVar.setValue(m_canvas->activePlayer());
    ui->playerComboBox->setCurrentIndex(ui->playerComboBox->findData(playerVar));
}

void MainWindow::on_playerComboBox_activated(int index)
{
    Player *player = ui->playerComboBox->itemData(index).value<Player*>();
    m_canvas->setActivePlayer(player);
    canvas_selectionChanged(player);
}

void MainWindow::on_removePlayerButton_clicked()
{
    int index = ui->playerComboBox->currentIndex();
    Player *player = ui->playerComboBox->itemData(index).value<Player*>();
    if (m_selectedObject == player) {
        canvas_selectionChanged(NULL);
    }
    if (m_game->removePlayer(player)) {
        ui->playerComboBox->removeItem(index);
    }
}

void MainWindow::on_addPlayerButton_clicked()
{
    Player *player = new ComputerPlayer("Player", Qt::white, NULL, m_game);
    m_game->addPlayer(player);
    m_canvas->setActivePlayer(player);
    QVariant playerVar;
    playerVar.setValue(player);
    ui->playerComboBox->addItem(player->name(), playerVar);
    connect(player, SIGNAL(nameChanged(QString,QString)), SLOT(player_nameChanged(QString,QString)));
    ui->playerComboBox->setCurrentIndex(ui->playerComboBox->count()-1);
    canvas_selectionChanged(player);
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
    statusBar()->showMessage("New Scenario created", 5000);
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
            m_canvas->setActivePlayer(scenario.activePlayer);
            updatePlayerComboBox();
            canvas_selectionChanged(m_canvas->activePlayer());
            QString relativeFilePath = QDir::current().relativeFilePath(fileName);
            updateTitle(relativeFilePath);
            statusBar()->showMessage(QString("Scenario \"%1\" successfully loaded").arg(relativeFilePath), 5000);
            m_scenarioFileName = fileName;
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
        statusBar()->showMessage(QString("Scenario successfully saved as \"%1\"").arg(relativeFilePath), 5000);
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
    m_server = new MultiplayerServer(this);
    if (m_server->listen(QHostAddress::Any, 54321)) {
        qDebug("The server is listening on interface %s, port %i",
               qPrintable(m_server->serverAddress().toString()), m_server->serverPort());
    } else {
        qCritical("Failed to start the server on interface %s, port %i: %s (error code %i)",
                  qPrintable(m_server->serverAddress().toString()), m_server->serverPort(),
                  qPrintable(m_server->errorString()), m_server->serverError());
    }
}

void MainWindow::on_action_ConnectToServer_triggered()
{
    MultiplayerClient *client = new MultiplayerClient(this);
    connect(client, SIGNAL(disconnected()), client, SLOT(deleteLater()));
    client->connectToHost("127.0.0.1", 54321);
}

void MainWindow::updateTitle(const QString &subTitle)
{
    setWindowTitle(QString(m_canvas->windowTitle() + " - %1").arg(subTitle));
}
