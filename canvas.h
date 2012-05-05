#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>
#include <QPainter>
#include <QSet>
#include <QElapsedTimer>

#include "gametime.h"
#include "planet.h"
#include "ship.h"

class Canvas : public QGLWidget
{
    Q_OBJECT

public:
    static Canvas *Instance; // singleton

    explicit Canvas(QWidget *parent = 0);

    inline const QSet<Planet*>& selectedPlanets() const { return m_selectedPlanets; }

private:
    QPainter m_painter;

    QSet<Planet*> m_planets;
    QSet<Planet*> m_selectedPlanets;
    Planet *m_target;

    QSet<Ship*> m_ships;

    enum Mode {
        EditorMode,
        GameMode
    };
    static const QString ModeStrings[];
    Mode m_mode;

    GameTime m_gameTime;

    QElapsedTimer m_FPSTimer;
    int m_FPSCounter;
    int m_FPS;

    QImage m_backgroundImage;

    void timerEvent(QTimerEvent *timerEvent);
    void paintEvent(QPaintEvent *paintEvent);

    void keyReleaseEvent(QKeyEvent *keyEvent);

    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);

};

#endif // CANVAS_H
