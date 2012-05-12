#include "playerintelligence.h"

PlayerIntelligence::PlayerIntelligence(Player *parent) :
    Intelligence(parent)
{
    m_player = parent;
}
