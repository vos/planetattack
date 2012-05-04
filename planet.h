#ifndef PLANET_H
#define PLANET_H

#include "spaceobject.h"

class Planet : public SpaceObject
{
    Q_OBJECT

public:
    Planet(const QVector2D& position, int radius, int resources, const QColor &color = Qt::blue, QObject *parent = NULL);

    inline int radius() const { return mRadius; }
    inline void setRadius(int radius) { mRadius = radius; }

    QRect rect() const;

public slots:
    void update(const QElapsedTimer &gameTimer, const QElapsedTimer &frameTimer);
    void draw(QPainter &painter);

private:
    int mRadius;
    QElapsedTimer mTimer;

};

#endif // PLANET_H
