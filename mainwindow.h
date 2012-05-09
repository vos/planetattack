#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>

#include "canvas.h"
#include "propertyeditor.h"

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
    void canvas_modeChanged();
    void canvas_selectionChanged(QObject *o);
    void saveButton_clicked();
    void player_nameChanged(const QString &oldName, const QString &newName);

    void on_modeComboBox_currentIndexChanged(int index);
    void on_playerComboBox_activated(int index);
    void on_addPlayerButton_clicked();

private:
    Ui::MainWindow *ui;
    Canvas *m_canvas;
    QObject *m_selectedObject;
    QList<PropertyEditor*> m_propertyEditorList;

};

#endif // MAINWINDOW_H
