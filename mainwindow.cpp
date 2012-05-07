#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>

#include "canvas.h"
#include "stringpropertyeditor.h"
#include "vector2dpropertyeditor.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
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

    connect(m_canvas, SIGNAL(modeChanged()), SLOT(canvas_modeChanged()));
    connect(m_canvas, SIGNAL(selectionChanged(QObject*)), SLOT(canvas_selectionChanged(QObject*)));

    m_canvas->setFocus(); // TODO focus on click
    connect(ui->action_setFocus, SIGNAL(triggered()), m_canvas, SLOT(setFocus()));

    m_selectedObject = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_modeComboBox_currentIndexChanged(int index)
{
    m_canvas->setMode((Canvas::Mode)index);
}

void MainWindow::canvas_modeChanged()
{
    if (ui->modeComboBox->currentIndex() == m_canvas->mode())
        return;
    ui->modeComboBox->setCurrentIndex(m_canvas->mode());
}

void MainWindow::canvas_selectionChanged(QObject *o)
{
    const QMetaObject *metaObject = o->metaObject();
    if (m_selectedObject == NULL || typeid(o) != typeid(m_selectedObject)) {
        // type has changed -> create new view
        QLayout *layout = ui->editorWidget->layout();
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
            default:
                editor = new StringPropertyEditor;
            }
            editor->setValue(property.read(o).value<QVariant>());
            layout->addWidget(editor->widget());
            m_propertyEditorList.append(editor);
        }
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
        property.write(m_selectedObject, editor->value());
    }
}
