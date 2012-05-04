#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

#include <drawable.h>

#include <QVector2D>
#include <QColor>

class SpaceObject : public Drawable
{
    Q_OBJECT

public:
    SpaceObject(const QVector2D& position = QVector2D(), int resources = 0, const QColor &color = Qt::blue, QObject *parent = NULL);

    inline const QVector2D& position() const { return mPosition; }
    inline void setPosition(const QVector2D &position) { mPosition = position; }

    inline int resources() const { return mResources; }
    inline void setResources(int resources) { mResources = resources; }

    inline const QColor& color() const { return mColor; }
    inline void setColor(const QColor &color = Qt::blue) { mColor = color; }

protected:
    QVector2D mPosition;
    int mResources;
    QColor mColor;
    
};

#endif // SPACEOBJECT_H
