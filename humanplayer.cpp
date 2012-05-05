#include "humanplayer.h"

HumanPlayer::HumanPlayer(const QString &name, const QColor &color, QObject *parent) :
    Player(name, color, true, parent)
{
}
