#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QSet>
#include <QMetaType>

#include "planet.h"
#include "ship.h"

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged USER true)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(qreal resourceFactor READ resourceFactor WRITE setResourceFactor)
    Q_PROPERTY(bool human READ isHuman DESIGNABLE false)
    Q_PROPERTY(bool computer READ isComputer DESIGNABLE false)
    Q_PROPERTY(int planetCount READ planetCount DESIGNABLE false)
    Q_PROPERTY(int shipCount READ shipCount DESIGNABLE false)

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

    inline int planetCount() const { return m_planets.count(); }
    inline QSet<Planet*>& planets() { return m_planets; }
    Q_INVOKABLE QSet<Planet*> getPlanets() { return m_planets; }
    Q_INVOKABLE void addPlanet(Planet *planet);
    Q_INVOKABLE Planet* addPlanet(const QVector2D &position, qreal radius = 50.0, qreal resources = 0.0);
    Q_INVOKABLE Planet* addPlanet(qreal xpos, qreal ypos, qreal radius = 50.0, qreal resources = 0.0);
    Q_INVOKABLE void removePlanet(Planet *planet);
    Q_INVOKABLE Planet* getRandomPlanet() const;

    inline QSet<Planet*>& selectedPlanets() { return m_selectedPlanets; }

    inline Planet* target() { return m_target; }
    inline void setTarget(Planet *target) { m_target = target; }

    inline int shipCount() const { return m_ships.count(); }
    inline QSet<Ship*>& ships() { return m_ships; }
    Q_INVOKABLE QSet<Ship*> getShips() { return m_ships; }
    Q_INVOKABLE void addShip(Ship *ship);
    Q_INVOKABLE void removeShip(Ship *ship);
    Q_INVOKABLE Ship* getRandomShip() const;

    Q_INVOKABLE QSet<Player*> getEnemies() const;
    Q_INVOKABLE Player* getRandomEnemy() const;
    Q_INVOKABLE QSet<Planet*> getEnemyPlanets() const;
    Q_INVOKABLE Planet* getRandomEnemyPlanet() const;
    Q_INVOKABLE QSet<Planet*> getOtherPlanets() const;
    Q_INVOKABLE Planet* getRandomOtherPlanet() const;

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

Q_DECLARE_METATYPE(Player*)
Q_DECLARE_METATYPE(QSet<Player*>)

#endif // PLAYER_H
