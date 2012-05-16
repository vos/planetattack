#include "planet.h"

#include <QRadialGradient>

#include "player.h"
#include "canvas.h"

Planet::Planet(const QVector2D& position, qreal radius, qreal resources, const QColor &color, Player *parent) :
    SpaceObject(position, resources, color, parent)
{
    m_radius = radius;
    m_productionFactor = 0.1; // default factor
    m_angle = 0.0;
    m_velocity = -90.0; // degrees per second
}

bool Planet::setPlayer(Player *p)
{
    if (p == player())
        return false;
    if (hasPlayer())
        player()->removePlanet(this);
    if (p != NULL)
        p->addPlanet(this);
    else {
        setParent(NULL);
        setColor();
    }
    return true;
}

Ship* Planet::transferResourcesTo(Planet *target, qreal resourceFactor)
{
    if (target == NULL) {
        qDebug("Planet::transferResourcesTo() target cannot be null");
        return NULL;
    }
    Player *owner = player();
    if (owner == NULL)
        return NULL; // cannot send a ship without a player
    if (resourceFactor < 0.0)
        resourceFactor = owner->resourceFactor();
    int res = m_resources * resourceFactor;
    if (res <= 0)
        return NULL;
    m_resources -= res;
    Ship *ship = new Ship(m_position, target, res, m_color, owner);
    owner->addShip(ship);
    return ship;
}

Ship* Planet::transferResourcesTo(Planet *target)
{
    return transferResourcesTo(target, -1.0);
}

QRect Planet::rect() const
{
    QPoint size(m_radius, m_radius);
    QPoint position = m_position.toPoint();
    return QRect(position - size, position + size);
}

void Planet::update(const GameTime &gameTime)
{
    if (!isNeutral()) {
        m_resources += m_radius * m_productionFactor * gameTime.elapsedSeconds();
    }
    m_angle += m_velocity * m_productionFactor * gameTime.elapsedSeconds();
    if (m_angle > 360) m_angle = 0.0;
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

    if (Canvas::Instance->activePlayer()->selectedPlanets().contains(this)) {
        QRadialGradient selectionGradient(m_position.toPoint(), m_radius);
        selectionGradient.setColorAt(0.8, Qt::transparent);
        selectionGradient.setColorAt(0.9, m_color.lighter());
        selectionGradient.setColorAt(1.0, Qt::transparent);
        painter.setBrush(selectionGradient);
        painter.drawPie(boundingRect, m_angle / 360 * 5760, 5600);
        selectionGradient.setColorAt(0.9, m_color.darker(150));
        painter.setBrush(selectionGradient);
        painter.drawPie(boundingRect, m_angle / 360 * 5760 - 160, 160);
    }

    QString text = QString("%1\n%2").arg(hasPlayer() ? player()->name() : "<neutral>").arg(int(m_resources));
    painter.setPen(Qt::white);
    painter.drawText(boundingRect, Qt::AlignCenter, text);
}
