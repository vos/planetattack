#include "computerplayer.h"

#include "defaultplayerintelligence.h"

ComputerPlayer::ComputerPlayer(const QString &name, const QColor &color, PlayerIntelligence *intelligence, QObject *parent) :
    Player(name, color, false, parent)
{
    setIntelligence(intelligence != NULL ? intelligence : new DefaultPlayerIntelligence(this));
}
