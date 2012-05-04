#include "planet.h"
#include "canvas.h"

#include <QRadialGradient>

Planet::Planet(const QVector2D& position, int radius, int resources, const QColor &color, QObject *parent) :
    SpaceObject(position, resources, color, parent)
{
    mRadius = radius;
    mTimer.start();
}

QRect Planet::rect() const
{
    QPoint size(mRadius, mRadius);
    QPoint position = mPosition.toPoint();
    return QRect(position - size, position + size);
}

void Planet::update(const QElapsedTimer &gameTimer, const QElapsedTimer &frameTimer)
{
    if (mTimer.hasExpired(1000)) {
        mResources += mRadius;
        mTimer.restart();
    }
}

void Planet::draw(QPainter &painter)
{
    painter.setPen(Qt::transparent);
    QRadialGradient radialGradient(mPosition.toPoint(), mRadius);
    QColor color = Canvas::Instance->selectedPlanets().contains(this) ? Qt::green : mColor; // TODO extract selectedColor
    radialGradient.setColorAt(0.0, color);
    radialGradient.setColorAt(0.9, color.darker(300));
    radialGradient.setColorAt(1.0, Qt::transparent);
    painter.setBrush(radialGradient);
    QRect boundingRect = rect();
    painter.drawEllipse(boundingRect);

    QString text = QString::number(mResources);
    painter.setPen(Qt::white);
    painter.drawText(boundingRect, Qt::AlignCenter, text);
}
