#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "canvas.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_canvas = new Canvas(this);
    setCentralWidget(m_canvas);
    setWindowTitle(m_canvas->windowTitle());
    m_canvas->setFocus();
}

MainWindow::~MainWindow()
{
    delete ui;
}
