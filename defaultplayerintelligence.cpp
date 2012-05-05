#include "defaultplayerintelligence.h"

#include "computerplayer.h"
#include "random.h"

DefaultPlayerIntelligence::DefaultPlayerIntelligence(QObject *parent) :
    PlayerIntelligence(parent)
{
    m_timer.start();
}

void DefaultPlayerIntelligence::think(const GameTime &gameTime)
{
    if (m_timer.hasExpired(500)) { // don't think too much!
        Player *self = getSelf();
        // choose own planet at random
        if (self->planets().isEmpty()) return; // I have no planets! :o
        Planet *myPlanet = *Random::randomElement(self->planets());
        // choose random enemy planet
        Player *enemyPlayer = *Random::randomElement(getEnemies());
        if (!enemyPlayer) return; // no enemy found! :)
        if (enemyPlayer->planets().isEmpty()) return; // enemy has no planets!
        Planet *enemyPlanet = *Random::randomElement(enemyPlayer->planets());
        // attack enemy planet with 50% of my choosen planets resources
        int res = int(myPlanet->resources() * 0.5);
        myPlanet->setResources(myPlanet->resources() - res);
        Ship *ship = new Ship(myPlanet->position(), enemyPlanet, res, myPlanet->color(), myPlanet);
        myPlanet->ships().insert(ship);
        m_timer.restart();
    }
}
