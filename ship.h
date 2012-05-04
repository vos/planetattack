#ifndef SHIP_H
#define SHIP_H

#include "spaceobject.h"
#include "planet.h"

class Ship : public SpaceObject
{
    Q_OBJECT

public:
    Ship(const QVector2D& position, Planet *target, int resources = 0, const QColor &color = Qt::blue, QObject *parent = NULL);

    inline const Planet* target() const { return mTarget; }

public slots:
    void update(const QElapsedTimer &gameTimer, const QElapsedTimer &frameTimer);
    void draw(QPainter &painter);

private:
    Planet *mTarget;
    int mSpeed; // pixels per second
    QVector2D mDirection; // tmp
    
};

#endif // SHIP_H
