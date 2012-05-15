#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>
#include <QPainter>
#include <QSet>
#include <QElapsedTimer>

#include "gametime.h"
#include "player.h"

QT_BEGIN_NAMESPACE
class QScriptEngine;
class QScriptEngineDebugger;
QT_END_NAMESPACE

class Canvas : public QGLWidget
{
    Q_OBJECT
    Q_PROPERTY(int playerCount READ playerCount)
    Q_PROPERTY(int planetCount READ planetCount)

public:
    static Canvas *Instance; // singleton

    explicit Canvas(QWidget *parent = NULL);
    ~Canvas();

    enum Mode {
        EditorMode,
        GameMode
    };
    inline Mode mode() const { return m_mode; }
    inline const QString& modeString() const { return ModeStrings[m_mode]; }
    static inline const QString& modeString(Mode mode) { return ModeStrings[mode]; }
    inline void setMode(Mode mode) { if (mode == m_mode) return; m_mode = mode; emit modeChanged(); }

    inline GameTime& gameTime() { return m_gameTime; }

    inline QSet<Player*>& players() { return m_players; }
    inline Player* activePlayer() const { return m_activePlayer; }
    inline void setActivePlayer(Player *player) { m_activePlayer = player; }

    inline bool globalAccess() const { return m_globalAccess; }
    inline void setGlobalAccess(bool access = false) { m_globalAccess = access; }

    inline QSet<Planet*>& planets() { return m_planets; }

    QScriptEngine* scriptEngine() { return m_scriptEngine; }
    QScriptEngineDebugger* scriptEngineDebugger() { return m_scriptEngineDebugger; }

    inline int playerCount() const { return m_players.count(); }
    Q_INVOKABLE QSet<Player*> getPlayers() { return m_players; }
    bool removePlayer(Player *player);
    Q_INVOKABLE Player* getRandomPlayer();

    inline int planetCount() const { return m_planets.count(); }
    Q_INVOKABLE QSet<Planet*> getPlanets() { return m_planets; }
    Q_INVOKABLE void addPlanet(Planet *planet);
    Q_INVOKABLE Planet* addPlanet(const QVector2D &position, qreal radius = 50.0, qreal resources = 0.0);
    Q_INVOKABLE Planet* addPlanet(qreal xpos, qreal ypos, qreal radius = 50.0, qreal resources = 0.0);
    Q_INVOKABLE void removePlanet(Planet *planet);
    Q_INVOKABLE Planet* getRandomPlanet();

signals:
    void modeChanged();
    void selectionChanged(QObject *o);

private:
    static const QString ModeStrings[];

    Mode m_mode;
    QPainter m_painter;

    GameTime m_gameTime;

    QElapsedTimer m_FPSTimer;
    int m_FPSCounter;
    int m_FPS;

    QRect m_factorSelectorRegion;
    QColor m_factorSelectorColor;
    bool m_factorSelectionActive;
    QImage m_backgroundImage;

    QSet<Player*> m_players;
    Player *m_activePlayer;
    bool m_globalAccess;

    QSet<Planet*> m_planets;
    Planet *m_selectedPlanet;
    QVector2D m_mouseClickDiff;

    QScriptEngine *m_scriptEngine;
    QScriptEngineDebugger *m_scriptEngineDebugger;

    void resizeEvent(QResizeEvent *resizeEvent);
    void timerEvent(QTimerEvent *timerEvent);
    void paintEvent(QPaintEvent *paintEvent);

    void keyReleaseEvent(QKeyEvent *keyEvent);

    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);

};

#endif // CANVAS_H
