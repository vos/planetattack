#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

class HumanPlayer : public Player
{
    Q_OBJECT

public:
    explicit HumanPlayer(const QString &name, const QColor &color, Game *parent = NULL);

};

#endif // HUMANPLAYER_H
