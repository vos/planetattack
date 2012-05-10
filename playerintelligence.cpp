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

QSet<Planet*> PlayerIntelligence::getEnemyPlanets()
{
    QSet<Planet*> planets;
    foreach (Player *player, getEnemies()) {
        planets.unite(player->planets());
    }
    return planets;
}

QSet<Planet*> PlayerIntelligence::getNeutralPlanets()
{
    QSet<Planet*> planets;
    foreach (Planet *planet, getAllPlanets()) {
        if (planet->isNeutral()) {
            planets.insert(planet);
        }
    }
    return planets;
}

QSet<Planet*> PlayerIntelligence::getOtherPlanets()
{
    QSet<Planet*> planets;
    Player *self = getSelf();
    foreach (Planet *planet, getAllPlanets()) {
        if (planet->player() != self) {
            planets.insert(planet);
        }
    }
    return planets;
}
