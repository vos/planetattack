#include "planet.h"

#include <QRadialGradient>

#include "player.h"

const QColor Planet::SelectedColor = Qt::green;

Planet::Planet(const QVector2D& position, qreal radius, qreal resources, const QColor &color, QObject *parent) :
    SpaceObject(position, resources, color, parent)
{
    m_radius = radius;
}

QRect Planet::rect() const
{
    QPoint size(m_radius, m_radius);
    QPoint position = m_position.toPoint();
    return QRect(position - size, position + size);
}

void Planet::update(const GameTime &gameTime)
{
    m_resources += m_radius * gameTime.elapsedGameTimeSeconds();
}

void Planet::draw(QPainter &painter)
{
    painter.setPen(Qt::transparent);
    QRadialGradient radialGradient(m_position.toPoint(), m_radius);
    Player *player = (Player*)parent();
    QColor color = player->selectedPlanets().contains(this) ? SelectedColor : m_color;
    radialGradient.setColorAt(0.0, color);
    radialGradient.setColorAt(0.9, color.darker(300));
    radialGradient.setColorAt(1.0, Qt::transparent);
    painter.setBrush(radialGradient);
    QRect boundingRect = rect();
    painter.drawEllipse(boundingRect);

    QString text = QString("%1\n%2").arg(player->name()).arg(int(m_resources));
    painter.setPen(Qt::white);
    painter.drawText(boundingRect, Qt::AlignCenter, text);
}
