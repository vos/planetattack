#include "planet.h"

#include <QRadialGradient>

#include "player.h"

Planet::Planet(const QVector2D& position, qreal radius, qreal resources, const QColor &color, Player *parent) :
    SpaceObject(position, resources, color, parent)
{
    m_radius = radius;
    m_productionFactor = 0.1; // default factor
}

QRect Planet::rect() const
{
    QPoint size(m_radius, m_radius);
    QPoint position = m_position.toPoint();
    return QRect(position - size, position + size);
}

void Planet::update(const GameTime &gameTime)
{
    m_resources += m_radius * m_productionFactor * gameTime.elapsedGameTimeSeconds();
}

void Planet::draw(QPainter &painter)
{
    painter.setPen(Qt::transparent);
    QRadialGradient radialGradient(m_position.toPoint(), m_radius);
    radialGradient.setColorAt(0.0, m_color);
    radialGradient.setColorAt(0.9, m_color.darker(300));
    radialGradient.setColorAt(1.0, Qt::transparent);
    painter.setBrush(radialGradient);
    QRect boundingRect = rect();
    painter.drawEllipse(boundingRect);

    if (hasPlayer() && player()->selectedPlanets().contains(this)) {
        QRadialGradient selectionGradient(m_position.toPoint(), m_radius);
        selectionGradient.setColorAt(0.8, Qt::transparent);
        selectionGradient.setColorAt(0.9, m_color.lighter());
        selectionGradient.setColorAt(1.0, Qt::transparent);
        painter.setBrush(selectionGradient);
        painter.drawEllipse(boundingRect);
    }

    QString text = QString("%1\n%2").arg(hasPlayer() ? player()->name() : "<neutral>").arg(int(m_resources));
    painter.setPen(Qt::white);
    painter.drawText(boundingRect, Qt::AlignCenter, text);
}
