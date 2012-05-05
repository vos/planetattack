#ifndef GAMETIME_H
#define GAMETIME_H

#include <QObject>
#include <QElapsedTimer>

class GameTime : public QObject
{
    Q_OBJECT

public:
    explicit GameTime(QObject *parent = NULL);

    inline qint64 totalGameTime() const { return m_totalGameTime; }
    inline qint64 elapsedGameTime() const { return m_elapsedGameTime; }
    inline qreal elapsedGameTimeSeconds() const { return m_elapsedGameTimeSeconds; }

public slots:
    void start();
    void update();

private:
    QElapsedTimer m_gameTimer;

    qint64 m_totalGameTime; // milliseconds since the start
    qint64 m_elapsedGameTime; // milliseconds since the last update
    qreal m_elapsedGameTimeSeconds; // seconds since the last update (temp)
};

#endif // GAMETIME_H
