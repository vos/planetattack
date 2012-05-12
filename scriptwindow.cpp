#include "scriptwindow.h"
#include "ui_scriptwindow.h"

#include <QFileDialog>
#include "scriptedplayerintelligence.h"

ScriptWindow::ScriptWindow(ScriptedPlayerIntelligence *spi, QWidget *parent) :
    QDialog(parent, Qt::Window),
    ui(new Ui::ScriptWindow)
{
    ui->setupUi(this);
    setModal(false);
    m_playerIntelligence = spi;
    if (!m_playerIntelligence->intelligenceProgram().isNull()) {
        ui->scriptTextEdit->setText(m_playerIntelligence->intelligenceProgram().sourceCode());
    }
}

ScriptWindow::~ScriptWindow()
{
    delete ui;
}

void ScriptWindow::on_applyButton_clicked()
{
    m_playerIntelligence->setIntelligenceProgram(ui->scriptTextEdit->toPlainText());
}

void ScriptWindow::on_openScriptFileButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Script File", QString(), "Qt Script Files (*.qs *.js);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        if (m_playerIntelligence->setIntelligenceProgramFile(fileName)) {
            ui->scriptTextEdit->setText(m_playerIntelligence->intelligenceProgram().sourceCode());
        }
    }
}
