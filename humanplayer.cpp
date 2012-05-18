#include "humanplayer.h"

HumanPlayer::HumanPlayer(const QString &name, const QColor &color, Game *parent) :
    Player(name, color, true, parent)
{
}
