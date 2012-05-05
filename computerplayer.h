#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include "player.h"
#include "playerintelligence.h"

class ComputerPlayer : public Player
{
    Q_OBJECT

public:
    ComputerPlayer(const QString &name, const QColor &color = Qt::red, PlayerIntelligence *intelligence = NULL, QObject *parent = NULL);

    inline PlayerIntelligence* intelligence() const { return m_intelligence; }
    inline void setIntelligence(PlayerIntelligence *intelligence) { m_intelligence = intelligence; m_intelligence->setParent(this); }

private:
    PlayerIntelligence *m_intelligence;

};

#endif // COMPUTERPLAYER_H
