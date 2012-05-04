#include "ship.h"
#include <cmath>

Ship::Ship(const QVector2D& position, Planet *target, int resources, const QColor &color, QObject *parent) :
    SpaceObject(position, resources, color, parent)
{
    mTarget = target;
    mSpeed = 100; // TODO dynamic
}

void Ship::update(const QElapsedTimer &gameTimer, const QElapsedTimer &frameTimer)
{
    if (mTarget != NULL) {
        // move towards target
        mDirection = mTarget->position() - mPosition;
        mDirection.normalize();
        mPosition += mDirection * (mSpeed * frameTimer.elapsed() / 1000.0);
        if ((mTarget->position() - mPosition).length() <= mTarget->radius()) {
            mTarget->setResources(mTarget->resources() + mResources);
            mResources = 0;
            mTarget = NULL;
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
    qreal degrees = atan2(mDirection.y(), mDirection.x()) * (180 / M_PI) + 90; // REFAC
    matrix.rotate(degrees);
    polygon = matrix.map(polygon);
    matrix.reset();
    matrix.translate(mPosition.x(), mPosition.y());
    polygon = matrix.map(polygon);

    painter.setPen(QPen(mColor, 3, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    painter.drawPolyline(polygon);

    painter.setPen(Qt::white);
    painter.drawText(mPosition.toPoint(), QString::number(mResources));
}
