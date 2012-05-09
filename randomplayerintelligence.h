#ifndef RANDOMPLAYERINTELLIGENCE_H
#define RANDOMPLAYERINTELLIGENCE_H

#include "playerintelligence.h"

class RandomPlayerIntelligence : public PlayerIntelligence
{
    Q_OBJECT

public:
    explicit RandomPlayerIntelligence(Player *parent = NULL);

    void think(const GameTime &gameTime);

private:
    QElapsedTimer m_timer;
    int m_delay;

};

#endif // RANDOMPLAYERINTELLIGENCE_H
