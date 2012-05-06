#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "canvas.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void canvas_selectionChanged(QObject *o);

private:
    Ui::MainWindow *ui;
    Canvas *m_canvas;
    QObject *m_selectedObject;
    QList<QWidget*> m_objectEditorList;

};

#endif // MAINWINDOW_H
