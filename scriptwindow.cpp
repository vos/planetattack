#include "scriptwindow.h"
#include "ui_scriptwindow.h"

#include <QStatusBar>
#include <QFileDialog>
#include "scriptedplayerintelligence.h"

// debug
#include "canvas.h"
#include <QScriptEngineDebugger>

ScriptWindow::ScriptWindow(ScriptedPlayerIntelligence *spi, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::ScriptWindow)
{
    ui->setupUi(this);
    setModal(false);

    m_statusBar = new QStatusBar;
    ui->verticalLayout->addWidget(m_statusBar);

    ui->splitter->setCollapsible(0, false);
    ui->scriptEditor->setDarkColorTheme();

    m_playerIntelligence = spi;
    if (!m_playerIntelligence->intelligenceProgram().isNull()) {
        ui->scriptEditor->setPlainText(m_playerIntelligence->intelligenceProgram().sourceCode());
        m_statusBar->showMessage("Script loaded", 5000);
    } else {
        m_statusBar->showMessage("Script is empty");
    }
}

ScriptWindow::~ScriptWindow()
{
    delete ui;
}

void ScriptWindow::on_applyButton_clicked()
{
    if (m_playerIntelligence->setIntelligenceProgram(ui->scriptEditor->toPlainText())) {
        m_statusBar->showMessage("Script successfully updated", 5000);
    } else {
        m_statusBar->showMessage("Syntax error in the script");
    }
}

void ScriptWindow::on_openScriptFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Script File", QString(), "Qt Script Files (*.qs *.js);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        if (m_playerIntelligence->setIntelligenceProgramFile(fileName)) {
            ui->scriptEditor->setPlainText(m_playerIntelligence->intelligenceProgram().sourceCode());
            m_statusBar->showMessage("Script successfully loaded and applied", 5000);
        } else {
            m_statusBar->showMessage("Failed to load the script");
        }
    }
}

void ScriptWindow::on_showDebugOutputButton_clicked()
{
    ui->splitter->addWidget(Canvas::Instance->scriptEngineDebugger()->widget(QScriptEngineDebugger::DebugOutputWidget));
    ui->splitter->setSizes(QList<int>() << ui->splitter->height() << 100);
}
