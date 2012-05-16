#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include "player.h"
#include "playerintelligence.h"

class ComputerPlayer : public Player
{
    Q_OBJECT
    Q_PROPERTY(PlayerIntelligence* intelligence READ intelligence WRITE setIntelligence SCRIPTABLE false)

public:
    ComputerPlayer(const QString &name, const QColor &color = Qt::red, PlayerIntelligence *intelligence = NULL, QObject *parent = NULL);

    inline PlayerIntelligence* intelligence() const { return m_intelligence; }
    void setIntelligence(PlayerIntelligence *intelligence = NULL);
    inline bool hasIntelligence() const { return m_intelligence != NULL; }

    void think(const GameTime &gameTime);

private:
    PlayerIntelligence *m_intelligence;

};

#endif // COMPUTERPLAYER_H
