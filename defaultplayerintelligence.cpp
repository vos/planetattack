#include "defaultplayerintelligence.h"

#include "computerplayer.h"
#include "random.h"

DefaultPlayerIntelligence::DefaultPlayerIntelligence(QObject *parent) :
    PlayerIntelligence(parent)
{
    m_timer.start();
    m_delay = Random::randomInt(5000, 10000); // start delay
}

void DefaultPlayerIntelligence::think(const GameTime &gameTime)
{
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
        int res = myPlanet->resources() * Random::randomReal(0.25, 0.75);
        myPlanet->setResources(myPlanet->resources() - res);
        Ship *ship = new Ship(myPlanet->position(), enemyPlanet, res, myPlanet->color(), self);
        self->ships().insert(ship);

        m_delay = Random::randomInt(250, 1000);
        m_timer.restart();
    }
}
