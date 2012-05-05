#include "ship.h"

#define _USE_MATH_DEFINES
#include <math.h> // cmath won't compile with _USE_MATH_DEFINES

Ship::Ship(const QVector2D& position, Planet *target, int resources, const QColor &color, QObject *parent) :
    SpaceObject(position, resources, color, parent)
{
    m_target = target;
    m_speed = 100; // TODO dynamic
}

void Ship::update(const GameTime &gameTime)
{
    if (m_target != NULL) {
        // move towards target
        m_direction = m_target->position() - m_position;
        m_direction.normalize();
        m_position += m_direction * (m_speed * gameTime.elapsedGameTimeSeconds());
        if ((m_target->position() - m_position).length() <= m_target->radius()) {
            m_target->setResources(m_target->resources() + m_resources);
            m_resources = 0;
            m_target = NULL;
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
    painter.drawText(m_position.toPoint(), QString::number(m_resources));
}
