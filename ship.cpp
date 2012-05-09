#include "ship.h"

#define _USE_MATH_DEFINES
#include <math.h> // cmath won't compile with _USE_MATH_DEFINES

#include "player.h"

Ship::Ship(const QVector2D& position, Planet *target, qreal resources, const QColor &color, Player *parent) :
    SpaceObject(position, resources, color, parent)
{
    m_target = target;
    m_speed = 100.0; // default speed
}

void Ship::update(const GameTime &gameTime)
{
    if (m_target != NULL) {
        // move towards target
        m_direction = m_target->position() - m_position;
        m_direction.normalize();
        m_position += m_direction * (m_speed * gameTime.elapsedGameTimeSeconds());
        if ((m_target->position() - m_position).length() <= m_target->radius()) {
            Player *targetOwner = m_target->player();
            if (targetOwner == player()) {
                // own planet -> add resources
                m_target->addResources(m_resources);
            } else {
                // enemy planet -> subtract resources
                if (m_target->subtractResources(m_resources) < 0.0) {
                    // resources depleted -> take-over target planet!
                    targetOwner->removePlanet(m_target);
                    player()->addPlanet(m_target);
                    m_target->addResources(m_resources);
                }
            }
            m_resources = 0.0;
            m_target = NULL; // marks this ship for removal
        }
    }
}

void Ship::draw(QPainter &painter)
{
    QPolygon polygon; // TODO static const
    polygon.append(QPoint(0,-8));
    polygon.append(QPoint(5,8));
    polygon.append(QPoint(-5,8));
    polygon.append(QPoint(0,-8));

    QMatrix matrix;
    qreal degrees = atan2(m_direction.y(), m_direction.x()) * (180 / M_PI) + 90; // REFAC
    matrix.rotate(degrees);
    polygon = matrix.map(polygon);
    matrix.reset();
    matrix.translate(m_position.x(), m_position.y());
    polygon = matrix.map(polygon);

    painter.setPen(QPen(m_color, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawPolyline(polygon);

    painter.setPen(Qt::white);
    painter.drawText(m_position.toPoint(), QString::number(int(m_resources)));
}
