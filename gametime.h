#ifndef GAMETIME_H
#define GAMETIME_H

#include <QObject>
#include <QElapsedTimer>
#include <QMetaType>

class GameTime : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 total READ total)
    Q_PROPERTY(qint64 elapsed READ elapsed)
    Q_PROPERTY(qreal elapsedSeconds READ elapsedSeconds)

public:
    explicit GameTime(QObject *parent = NULL);
    GameTime(const GameTime &other);
    GameTime& operator =(const GameTime &other);

    inline qint64 total() const { return m_total; }
    inline qint64 elapsed() const { return m_elapsed; }
    inline qreal elapsedSeconds() const { return m_elapsedSeconds; }

public slots:
    void start();
    void update();

private:
    QElapsedTimer m_gameTimer;

    qint64 m_total; // milliseconds since the start
    qint64 m_elapsed; // milliseconds since the last update
    qreal m_elapsedSeconds; // seconds since the last update (temp)
};

Q_DECLARE_METATYPE(GameTime*)

#endif // GAMETIME_H
