#include "computerplayer.h"

ComputerPlayer::ComputerPlayer(const QString &name, const QColor &color, PlayerIntelligence *intelligence, Game *parent) :
    Player(name, color, false, parent), m_intelligence(NULL)
{
    setIntelligence(intelligence);
}

void ComputerPlayer::setIntelligence(PlayerIntelligence *intelligence)
{
    if (m_intelligence != NULL && m_intelligence != intelligence)
        delete m_intelligence;
    m_intelligence = intelligence;
    if (m_intelligence)
        m_intelligence->setPlayer(this);
}

void ComputerPlayer::think(const GameTime &gameTime)
{
    if (hasIntelligence()) {
        m_intelligence->think(gameTime);
    }
}
