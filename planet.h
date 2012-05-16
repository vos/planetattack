#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"
#include <QMetaType>
#include <QSet>

class Ship;

class Planet : public SpaceObject
{
    Q_OBJECT
    Q_PROPERTY(qreal radius READ radius WRITE setRadius)
    Q_PROPERTY(qreal productionFactor READ productionFactor WRITE setProductionFactor)
    Q_PROPERTY(bool neutral READ isNeutral DESIGNABLE false)

public:
    Planet(const QVector2D& position, qreal radius = 50.0, qreal resources = 0.0, const QColor &color = Qt::darkGray, Player *parent = NULL);

    inline qreal radius() const { return m_radius; }
    inline void setRadius(qreal radius) { m_radius = radius; }

    inline qreal productionFactor() const { return m_productionFactor; }
    inline void setProductionFactor(qreal factor) { m_productionFactor = factor; }

    inline bool isNeutral() const { return !hasPlayer(); }
    bool setPlayer(Player *player);

    Q_INVOKABLE Ship* transferResourcesTo(Planet *target, qreal resourceFactor);
    Q_INVOKABLE Ship* transferResourcesTo(Planet *target);

    QRect rect() const;

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    qreal m_radius;
    qreal m_productionFactor;

    // animation
    qreal m_angle;
    qreal m_velocity;

};

Q_DECLARE_METATYPE(Planet*)
Q_DECLARE_METATYPE(QSet<Planet*>)

#endif // PLANET_H
