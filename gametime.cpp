#include "gametime.h"

GameTime::GameTime(QObject *parent) :
    QObject(parent)
{
    m_total = 0;
    m_elapsed = 0;
    m_elapsedSeconds = 0.0;
}

GameTime::GameTime(const GameTime &other)
{
    m_gameTimer = other.m_gameTimer;
    m_total = other.m_total;
    m_elapsed = other.m_elapsed;
    m_elapsedSeconds = other.m_elapsedSeconds;
}

GameTime &GameTime::operator =(const GameTime &other)
{
    if (&other != this) {
        m_gameTimer = other.m_gameTimer;
        m_total = other.m_total;
        m_elapsed = other.m_elapsed;
        m_elapsedSeconds = other.m_elapsedSeconds;
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
    m_elapsed = elapsed - m_total;
    m_elapsedSeconds = m_elapsed / 1000.0;
    m_total = elapsed;
}
