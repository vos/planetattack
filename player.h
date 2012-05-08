#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>
#include <QSet>

#include "planet.h"
#include "ship.h"

class Player : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString name READ name WRITE setName USER true)
    Q_PROPERTY(QColor color READ color WRITE setColor)

public:
    Player(const QString &name, const QColor &color, bool human = true, QObject *parent = NULL);

    inline const QString& name() const { return m_name; }
    inline void setName(const QString &name) { m_name = name; }

    inline const QColor& color() const { return m_color; }
    inline void setColor(const QColor &color) { m_color = color; }

    inline bool isHuman() const { return m_human; }
    inline bool isComputer() const { return !m_human; }

    inline QSet<Planet*>& planets() { return m_planets; }
    inline QSet<Planet*>& selectedPlanets() { return m_selectedPlanets; }

    inline Planet* target() { return m_target; }
    inline void setTarget(Planet *target) { m_target = target; }

    inline QSet<Ship*>& ships() { return m_ships; }

protected:
    QString m_name;
    QColor m_color;
    bool m_human;

    QSet<Planet*> m_planets;
    QSet<Planet*> m_selectedPlanets;
    Planet *m_target;

    QSet<Ship*> m_ships;

};

#endif // PLAYER_H
