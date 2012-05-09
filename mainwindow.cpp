#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "canvas.h"
#include "computerplayer.h"
#include "stringpropertyeditor.h"
#include "vector2dpropertyeditor.h"
#include "playerintelligencepropertyeditor.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_selectedObject(NULL)
{
    ui->setupUi(this);
    m_canvas = new Canvas(this);
    setCentralWidget(m_canvas);
    setWindowTitle(m_canvas->windowTitle());

    ui->modeComboBox->addItem(Canvas::modeString(Canvas::EditorMode));
    ui->modeComboBox->addItem(Canvas::modeString(Canvas::GameMode));
    ui->modeComboBox->setCurrentIndex(m_canvas->mode());

    QLayout *layout = new QVBoxLayout;
    ui->editorWidget->setLayout(layout);

    foreach (Player *player, m_canvas->players()) {
        ui->playerComboBox->addItem(player->name());
        connect(player, SIGNAL(nameChanged(QString,QString)), SLOT(player_nameChanged(QString,QString)));
    }
    ui->playerComboBox->setCurrentIndex(ui->playerComboBox->findText(m_canvas->activePlayer()->name()));

    connect(m_canvas, SIGNAL(modeChanged()), SLOT(canvas_modeChanged()));
    connect(m_canvas, SIGNAL(selectionChanged(QObject*)), SLOT(canvas_selectionChanged(QObject*)));

    m_canvas->setFocus(); // TODO focus on click
    connect(ui->action_setFocus, SIGNAL(triggered()), m_canvas, SLOT(setFocus()));
}

MainWindow::~MainWindow()
{
    qDeleteAll(m_propertyEditorList);
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
        m_propertyEditorList.clear();
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
        m_propertyEditorList.clear();
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
        }
        layout->addWidget(new QLabel(metaObject->className()));
        for(int i = 1; i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            layout->addWidget(new QLabel(QString("%1 (%2)").arg(property.name(), property.typeName())));
            PropertyEditor *editor;
            switch (property.type()) {
            case QVariant::Vector2D:
                editor = new Vector2DPropertyEditor;
                break;
            case QVariant::UserType:
                qDebug() << "UserType" << property.typeName();
                if (strcmp(property.typeName(), "PlayerIntelligence*") == 0) {
                    editor = new PlayerIntelligencePropertyEditor;
                }
                break;
            default:
                editor = new StringPropertyEditor;
            }
            editor->setValue(property.read(o).value<QVariant>());
            layout->addWidget(editor->widget());
            m_propertyEditorList.append(editor);
        }
        layout->addStretch();
        QPushButton *saveButton = new QPushButton("Save");
        connect(saveButton, SIGNAL(clicked()), SLOT(saveButton_clicked()));
        layout->addWidget(saveButton);
    } else {
        // type has not changed -> just update the view
        for(int i = 1; i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            PropertyEditor *editor = m_propertyEditorList.at(i - 1);
            editor->setValue(property.read(o).value<QVariant>());
        }
    }
    m_selectedObject = o;
}

void MainWindow::saveButton_clicked()
{
    if (!m_selectedObject) return;

    const QMetaObject *metaObject = m_selectedObject->metaObject();
    for(int i = 1; i < metaObject->propertyCount(); ++i) {
        QMetaProperty property = metaObject->property(i);
        PropertyEditor *editor = m_propertyEditorList.at(i - 1);
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
    Q_UNUSED(index)
    QString playerName = ui->playerComboBox->currentText();
    Player *player;
    foreach (Player *p, m_canvas->players()) {
        if (p->name() == playerName) {
            player = p;
            break;
        }
    }
    m_canvas->setActivePlayer(player);
    canvas_selectionChanged(player);
}

void MainWindow::on_addPlayerButton_clicked()
{
    Player *player = new ComputerPlayer("Player", Qt::white, NULL, m_canvas);
    m_canvas->players().insert(player);
    m_canvas->setActivePlayer(player);
    ui->playerComboBox->addItem(player->name());
    connect(player, SIGNAL(nameChanged(QString,QString)), SLOT(player_nameChanged(QString,QString)));
    ui->playerComboBox->setCurrentIndex(ui->playerComboBox->count()-1);
    canvas_selectionChanged(player);
}
