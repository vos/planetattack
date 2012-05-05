#ifndef DEFAULTPLAYERINTELLIGENCE_H
#define DEFAULTPLAYERINTELLIGENCE_H

#include "playerintelligence.h"

class DefaultPlayerIntelligence : public PlayerIntelligence
{
    Q_OBJECT

public:
    explicit DefaultPlayerIntelligence(QObject *parent = NULL);

    void think(const GameTime &gameTime);

};

#endif // DEFAULTPLAYERINTELLIGENCE_H
