#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"

class Planet : public SpaceObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
    Q_PROPERTY(qreal productionFactor READ productionFactor WRITE setProductionFactor)

public:
    Planet(const QVector2D& position, qreal radius, qreal resources, const QColor &color, Player *parent = NULL);

    inline qreal radius() const { return m_radius; }
    inline void setRadius(qreal radius) { m_radius = radius; }

    inline qreal productionFactor() const { return m_productionFactor; }
    inline void setProductionFactor(qreal factor) { m_productionFactor = factor; }

    QRect rect() const;

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    qreal m_radius;
    qreal m_productionFactor;

};

#endif // PLANET_H
