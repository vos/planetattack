#ifndef PLAYERINTELLIGENCE_H
#define PLAYERINTELLIGENCE_H

#include "intelligence.h"
#include "canvas.h"

class ComputerPlayer;

class PlayerIntelligence : public Intelligence
{
    Q_OBJECT

public:
    explicit PlayerIntelligence(Player *parent = NULL);

protected:
    inline Player* getSelf() { return (Player*)parent(); }
    inline QSet<Player*>& getAllPlayers() { return Canvas::Instance->players(); }
    inline QSet<Planet*>& getAllPlanets() { return Canvas::Instance->planets(); }
    QSet<Player*> getEnemies();
    QSet<Planet*> getEnemyPlanets();
    QSet<Planet*> getNeutralPlanets();
    QSet<Planet*> getOtherPlanets();

};

Q_DECLARE_METATYPE(PlayerIntelligence*)

#endif // PLAYERINTELLIGENCE_H
