#include "scriptwindow.h"
#include "ui_scriptwindow.h"

#include <QStatusBar>
#include <QFileDialog>
#include <QMessageBox>

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
        m_statusBar->showMessage("Script is empty", 5000);
    }
    updateTitle();
}

ScriptWindow::~ScriptWindow()
{
    delete ui;
}

void ScriptWindow::on_applyButton_clicked()
{
    if (m_playerIntelligence->setIntelligenceProgram(ui->scriptEditor->toPlainText())) {
        m_statusBar->showMessage("Script successfully updated", 5000);
        updateTitle();
    } else {
        m_statusBar->showMessage("Syntax error in the script");
    }
}

void ScriptWindow::on_openScriptFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Script File",
                                                    QDir::current().filePath("scripts"),
                                                    "Qt Script Files (*.js *.qs );;All Files (*.*)");
    if (!fileName.isEmpty()) {
        if (m_playerIntelligence->setIntelligenceProgramFile(fileName)) {
            ui->scriptEditor->setPlainText(m_playerIntelligence->intelligenceProgram().sourceCode());
            m_statusBar->showMessage("Script successfully loaded and applied", 5000);
            updateTitle();
        } else {
            m_statusBar->showMessage("Failed to load the script");
        }
    }
}

void ScriptWindow::on_saveScriptFileButton_clicked()
{
    QString fileName(m_playerIntelligence->intelligenceProgram().fileName());
    if (fileName.isEmpty()) {
        fileName = QFileDialog::getSaveFileName(this, "Save Script File",
                                                QDir::current().filePath("scripts"),
                                                "Qt Script Files (*.js)");
    }
    if (!fileName.isEmpty()) {
        QScriptProgram intelligenceProgram(ui->scriptEditor->toPlainText(), fileName);
        if (m_playerIntelligence->setIntelligenceProgram(intelligenceProgram)) {
            QFile file(fileName);
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                file.write(intelligenceProgram.sourceCode().toUtf8());
                file.close();
                m_statusBar->showMessage("Script successfully saved and applied", 5000);
                updateTitle();
            } else {
                QMessageBox::warning(this, "Save Script File Error",
                                     QString("Cannot save script file \"%1\":\n%2")
                                     .arg(fileName, file.errorString()));
            }
        } else {
            m_statusBar->showMessage("Syntax error in the script");
        }
    }
}

void ScriptWindow::on_showDebugOutputButton_clicked()
{
    ui->splitter->addWidget(Canvas::Instance->scriptEngineDebugger()->widget(QScriptEngineDebugger::DebugOutputWidget));
    ui->splitter->setSizes(QList<int>() << ui->splitter->height() << 100);
}

void ScriptWindow::updateTitle()
{
    const QScriptProgram &intelligenceProgram = m_playerIntelligence->intelligenceProgram();
    QString fileName = intelligenceProgram.fileName();
    setWindowTitle(QString("Script Editor - %1").arg(fileName.isEmpty() ? "<unsaved script>" : QDir::current().relativeFilePath(fileName)));
}
