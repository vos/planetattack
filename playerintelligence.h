#ifndef PLAYERINTELLIGENCE_H
#define PLAYERINTELLIGENCE_H

#include "intelligence.h"

class ComputerPlayer;

class PlayerIntelligence : public Intelligence
{
    Q_OBJECT

public:
    explicit PlayerIntelligence(QObject *parent = NULL);

    inline ComputerPlayer* computerPlayer() const { return (ComputerPlayer*)parent(); } // qobject_cast<ComputerPlayer*>(parent());

};

#endif // PLAYERINTELLIGENCE_H
