#include "playerintelligencepropertyeditor.h"

#include <QVBoxLayout>

#include "scriptedplayerintelligence.h"
#include "game.h"
#include "scriptwindow.h"

PlayerIntelligencePropertyEditor::PlayerIntelligencePropertyEditor(QWidget *parent) :
    QWidget(parent)
{
    m_playerIntelligence = NULL;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setMargin(0);
    setLayout(layout);
    layout->addWidget(&m_comboBox);
    m_scriptButton.setText("Edit Script");
    m_scriptButton.setVisible(false);
    layout->addWidget(&m_scriptButton);

    m_comboBox.addItem("<null>");
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
    if (m_playerIntelligence != NULL) {
        const QMetaObject *metaObject = m_playerIntelligence->metaObject();
        m_comboBox.setCurrentIndex(m_comboBox.findText(QString(metaObject->className())));
    } else {
        m_comboBox.setCurrentIndex(0);
    }
}

void PlayerIntelligencePropertyEditor::comboBox_currentIndexChanged(int index)
{
    // FIXME: handle "memory leaks"
    const QString className(m_playerIntelligence ? m_playerIntelligence->metaObject()->className() : "");
    switch (index) {
    case 0:
        m_playerIntelligence = NULL;
        break;
    case 1:
        if (className != "ScriptedPlayerIntelligence")
            m_playerIntelligence = new ScriptedPlayerIntelligence(Game::instance()->scriptEngine());
        break;
    }
    m_scriptButton.setVisible(index == 1);
}

void PlayerIntelligencePropertyEditor::scriptButton_clicked()
{
    ScriptedPlayerIntelligence *spi = qobject_cast<ScriptedPlayerIntelligence*>(m_playerIntelligence);
    if (spi == NULL) {
        qWarning("error: cannot cast to ScriptedPlayerIntelligence*");
        return;
    }

    ScriptWindow *scriptWindow = new ScriptWindow(spi, this);
    scriptWindow->show();
    scriptWindow->raise();
    scriptWindow->activateWindow();
}
