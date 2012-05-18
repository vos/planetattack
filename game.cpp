#include "game.h"

#include <QScriptEngine>

#include <QDebug>
#include <QScriptEngineDebugger>

#include "scriptextensions.h"

#include "computerplayer.h"
#include "planet.h"
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
    emit modeChanged();
}

void Game::setUpdateLimit(int fps)
{
    m_timer.setInterval(1000 / fps);
}

void Game::setUpdateInterval(int msec)
{
    m_timer.setInterval(msec);
}

void Game::addPlayer(Player *player)
{
    if (player == NULL) {
        qDebug("Game::addPlayer() player cannot be null");
        return;
    }
    m_players.insert(player);
    emit playerAdded(player);
}

bool Game::removePlayer(Player *player)
{
    if (player == NULL || m_players.count() <= 1) // don't remove the last player
        return false;
    m_players.remove(player);
    emit playerRemoved(player); // emit synchronous
    delete player;
    return true;
}

void Game::clearPlayers()
{
    foreach (Player *player, m_players) {
        emit playerRemoved(player); // emit synchronous
    }
    qDeleteAll(m_players);
    m_players.clear();
}

void Game::addPlanet(Planet *planet)
{
    if (planet == NULL) {
        qDebug("Game::addPlanet() planet cannot be null");
        return;
    }
    m_planets.insert(planet);
    emit planetAdded(planet);
}

Planet* Game::addPlanet(const QVector2D &position, qreal radius, qreal resources)
{
    Planet *planet = new Planet(position, radius, resources);
    addPlanet(planet);
    return planet;
}

Planet* Game::addPlanet(qreal xpos, qreal ypos, qreal radius, qreal resources)
{
    return addPlanet(QVector2D(xpos, ypos), radius, resources);
}

void Game::removePlanet(Planet *planet)
{
    if (planet == NULL)
        return;
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
    m_planets.remove(planet); // emit synchronous
    emit planetRemoved(planet);
    delete planet;
}

void Game::clearPlanets()
{
    foreach (Planet *planet, m_planets) {
        emit planetRemoved(planet); // emit synchronous
    }
    qDeleteAll(m_planets);
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
