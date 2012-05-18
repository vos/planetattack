#include "player.h"

#include "game.h"
#include "planet.h"
#include "ship.h"
#include "randomutil.h"

Player::Player(const QString &name, const QColor &color, bool human, Game *parent) :
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
    foreach (Planet *planet, m_planets) {
        planet->setPlayer(NULL);
    }

    qDeleteAll(m_ships);
}

void Player::setName(const QString &name)
{
    if (name == m_name)
        return;
    QString oldName = m_name;
    m_name = name;
    emit changed(NameChange);
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
    emit changed(ColorChange);
}

void Player::setResourceFactor(qreal factor)
{
    if (factor == m_resourceFactor) // TODO: use small epsilon?
        return;
    m_resourceFactor = qBound(0.0, factor, 1.0);
    emit changed(ResourceFactorChange);
}

void Player::addPlanet(Planet *planet)
{
    if (planet == NULL) {
        qDebug("Player::addPlanet() planet cannot be null");
        return;
    }
    planet->setParent(this);
    planet->setColor(m_color);
    m_planets.insert(planet);
}

Planet *Player::addPlanet(const QVector2D &position, qreal radius, qreal resources)
{
    Planet *planet = new Planet(position, radius, resources, m_color, this);
    game()->addPlanet(planet);
    m_planets.insert(planet);
    return planet;
}

void Player::removePlanet(Planet *planet)
{
    m_selectedPlanets.remove(planet);
    m_planets.remove(planet);
}

Planet* Player::getRandomPlanet() const
{
    if (m_planets.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(m_planets);
}

void Player::addShip(Ship *ship)
{
    if (ship == NULL) {
        qDebug("Player::addShip() ship cannot be null");
        return;
    }
    m_ships.insert(ship);
    emit game()->resourcesTransferInitiated(ship);
}

void Player::removeShip(Ship *ship)
{
    m_ships.remove(ship);
    delete ship;
}

Ship* Player::getRandomShip() const
{
    if (m_ships.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(m_ships);
}

QSet<Player*> Player::getEnemies() const
{
    QSet<Player*> enemies;
    foreach (Player *player, game()->players()) {
        if (player != this) {
            enemies.insert(player);
        }
    }
    return enemies;
}

Player* Player::getRandomEnemy() const
{
    QSet<Player*> enemies = getEnemies();
    if (enemies.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(enemies);
}

QSet<Planet*> Player::getEnemyPlanets() const
{
    QSet<Planet*> planets;
    foreach (Player *player, getEnemies()) {
        planets.unite(player->planets());
    }
    return planets;
}

Planet* Player::getRandomEnemyPlanet() const
{
    QSet<Planet*> enemyPlanets = getEnemyPlanets();
    if (enemyPlanets.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(enemyPlanets);
}

QSet<Planet*> Player::getOtherPlanets() const
{
    QSet<Planet*> planets;
    foreach (Planet *planet, game()->planets()) {
        if (planet->player() != this) {
            planets.insert(planet);
        }
    }
    return planets;
}

Planet* Player::getRandomOtherPlanet() const
{
    QSet<Planet*> otherPlanets = getOtherPlanets();
    if (otherPlanets.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(otherPlanets);
}
