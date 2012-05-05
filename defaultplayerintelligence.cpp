#include "defaultplayerintelligence.h"

#include "computerplayer.h"

DefaultPlayerIntelligence::DefaultPlayerIntelligence(QObject *parent) :
    PlayerIntelligence(parent)
{
}

void DefaultPlayerIntelligence::think(const GameTime &gameTime)
{
    ComputerPlayer *player = computerPlayer();
    // TODO think hard
    foreach (Planet *planet, player->planets()) {
        // TODO
    }
}
