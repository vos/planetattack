#include "spaceobject.h"
#include "player.h"

SpaceObject::SpaceObject(const QVector2D& position, qreal resources, const QColor &color, Player *parent) :
    Drawable(parent)
{
    m_position = position;
    m_resources = resources;
    m_color = color;
}

bool SpaceObject::setPlayer(Player *p)
{
    if (p == player())
        return false;
    setParent((QObject*)p);
    return true;
}
