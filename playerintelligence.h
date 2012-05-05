#ifndef PLAYERINTELLIGENCE_H
#define PLAYERINTELLIGENCE_H

#include "intelligence.h"
#include "canvas.h"

class ComputerPlayer;

class PlayerIntelligence : public Intelligence
{
    Q_OBJECT

public:
    explicit PlayerIntelligence(QObject *parent = NULL);

protected:
    inline Player* getSelf() { return (Player*)parent(); }
    inline QSet<Player*>& getAllPlayers() { return Canvas::Instance->players(); }
    QSet<Player*> getEnemies();

};

#endif // PLAYERINTELLIGENCE_H
