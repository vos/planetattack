#include "playerintelligence.h"

PlayerIntelligence::PlayerIntelligence(QObject *parent) :
    Intelligence(parent)
{
}

QSet<Player*> PlayerIntelligence::getEnemies()
{
    QSet<Player*> enemies;
    Player *self = getSelf();
    foreach (Player *player, getAllPlayers()) {
        if (player != self) {
            enemies.insert(player);
        }
    }
    return enemies;
}
