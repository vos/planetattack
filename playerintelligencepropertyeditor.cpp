#include "playerintelligencepropertyeditor.h"

#include <QVBoxLayout>
#include <QFileDialog>

#include "playerintelligence.h"
#include "scriptedplayerintelligence.h"
#include "canvas.h"

PlayerIntelligencePropertyEditor::PlayerIntelligencePropertyEditor(QWidget *parent) :
    QWidget(parent)
{
    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    setLayout(layout);
    layout->addWidget(&m_comboBox);
    m_scriptButton.setText("Edit Script");
    m_scriptButton.setVisible(false);
    layout->addWidget(&m_scriptButton);

    m_comboBox.addItem("PlayerIntelligence");
    m_comboBox.addItem("ScriptedPlayerIntelligence");

    connect(&m_comboBox, SIGNAL(currentIndexChanged(int)), SLOT(comboBox_currentIndexChanged(int)));
    connect(&m_scriptButton, SIGNAL(clicked()), SLOT(scriptButton_clicked()));
}

QVariant PlayerIntelligencePropertyEditor::value() const
{
    QVariant var;
    var.setValue(m_playerIntelligence);
    return var;
}

void PlayerIntelligencePropertyEditor::setValue(const QVariant &value)
{
    m_playerIntelligence = value.value<PlayerIntelligence*>();
    const QMetaObject *metaObject = m_playerIntelligence->metaObject();
    m_comboBox.setCurrentIndex(m_comboBox.findText(QString(metaObject->className())));
}

void PlayerIntelligencePropertyEditor::comboBox_currentIndexChanged(int index)
{
    // TODO handle "memory leaks"
    const char* className = m_playerIntelligence->metaObject()->className();
    switch (index) {
    case 0:
        if (strcmp(className, "PlayerIntelligence") != 0)
            m_playerIntelligence = new PlayerIntelligence;
        break;
    case 1:
        if (strcmp(className, "ScriptedPlayerIntelligence") != 0)
            m_playerIntelligence = new ScriptedPlayerIntelligence(&Canvas::Instance->scriptEngine());
        break;
    }
    m_scriptButton.setVisible(index == 1);
}

void PlayerIntelligencePropertyEditor::scriptButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Open Script File", QString(), "Qt Script Files (*.qs *.js);;All Files (*.*)");
    if (!fileName.isEmpty()) {
        ScriptedPlayerIntelligence *spi = qobject_cast<ScriptedPlayerIntelligence*>(m_playerIntelligence);
        if (spi != NULL) {
            QFile file(fileName);
            spi->setIntelligenceProgram(file);
        } else {
            qWarning("error: cannot cast to ScriptedPlayerIntelligence*");
        }
    }
}
