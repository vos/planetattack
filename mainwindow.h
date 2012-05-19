#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHash>

#include "game.h"

class Canvas;
class PropertyEditor;
class MultiplayerServer;

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
    void game_modeChanged(Game::Mode mode);
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

    void on_action_createServer_triggered();
    void on_action_ConnectToServer_triggered();

private:
    Ui::MainWindow *ui;
    Canvas *m_canvas;
    Game *m_game;
    QString m_scenarioFileName;
    MultiplayerServer *m_server;
    QObject *m_selectedObject;
    QHash<QString, PropertyEditor*> m_propertyEditorMap;

    void clearPropertyEditor();
    void updateTitle(const QString &subTitle);

};

#endif // MAINWINDOW_H
