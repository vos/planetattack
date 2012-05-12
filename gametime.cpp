#include "gametime.h"

GameTime::GameTime(QObject *parent) :
    QObject(parent)
{
    m_totalGameTime = 0;
    m_elapsedGameTime = 0;
    m_elapsedGameTimeSeconds = 0.0;
}

GameTime::GameTime(const GameTime &other)
{
    m_gameTimer = other.m_gameTimer;
    m_totalGameTime = other.m_totalGameTime;
    m_elapsedGameTime = other.m_elapsedGameTime;
    m_elapsedGameTimeSeconds = other.m_elapsedGameTimeSeconds;
}

GameTime &GameTime::operator =(const GameTime &other)
{
    if (&other != this) {
        m_gameTimer = other.m_gameTimer;
        m_totalGameTime = other.m_totalGameTime;
        m_elapsedGameTime = other.m_elapsedGameTime;
        m_elapsedGameTimeSeconds = other.m_elapsedGameTimeSeconds;
    }
    return *this;
}

void GameTime::start()
{
    m_gameTimer.start();
}

void GameTime::update()
{
    qint64 elapsed = m_gameTimer.elapsed();
    m_elapsedGameTime = elapsed - m_totalGameTime;
    m_elapsedGameTimeSeconds = m_elapsedGameTime / 1000.0;
    m_totalGameTime = elapsed;
}
