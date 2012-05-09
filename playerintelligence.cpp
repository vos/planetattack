#include "playerintelligence.h"
#include "canvas.h"

PlayerIntelligence::PlayerIntelligence(Player *parent) :
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
