#ifndef SHIP_H
#define SHIP_H

#include "spaceobject.h"
#include "planet.h"

class Ship : public SpaceObject
{
    Q_OBJECT

public:
    Ship(const QVector2D& position, Planet *target, int resources = 0, const QColor &color = Qt::blue, QObject *parent = NULL);

    inline const Planet* target() const { return m_target; }

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    Planet *m_target;
    int m_speed; // pixels per second
    QVector2D m_direction; // temp
    
};

#endif // SHIP_H
