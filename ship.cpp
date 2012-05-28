#include "ship.h"

#define _USE_MATH_DEFINES
#include <math.h> // cmath won't compile with _USE_MATH_DEFINES

#include "player.h"
#include "planet.h"

Ship::Ship() :
    SpaceObject()
{
}

Ship::Ship(const QVector2D& position, Planet *target, qreal resources, const QColor &color, Player *parent) :
    SpaceObject(position, resources, color, parent)
{
    setTarget(target);
    m_speed = 100.0; // default speed
}

void Ship::setTarget(Planet *target)
{
    if (target == NULL) {
        qDebug("Ship::setTarget target cannot be null");
        return;
    }
    m_target = target;
}

void Ship::update(const GameTime &gameTime)
{
    if (m_target == NULL)
        return;

    // move towards target
    m_direction = m_target->position() - m_position;
    m_direction.normalize();
    m_position += m_direction * (m_speed * gameTime.elapsedSeconds());
    if ((m_target->position() - m_position).length() <= m_target->radius()) {
        Player *targetOwner = m_target->player();
        if (targetOwner == player()) {
            // own planet -> add resources
            m_target->addResources(m_resources);
        } else {
            // neutral or enemy planet
            if (m_target->resources() - m_resources < 0.0) {
                // resources depleted -> take-over target planet!
                if (!m_target->isNeutral()) { // planet has no owner if it is neutral
                    targetOwner->removePlanet(m_target);
                }
                player()->addPlanet(m_target);
                m_target->setResources(m_resources - m_target->resources());
            } else {
                // subtract resources
                m_target->subtractResources(m_resources);
            }
        }
        m_resources = 0.0;
        m_target = NULL; // marks this ship for removal
    }
}

void Ship::draw(QPainter &painter)
{
    static const QPolygon s_polygon = QPolygon()
            << QPoint( 0, -8)
            << QPoint( 5,  8)
            << QPoint(-5,  8)
            << QPoint( 0, -8);

    QMatrix matrix;
    qreal degrees = atan2(m_direction.y(), m_direction.x()) * (180 / M_PI) + 90; // REFAC
    matrix.rotate(degrees);
    QPolygon polygon = matrix.map(s_polygon);
    polygon.translate(m_position.toPoint());

    painter.setPen(QPen(m_color, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawPolyline(polygon);

    painter.setPen(Qt::white);
    painter.drawText(m_position.toPoint(), QString::number(int(m_resources)));
}

#ifndef QT_NO_DATASTREAM
QDataStream& operator<<(QDataStream &stream, const Ship &ship)
{
    return stream << ship.m_position
                  << ship.m_resources
                  << ship.m_color
                  << ship.m_speed;
}

QDataStream& operator>>(QDataStream &stream, Ship &ship)
{
    return stream >> ship.m_position
                  >> ship.m_resources
                  >> ship.m_color
                  >> ship.m_speed;
}
#endif
