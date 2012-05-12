#ifndef PLAYERINTELLIGENCE_H
#define PLAYERINTELLIGENCE_H

#include "intelligence.h"
#include "player.h"

#include <QMetaType>

class PlayerIntelligence : public Intelligence
{
    Q_OBJECT

public:
    explicit PlayerIntelligence(Player *parent = NULL);

    inline Player* player() { return m_player; }
    virtual void setPlayer(Player *player) { m_player = player; setParent(player); }

private:
    Player *m_player;

};

Q_DECLARE_METATYPE(PlayerIntelligence*)

#endif // PLAYERINTELLIGENCE_H
