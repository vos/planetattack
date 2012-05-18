#include "playerpropertyeditor.h"

#include "game.h"
#include "player.h"

PlayerPropertyEditor::PlayerPropertyEditor(QWidget *parent) :
    QComboBox(parent)
{
    QVariant var;
    var.setValue<Player*>(NULL);
    addItem("<null>", var);
    foreach (Player *player, Game::instance()->players()) {
        QVariant var;
        var.setValue(player);
        addItem(player->name(), var);
    }
}

QVariant PlayerPropertyEditor::value() const
{
    return itemData(currentIndex());
}

void PlayerPropertyEditor::setValue(const QVariant &value)
{
    setCurrentIndex(findData(value));
}
