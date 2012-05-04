#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>
#include <QPainter>
#include <QSet>
#include <QElapsedTimer>

#include "planet.h"
#include "ship.h"

class Canvas : public QGLWidget
{
    Q_OBJECT

public:
    static Canvas *Instance; // singleton

    explicit Canvas(QWidget *parent = 0);

    inline const QSet<Planet*>& selectedPlanets() const { return mSelectedPlanets; }

public slots:
    void keyReleaseEvent(QKeyEvent *keyEvent);

private slots:
    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);

    void tick();

private:
    QPainter mPainter;

    QSet<Planet*> mPlanets;
    QSet<Planet*> mSelectedPlanets;
    Planet *mTarget;

    QSet<Ship*> mShips;

    enum Mode {
        EditorMode,
        GameMode
    };
    static const QString ModeStrings[];
    Mode mMode;

    QElapsedTimer mGameTimer;
    QElapsedTimer mFrameTimer;
    QElapsedTimer mFPSTimer;
    int mFPSCounter;
    int mFPS;

    QImage mBackgroundImage;

    void paintEvent(QPaintEvent *paintEvent);

};

#endif // CANVAS_H
