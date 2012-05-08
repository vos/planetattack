#include "playerintelligencepropertyeditor.h"

#include "defaultplayerintelligence.h"

PlayerIntelligencePropertyEditor::PlayerIntelligencePropertyEditor(QWidget *parent) :
    QComboBox(parent)
{
    addItem("DefaultPlayerIntelligence");
}

QVariant PlayerIntelligencePropertyEditor::value() const
{
    PlayerIntelligence *pi;
    switch (currentIndex()) {
     case 0:
        pi = new DefaultPlayerIntelligence;
        break;
    }
    QVariant var;
    var.setValue(pi);
    return var;
}

void PlayerIntelligencePropertyEditor::setValue(const QVariant &value)
{
    // TODO
}
