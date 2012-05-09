#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"

class Planet : public SpaceObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)

public:
    Planet(const QVector2D& position, qreal radius, qreal resources, const QColor &color, QObject *parent = NULL);

    inline qreal radius() const { return m_radius; }
    inline void setRadius(qreal radius) { m_radius = radius; }

    QRect rect() const;

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    static const QColor SelectedColor;

    qreal m_radius;

};

#endif // PLANET_H
