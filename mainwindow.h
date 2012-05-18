#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

class Canvas;
class Game;
class PropertyEditor;

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
    void game_modeChanged();
    void canvas_selectionChanged(QObject *o);
    void saveButton_clicked();
    void player_nameChanged(const QString &oldName, const QString &newName);

    void on_modeComboBox_currentIndexChanged(int index);
    void updatePlayerComboBox();
    void on_playerComboBox_activated(int index);
    void on_removePlayerButton_clicked();
    void on_addPlayerButton_clicked();
    void on_globalAccessCheckBox_toggled(bool checked);

    void on_action_newScenario_triggered();
    void on_action_openScenario_triggered();
    void on_action_saveScenario_triggered();
    void on_action_saveScenarioAs_triggered();

private:
    Ui::MainWindow *ui;
    Canvas *m_canvas;
    Game *m_game;
    QString m_scenarioFileName;
    QObject *m_selectedObject;
    QHash<QString, PropertyEditor*> m_propertyEditorMap;

    void clearPropertyEditor();
    void updateTitle(const QString &subTitle);

};

#endif // MAINWINDOW_H
