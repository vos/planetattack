#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"

#include <QSet>

#include "ship.h"

class Planet : public SpaceObject
{
    Q_OBJECT

public:
    Planet(const QVector2D& position, int radius, int resources, const QColor &color, QObject *parent = NULL);

    inline int radius() const { return m_radius; }
    inline void setRadius(int radius) { m_radius = radius; }

    inline QSet<Ship*>& ships() { return m_ships; }

    QRect rect() const;

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    static const QColor SelectedColor;

    int m_radius;
    QSet<Ship*> m_ships;
    QElapsedTimer m_timer;

};

#endif // PLANET_H
