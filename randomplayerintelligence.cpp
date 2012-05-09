#include "randomplayerintelligence.h"

#include "random.h"

RandomPlayerIntelligence::RandomPlayerIntelligence(Player *parent) :
    PlayerIntelligence(parent)
{
    m_timer.start();
    m_delay = Random::randomInt(5000, 10000); // start delay
}

void RandomPlayerIntelligence::think(const GameTime &gameTime)
{
    Q_UNUSED(gameTime)
    if (m_timer.hasExpired(m_delay)) { // don't think too much!
        Player *self = getSelf();
        // choose own planet at random
        if (self->planets().isEmpty()) return; // I have no planets! :o
        Planet *myPlanet = *Random::randomElement(self->planets());
        // choose random enemy planet
        QSet<Player*> enemies = getEnemies();
        if (enemies.isEmpty()) return; // no enemies found! :)
        Player *enemyPlayer = *Random::randomElement(enemies);
        if (enemyPlayer->planets().isEmpty()) return; // enemy has no planets!
        Planet *enemyPlanet = *Random::randomElement(enemyPlayer->planets());
        // attack enemy planet with 25-75% of my choosen planets resources
        self->addShip(myPlanet, enemyPlanet, Random::randomReal(0.25, 0.75));

        m_delay = Random::randomInt(250, 1000);
        m_timer.restart();
    }
}
