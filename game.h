#ifndef GAME_H
#define GAME_H

#include <QObject>
#include <QSet>
#include <QTimer>
#include <QElapsedTimer>

#include "gametime.h"

QT_BEGIN_NAMESPACE
class QScriptEngine;
class QScriptEngineDebugger;
QT_END_NAMESPACE

class Player;
class Planet;

class Game : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int playerCount READ playerCount)
    Q_PROPERTY(int planetCount READ planetCount)

public:
    static Game *s_instance; // singleton
    inline static Game* instance() { return s_instance; }

    explicit Game(QObject *parent = NULL);
    ~Game();

    enum Mode {
        EditorMode,
        GameMode
    };
    inline Mode mode() const { return m_mode; }
    inline const QString& modeString() const { return ModeStrings[m_mode]; }
    static inline const QString& modeString(Mode mode) { return ModeStrings[mode]; }
    void setMode(Mode mode);

    void setUpdateLimit(int fps = 60);
    void setUpdateInterval(int msec = 16);
    inline bool isRunning() const { return m_timer.isActive(); }
    inline GameTime& gameTime() { return m_gameTime; }
    inline int fps() const { return m_fps; }

    inline QSet<Player*>& players() { return m_players; }
    inline int playerCount() const { return m_players.count(); }
    void addPlayer(Player *player);
    bool removePlayer(Player *player);
    void clearPlayers();

    inline QSet<Planet*>& planets() { return m_planets; }
    inline int planetCount() const { return m_planets.count(); }
    void addPlanet(Planet *planet);
    Planet* addPlanet(const QVector2D &position, qreal radius = 50.0, qreal resources = 0.0);
    Planet* addPlanet(qreal xpos, qreal ypos, qreal radius = 50.0, qreal resources = 0.0);
    void removePlanet(Planet *planet);
    void clearPlanets();

    inline QScriptEngine* scriptEngine() const { return m_scriptEngine; }
    inline QScriptEngineDebugger* scriptEngineDebugger() const { return m_scriptEngineDebugger; }

    // script helpers
    Q_INVOKABLE QSet<Player*> getPlayers() const { return m_players; }
    Q_INVOKABLE Player* getRandomPlayer() const;

    Q_INVOKABLE QSet<Planet*> getPlanets() const { return m_planets; }
    Q_INVOKABLE Planet* getRandomPlanet() const;

signals:
    void updated(); // game logic updated
    void modeChanged();
    void playerAdded(Player *player);
    void playerRemoved(Player *player);
    void playerChanged(Player *player); // TODO: emit signal
    void planetAdded(Planet *planet);
    void planetRemoved(Planet *planet);
    void planetChanged(Planet *planet); // TODO: emit signal

public slots:
    void startGameLoop();
    void stopGameLoop();

private slots:
    void timer_timeout();

private:
    static const QString ModeStrings[];
    Mode m_mode;

    QTimer m_timer;
    GameTime m_gameTime;
    QElapsedTimer m_fpsTimer;
    int m_fpsCounter;
    int m_fps;

    QSet<Player*> m_players;
    QSet<Planet*> m_planets;

    QScriptEngine *m_scriptEngine;
    QScriptEngineDebugger *m_scriptEngineDebugger;

};

#endif // GAME_H
