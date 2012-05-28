#ifndef SHIP_H
#define SHIP_H

#include "spaceobject.h"
#include <QMetaType>
#include <QSet>

class Planet;

class Ship : public SpaceObject
{
    Q_OBJECT
    Q_PROPERTY(Planet* target READ target WRITE setTarget)
    Q_PROPERTY(qreal speed READ speed WRITE setSpeed)

public:
    Ship();
    Ship(const QVector2D& position, Planet *target, qreal resources, const QColor &color, Player *parent = NULL);

    inline Planet* target() const { return m_target; }
    void setTarget(Planet *target);

    inline qreal speed() const { return m_speed; }
    inline void setSpeed(qreal speed) { m_speed = speed; }

#ifndef QT_NO_DATASTREAM
    friend QDataStream& operator<<(QDataStream &stream, const Ship &ship);
    friend QDataStream& operator>>(QDataStream &stream, Ship &ship);
#endif

public slots:
    void update(const GameTime &gameTime);
    void draw(QPainter &painter);

private:
    Planet *m_target;
    qreal m_speed; // pixels per second
    QVector2D m_direction; // temp
    
};

#ifndef QT_NO_DATASTREAM
QDataStream& operator<<(QDataStream &stream, const Ship &ship);
QDataStream& operator>>(QDataStream &stream, Ship &ship);
#endif

Q_DECLARE_METATYPE(Ship*)
Q_DECLARE_METATYPE(QSet<Ship*>)

#endif // SHIP_H
