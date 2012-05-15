#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>

#include "canvas.h"
#include "computerplayer.h"
#include "stringpropertyeditor.h"
#include "vector2dpropertyeditor.h"
#include "playerpropertyeditor.h"
#include "playerintelligencepropertyeditor.h"

#include "xmlscenarioserializer.h"

#include <QDebug>
#include <QScriptEngineDebugger>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_selectedObject(NULL)
{
    ui->setupUi(this);
    m_canvas = new Canvas(this);
    setCentralWidget(m_canvas);
    setWindowTitle(m_canvas->windowTitle());

    ui->menuBar->addMenu(m_canvas->scriptEngineDebugger()->createStandardMenu());
    addToolBar(Qt::TopToolBarArea, m_canvas->scriptEngineDebugger()->createStandardToolBar());

    ui->modeComboBox->addItem(Canvas::modeString(Canvas::EditorMode));
    ui->modeComboBox->addItem(Canvas::modeString(Canvas::GameMode));
    ui->modeComboBox->setCurrentIndex(m_canvas->mode());

    QLayout *layout = new QVBoxLayout;
    ui->editorWidget->setLayout(layout);

    foreach (Player *player, m_canvas->players()) {
        QVariant playerVar;
        playerVar.setValue(player);
        ui->playerComboBox->addItem(player->name(), playerVar);
        connect(player, SIGNAL(nameChanged(QString,QString)), SLOT(player_nameChanged(QString,QString)));
    }
    ui->playerComboBox->setCurrentIndex(ui->playerComboBox->findText(m_canvas->activePlayer()->name()));

    connect(m_canvas, SIGNAL(modeChanged()), SLOT(canvas_modeChanged()));
    connect(m_canvas, SIGNAL(selectionChanged(QObject*)), SLOT(canvas_selectionChanged(QObject*)));

    m_canvas->setFocus();
    connect(ui->action_setFocus, SIGNAL(triggered()), m_canvas, SLOT(setFocus()));
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_propertyEditorMap.values());
    delete m_canvas;
    delete ui;
}

void MainWindow::canvas_modeChanged()
{
    if (ui->modeComboBox->currentIndex() == m_canvas->mode())
        return;
    ui->modeComboBox->setCurrentIndex(m_canvas->mode());
}

void MainWindow::canvas_selectionChanged(QObject *o)
{
    if (!o) {
        // selection set to NULL -> clear editor
        QVBoxLayout *layout = (QVBoxLayout*)ui->editorWidget->layout();
        // delete all widgets
        m_propertyEditorMap.clear();
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
        }
        m_selectedObject = NULL;
        return;
    }

    const QMetaObject *metaObject = o->metaObject();
    if (m_selectedObject == NULL || m_selectedObject->metaObject() != metaObject) {
        // type has changed -> create new view
        QVBoxLayout *layout = (QVBoxLayout*)ui->editorWidget->layout();
        // delete all widgets
        m_propertyEditorMap.clear();
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
        }
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
    m_canvas->setMode((Canvas::Mode)index);
}

void MainWindow::on_playerComboBox_activated(int index)
{
    Player *player = ui->playerComboBox->itemData(index).value<Player*>();
    m_canvas->setActivePlayer(player);
    canvas_selectionChanged(player);
}

void MainWindow::on_addPlayerButton_clicked()
{
    Player *player = new ComputerPlayer("Player", Qt::white, NULL, m_canvas);
    m_canvas->players().insert(player);
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

void MainWindow::on_action_openScenario_triggered()
{
    QString fileName("scenario0.xml");
    XmlScenarioSerializer serializer;
    ScenarioSerializer::Scenario scenario;
    if (serializer.deserialize(fileName, scenario)) {
        // TODO set active scenario to canvas
//        qDeleteAll(m_canvas->players());
//        qDeleteAll(m_canvas->planets());
        statusBar()->showMessage(QString("Scenario \"%1\" successfully loaded").arg(fileName));
    } else {
        QMessageBox::warning(this, "Open Scenario Error", QString("Cannot open scenario file \"%1\"").arg(fileName));
    }
}

void MainWindow::on_action_saveScenario_triggered()
{
    QString fileName("scenario0.xml");
    XmlScenarioSerializer serializer;
    ScenarioSerializer::Scenario scenario = {
        m_canvas->players(),
        m_canvas->activePlayer(),
        m_canvas->planets()
    };
    if (serializer.serialize(scenario, fileName)) {
        statusBar()->showMessage(QString("Scenario successfully saved as \"%1\"").arg(fileName));
    } else {
        QMessageBox::warning(this, "Save Scenario Error", QString("Cannot save scenario file \"%1\"").arg(fileName));
    }
}
