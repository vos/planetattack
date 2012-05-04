#include "spaceobject.h"

SpaceObject::SpaceObject(const QVector2D& position, int resources, const QColor &color, QObject *parent) :
    Drawable(parent)
{
    mPosition = position;
    mResources = resources;
    mColor = color;
}
