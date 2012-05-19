#include "game.h"

#include <QScriptEngine>

#include <QDebug>
#include <QScriptEngineDebugger>

#include "scriptextensions.h"

#include "computerplayer.h"
#include "ship.h"
#include "randomutil.h"

Game* Game::s_instance = NULL;

const QString Game::ModeStrings[] = {
    "EditorMode",
    "GameMode"
};

Game::Game(QObject *parent) :
    QObject(parent)
{
    Game::s_instance = this;
    m_mode = EditorMode;

    m_fpsCounter = 0;
    m_fps = 60;

    // scripting
    m_scriptEngine = new QScriptEngine(this);
    addScriptExtentions(this);
    m_scriptEngineDebugger = new QScriptEngineDebugger(this);
    m_scriptEngineDebugger->attachTo(m_scriptEngine);

    setUpdateLimit();
    connect(&m_timer, SIGNAL(timeout()), SLOT(timer_timeout()));
}

Game::~Game()
{
    qDeleteAll(m_players);
    qDeleteAll(m_planets);
}

void Game::setMode(Game::Mode mode)
{
    if (mode == m_mode)
        return;
    m_mode = mode;
    emit modeChanged(mode);
}

void Game::setUpdateLimit(int fps)
{
    m_timer.setInterval(1000 / fps);
}

void Game::setUpdateInterval(int msec)
{
    m_timer.setInterval(msec);
}

bool Game::addPlayer(Player *player)
{
    if (player == NULL) {
        qDebug("Game::addPlayer() player cannot be null");
        return false;
    }
    if (m_players.contains(player)) {
        qDebug("Game::addPlayer() player already added: %s", qPrintable(player->name()));
        return false;
    }
    m_players.insert(player);
    connect(player, SIGNAL(changed(Player::ChangeType)), SLOT(player_changed(Player::ChangeType)), Qt::DirectConnection);
    emit playerAdded(player);
    return true;
}

bool Game::removePlayer(Player *player)
{
    if (player == NULL || m_players.count() <= 1) // don't remove the last player
        return false;
    if (!m_players.remove(player)) {
        qDebug("Game::removePlayer() player not removed: %s", qPrintable(player->name()));
        return false;
    }
    emit playerRemoved(player); // emit synchronous
    disconnect(player);
    delete player;
    return true;
}

void Game::clearPlayers()
{
    foreach (Player *player, m_players) {
        emit playerRemoved(player); // emit synchronous
        disconnect(player);
        delete player;
    }
    m_players.clear();
}

bool Game::addPlanet(Planet *planet)
{
    if (planet == NULL) {
        qDebug("Game::addPlanet() planet cannot be null");
        return false;
    }
    if (m_planets.contains(planet)) {
        qDebug("Game::addPlanet() planet already added: position = %lf, %lf", planet->position().x(), planet->position().y());
        return false;
    }
    m_planets.insert(planet);
    connect(planet, SIGNAL(changed(Planet::ChangeType)), SLOT(planet_changed(Planet::ChangeType)), Qt::DirectConnection);
    emit planetAdded(planet);
    return true;
}

Planet* Game::addPlanet(const QVector2D &position, qreal radius, qreal resources)
{
    Planet *planet = new Planet(position, radius, resources);
    addPlanet(planet);
    return planet;
}

bool Game::removePlanet(Planet *planet)
{
    if (planet == NULL)
        return false;
    foreach (Player *player, m_players) {
        if (player->target() == planet) {
            player->setTarget(NULL);
        }
        foreach (Ship *ship, player->ships()) {
            if (ship->target() == planet) {
                player->removeShip(ship);
            }
        }
        player->removePlanet(planet);
    }
    if (!m_planets.remove(planet)) {
        qDebug("Game::removePlanet() planet not removed: position = %lf, %lf", planet->position().x(), planet->position().y());
        return false;
    }
    emit planetRemoved(planet); // emit synchronous
    disconnect(planet);
    delete planet;
    return true;
}

void Game::clearPlanets()
{
    foreach (Planet *planet, m_planets) {
        emit planetRemoved(planet); // emit synchronous
        disconnect(planet);
        delete planet;
    }
    m_planets.clear();
}

Player* Game::getRandomPlayer() const
{
    if (m_players.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(m_players);
}

Planet* Game::getRandomPlanet() const
{
    if (m_planets.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(m_planets);
}

void Game::startGameLoop()
{
    m_gameTime.start();
    m_fpsTimer.start();
    m_timer.start();
}

void Game::stopGameLoop()
{
    m_timer.stop();
    // TODO: pause game time
}

void Game::timer_timeout()
{
    if (m_mode == GameMode) {
        foreach (Planet *planet, m_planets) {
            planet->update(m_gameTime);
        }
        foreach (Player *player, m_players) {
            foreach (Ship *ship, player->ships()) {
                ship->update(m_gameTime);
                if (ship->target() == NULL) {
                    player->removeShip(ship);
                    emit resourcesTransferCompleted(ship);
                }
            }
            if (player->isComputer()) {
                ComputerPlayer *computerPlayer = static_cast<ComputerPlayer*>(player);
                computerPlayer->think(m_gameTime);
            }
        }
    }

    m_fpsCounter++;
    if (m_fpsTimer.hasExpired(1000)) {
        m_fps = m_fpsCounter;
        m_fpsCounter = 0;
        m_fpsTimer.start();
    }

    m_gameTime.update();
    emit updated();
}

void Game::player_changed(Player::ChangeType changeType)
{
    Player *player = qobject_cast<Player*>(sender());
    Q_ASSERT(player);
//    qDebug("Game::player_changed() %s -> %d", qPrintable(player->name()), changeType);
    emit playerChanged(player, changeType);
}

void Game::planet_changed(Planet::ChangeType changeType)
{
    Planet *planet = qobject_cast<Planet*>(sender());
    Q_ASSERT(planet);
//    qDebug("Game::planet_changed() %p -> %d", planet, changeType);
    emit planetChanged(planet, changeType);
}
