#include "spaceobject.h"

SpaceObject::SpaceObject(const QVector2D& position, int resources, const QColor &color, QObject *parent) :
    Drawable(parent)
{
    m_position = position;
    m_resources = resources;
    m_color = color;
}
