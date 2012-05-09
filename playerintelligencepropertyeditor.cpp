#include "playerintelligencepropertyeditor.h"

#include "randomplayerintelligence.h"

PlayerIntelligencePropertyEditor::PlayerIntelligencePropertyEditor(QWidget *parent) :
    QComboBox(parent)
{
    addItem("PlayerIntelligence");
    addItem("RandomPlayerIntelligence");
}

QVariant PlayerIntelligencePropertyEditor::value() const
{
    PlayerIntelligence *pi;
    switch (currentIndex()) {
    case 0:
        pi = new PlayerIntelligence;
        break;
    case 1:
        pi = new RandomPlayerIntelligence;
        break;
    }
    QVariant var;
    var.setValue(pi);
    return var;
}

void PlayerIntelligencePropertyEditor::setValue(const QVariant &value)
{
    PlayerIntelligence *pi = value.value<PlayerIntelligence*>();
    const QMetaObject *metaObject = pi->metaObject();
    setCurrentIndex(findText(QString(metaObject->className())));
}
