#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QSet>

#include "planet.h"
#include "ship.h"

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged USER true)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(qreal resourceFactor READ resourceFactor WRITE setResourceFactor)

public:
    Player(const QString &name, const QColor &color, bool human = true, QObject *parent = NULL);
    virtual ~Player();

    inline const QString& name() const { return m_name; }
    void setName(const QString &name);

    inline const QColor& color() const { return m_color; }
    void setColor(const QColor &color);

    inline bool isHuman() const { return m_human; }
    inline bool isComputer() const { return !m_human; }

    inline qreal resourceFactor() const { return m_resourceFactor; }
    inline void setResourceFactor(qreal factor = 0.5) { m_resourceFactor = qBound(0.0, factor, 1.0); }

    inline QSet<Planet*>& planets() { return m_planets; }
    inline QSet<Planet*>& selectedPlanets() { return m_selectedPlanets; }

    inline Planet* target() { return m_target; }
    inline void setTarget(Planet *target) { m_target = target; }

    inline QSet<Ship*>& ships() { return m_ships; }

    void addPlanet(Planet *planet);
    Planet* addPlanet(const QVector2D& position, qreal radius = 50.0, qreal resources = 0.0);
    void removePlanet(Planet *planet);

    Ship* addShip(Planet *origin, Planet *target, qreal resourceFactor);
    Ship* addShip(Planet *origin, Planet *target);
    void removeShip(Ship *ship);

signals:
    void nameChanged(const QString &oldName, const QString &newName);

protected:
    QString m_name;
    QColor m_color;
    bool m_human;
    qreal m_resourceFactor;

    QSet<Planet*> m_planets;
    QSet<Planet*> m_selectedPlanets;
    Planet *m_target;

    QSet<Ship*> m_ships;

};

#endif // PLAYER_H
