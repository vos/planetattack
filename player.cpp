#include "player.h"

#include "canvas.h"

Player::Player(const QString &name, const QColor &color, bool human, QObject *parent) :
    QObject(parent)
{
    m_name = name;
    m_color = color;
    m_human = human;
    m_resourceFactor = 0.5;
    m_target = NULL;
}

Player::~Player()
{
    qDeleteAll(m_ships);
}

void Player::setName(const QString &name)
{
    if (name == m_name)
        return;
    QString oldName = m_name;
    m_name = name;
    emit nameChanged(oldName, name);
}

void Player::setColor(const QColor &color)
{
    if (color == m_color)
        return;
    m_color = color;
    foreach (Planet *planet, m_planets) {
        planet->setColor(color);
    }
    foreach (Ship *ship, m_ships) {
        ship->setColor(color);
    }
}

void Player::addPlanet(Planet *planet)
{
    planet->setParent(this);
    planet->setColor(m_color);
    m_planets.insert(planet);
}

Planet *Player::addPlanet(const QVector2D &position, qreal radius, qreal resources)
{
    Planet *planet = new Planet(position, radius, resources, m_color, this);
    Canvas::Instance->planets().insert(planet);
    m_planets.insert(planet);
    return planet;
}

void Player::removePlanet(Planet *planet)
{
    if (planet == m_target)
        m_target = NULL;
    m_selectedPlanets.remove(planet);
    m_planets.remove(planet);
}

Ship* Player::addShip(Planet *origin, Planet *target, qreal resourceFactor)
{
    int res = origin->resources() * resourceFactor;
    if (res <= 0)
        return NULL;
    origin->subtractResources(res);
    Ship *ship = new Ship(origin->position(), target, res, origin->color(), this);
    m_ships.insert(ship);
    return ship;
}

Ship* Player::addShip(Planet *origin, Planet *target)
{
    return addShip(origin, target, m_resourceFactor);
}

void Player::removeShip(Ship *ship)
{
    m_ships.remove(ship);
}

QSet<Player*> Player::getEnemies()
{
    QSet<Player*> enemies;
    foreach (Player *player, Canvas::Instance->players()) {
        if (player != this) {
            enemies.insert(player);
        }
    }
    return enemies;
}

QSet<Planet*> Player::getEnemyPlanets()
{
    QSet<Planet*> planets;
    foreach (Player *player, getEnemies()) {
        planets.unite(player->planets());
    }
    return planets;
}

QSet<Planet*> Player::getOtherPlanets()
{
    QSet<Planet*> planets;
    foreach (Planet *planet, Canvas::Instance->planets()) {
        if (planet->player() != this) {
            planets.insert(planet);
        }
    }
    return planets;
}
