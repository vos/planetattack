#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>
#include <QPainter>
#include <QSet>
#include <QElapsedTimer>

#include "gametime.h"
#include "player.h"

class Canvas : public QGLWidget
{
    Q_OBJECT

public:
    static Canvas *Instance; // singleton

    explicit Canvas(QWidget *parent = NULL);

    enum Mode {
        EditorMode,
        GameMode
    };
    inline Mode mode() const { return m_mode; }
    inline const QString& modeString() const { return ModeStrings[m_mode]; }
    static inline const QString& modeString(Mode mode) { return ModeStrings[mode]; }
    inline void setMode(Mode mode) { if (mode == m_mode) return; m_mode = mode; emit modeChanged(); }

    inline QSet<Player*>& players() { return m_players; }

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

    QImage m_backgroundImage;

    QSet<Player*> m_players;
    Player *m_localPlayer;

    void timerEvent(QTimerEvent *timerEvent);
    void paintEvent(QPaintEvent *paintEvent);

    void keyReleaseEvent(QKeyEvent *keyEvent);

    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);

};

#endif // CANVAS_H
