#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QSet>
#include <QColor>
#include <QMetaType>
#include <QDebug>

class Game;
class Planet;
class Ship;

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
    Player(const QString &name = QString(), const QColor &color = Qt::white, bool human = true, Game *parent = NULL);
    virtual ~Player();

    inline const QString& name() const { return m_name; }
    void setName(const QString &name);

    inline const QColor& color() const { return m_color; }
    void setColor(const QColor &color);

    inline bool isHuman() const { return m_human; }
    inline bool isComputer() const { return !m_human; }

    inline qreal resourceFactor() const { return m_resourceFactor; }
    void setResourceFactor(qreal factor = 0.5);

    inline int planetCount() const { return m_planets.count(); }
    inline QSet<Planet*>& planets() { return m_planets; }
    void addPlanet(Planet *planet);
    Planet* addPlanet(const QVector2D &position, qreal radius = 50.0, qreal resources = 0.0);
    void removePlanet(Planet *planet);

    inline QSet<Planet*>& selectedPlanets() { return m_selectedPlanets; }

    inline Planet* target() { return m_target; }
    inline void setTarget(Planet *target) { m_target = target; }

    inline int shipCount() const { return m_ships.count(); }
    inline QSet<Ship*>& ships() { return m_ships; }
    void addShip(Ship *ship);
    void removeShip(Ship *ship);

    // script helpers and functions
    Q_INVOKABLE QSet<Planet*> getPlanets() const { return m_planets; }
    Q_INVOKABLE Planet* getRandomPlanet() const;

    Q_INVOKABLE QSet<Ship*> getShips() const { return m_ships; }
    Q_INVOKABLE Ship* getRandomShip() const;

    Q_INVOKABLE QSet<Player*> getEnemies() const;
    Q_INVOKABLE Player* getRandomEnemy() const;
    Q_INVOKABLE QSet<Planet*> getEnemyPlanets() const;
    Q_INVOKABLE Planet* getRandomEnemyPlanet() const;
    Q_INVOKABLE QSet<Planet*> getOtherPlanets() const;
    Q_INVOKABLE Planet* getRandomOtherPlanet() const;

    enum ChangeType {
        NameChange,
        ColorChange,
        ResourceFactorChange
    };

#ifndef QT_NO_DATASTREAM
    friend QDataStream& operator<<(QDataStream &stream, const Player &player);
    friend QDataStream& operator>>(QDataStream &stream, Player &player);
#endif

signals:
    void changed(Player::ChangeType changeType);
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

    inline Game* game() const { return (Game*)parent(); }

};

#ifndef QT_NO_DEBUG_STREAM
QDebug operator<<(QDebug, const Player &player);
#endif

#ifndef QT_NO_DATASTREAM
QDataStream& operator<<(QDataStream &stream, const Player &player);
QDataStream& operator>>(QDataStream &stream, Player &player);
#endif

Q_DECLARE_METATYPE(Player*)
Q_DECLARE_METATYPE(QSet<Player*>)

#endif // PLAYER_H
