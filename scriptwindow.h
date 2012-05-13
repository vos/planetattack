#ifndef SCRIPTWINDOW_H
#define SCRIPTWINDOW_H

#include <QDialog>

QT_FORWARD_DECLARE_CLASS(QStatusBar)

namespace Ui {
class ScriptWindow;
}

class ScriptedPlayerIntelligence;

class ScriptWindow : public QDialog
{
    Q_OBJECT

public:
    explicit ScriptWindow(ScriptedPlayerIntelligence *spi, QWidget *parent = NULL);
    ~ScriptWindow();

private slots:
    void on_applyButton_clicked();
    void on_openScriptFileButton_clicked();
    void on_showDebugOutputButton_clicked();

private:
    Ui::ScriptWindow *ui;
    QStatusBar *m_statusBar;
    ScriptedPlayerIntelligence *m_playerIntelligence;

};

#endif // SCRIPTWINDOW_H
