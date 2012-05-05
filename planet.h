#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"

class Planet : public SpaceObject
{
    Q_OBJECT

public:
    Planet(const QVector2D& position, int radius, int resources, const QColor &color = Qt::blue, QObject *parent = NULL);

    inline int radius() const { return m_radius; }
    inline void setRadius(int radius) { m_radius = radius; }

    QRect rect() const;

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    int m_radius;
    QElapsedTimer m_timer;

};

#endif // PLANET_H
