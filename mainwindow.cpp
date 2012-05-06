#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QMetaObject>
#include <QMetaProperty>

#include <QVBoxLayout>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>

#include "canvas.h"

#include <QDebug>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_canvas = new Canvas(this);
    setCentralWidget(m_canvas);
    setWindowTitle(m_canvas->windowTitle());
    QLayout *layout = new QVBoxLayout;
    ui->editorWidget->setLayout(layout);
    connect(m_canvas, SIGNAL(selectionChanged(QObject*)), SLOT(canvas_selectionChanged(QObject*)));
    m_canvas->setFocus(); // TODO focus on click

    m_selectedObject = NULL;
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::canvas_selectionChanged(QObject *o)
{
    const QMetaObject *metaObject = o->metaObject();
    if (o != m_selectedObject) {
        QLayout *layout = ui->editorWidget->layout();
        // delete all widgets
        m_objectEditorList.clear();
        QLayoutItem *item;
        while ((item = layout->takeAt(0)) != NULL) {
            delete item->widget();
            delete item;
        }
        layout->addWidget(new QLabel(metaObject->className()));
        for(int i = 1; i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            layout->addWidget(new QLabel(QString("%1 (%2)").arg(property.name(), property.typeName())));
            QString value;
            if (property.type() == QVariant::Vector2D) {
                QVector2D vec = property.read(o).value<QVector2D>();
                value = QString("%1, %2").arg(vec.x()).arg(vec.y());
            } else {
                value = property.read(o).toString();
            }
            QWidget *editorWidget = new QTextEdit(value);
            layout->addWidget(editorWidget);
            m_objectEditorList.append(editorWidget);
        }
        QPushButton *saveButton = new QPushButton("Save");
        saveButton->setEnabled(false);
        layout->addWidget(saveButton);
        m_selectedObject = o;
    } else {
        for(int i = 1; i < metaObject->propertyCount(); ++i) {
            QMetaProperty property = metaObject->property(i);
            QString value;
            if (property.type() == QVariant::Vector2D) {
                QVector2D vec = property.read(o).value<QVector2D>();
                value = QString("%1, %2").arg(vec.x()).arg(vec.y());
            } else {
                value = property.read(o).toString();
            }
            ((QTextEdit*)m_objectEditorList.at(i-1))->setText(value);
        }
    }
}
