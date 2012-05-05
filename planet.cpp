#include "planet.h"
#include "canvas.h"

#include <QRadialGradient>

Planet::Planet(const QVector2D& position, int radius, int resources, const QColor &color, QObject *parent) :
    SpaceObject(position, resources, color, parent)
{
    m_radius = radius;
    m_timer.start();
}

QRect Planet::rect() const
{
    QPoint size(m_radius, m_radius);
    QPoint position = m_position.toPoint();
    return QRect(position - size, position + size);
}

void Planet::update(const GameTime &gameTime)
{
    if (m_timer.hasExpired(1000)) {
        m_resources += m_radius;
        m_timer.restart();
    }
}

void Planet::draw(QPainter &painter)
{
    painter.setPen(Qt::transparent);
    QRadialGradient radialGradient(m_position.toPoint(), m_radius);
    QColor color = Canvas::Instance->selectedPlanets().contains(this) ? Qt::green : m_color; // TODO extract selectedColor
    radialGradient.setColorAt(0.0, color);
    radialGradient.setColorAt(0.9, color.darker(300));
    radialGradient.setColorAt(1.0, Qt::transparent);
    painter.setBrush(radialGradient);
    QRect boundingRect = rect();
    painter.drawEllipse(boundingRect);

    QString text = QString::number(m_resources);
    painter.setPen(Qt::white);
    painter.drawText(boundingRect, Qt::AlignCenter, text);
}
