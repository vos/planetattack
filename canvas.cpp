#include "canvas.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QStatusBar>

#include <cmath>
#include <QDebug>

Canvas* Canvas::Instance = NULL;

const QString Canvas::ModeStrings[] = {
    "EditorMode",
    "GameMode"
};

Canvas::Canvas(QWidget *parent) :
    QGLWidget(parent)
{
    setWindowTitle("PlanetAttack 0.0.1 alpha - © 2012 Alexander Vos");
    Canvas::Instance = this;

    mPainter.begin(this);
    mPainter.setRenderHints(QPainter::Antialiasing |
                            QPainter::TextAntialiasing |
                            QPainter::HighQualityAntialiasing);
    mPainter.end();

    mTarget = NULL;
    mMode = EditorMode;

    mFPSCounter = 0;
    mFPS = 60;

    mBackgroundImage = QImage("background.jpg");

    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), SLOT(tick()));
    timer->start(int(1000.0 / 60));

    mGameTimer.start();
    mFrameTimer.start();
    mFPSTimer.start();
}

void Canvas::tick()
{
    if (mMode == GameMode) {
        foreach (Planet *planet, mPlanets) {
            planet->update(mGameTimer, mFrameTimer);
        }
        foreach (Ship *ship, mShips) {
            ship->update(mGameTimer, mFrameTimer);
            if (ship->target() == NULL) {
                mShips.remove(ship);
                delete ship;
            }
        }
    }
    mFPSCounter++;
    if (mFPSTimer.hasExpired(1000)) {
        mFPS = mFPSCounter;
        mFPSCounter = 0;
        mFPSTimer.restart();
    }
    update();
    mFrameTimer.restart();
}

void Canvas::keyReleaseEvent(QKeyEvent *keyEvent)
{
//    qDebug() << "keyPressEvent = " << keyEvent;
    switch (keyEvent->key()) {
    case Qt::Key_F1: {
        mMode = mMode == EditorMode ? GameMode : EditorMode;
        break;
    }
    case Qt::Key_Delete:
        if (mMode == EditorMode) {
            foreach (Planet *planet, mSelectedPlanets) {
                if (planet == mTarget) {
                    mTarget = NULL;
                }
                mSelectedPlanets.remove(planet);
                mPlanets.remove(planet);
                delete planet;
            }
        }
        break;
    }
}

void Canvas::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        if (!(mouseEvent->modifiers() & Qt::ControlModifier)) {
            mSelectedPlanets.clear();
        }
        foreach (Planet *p, mPlanets) {
            qreal len = (p->position() - QVector2D(mouseEvent->pos())).length();
            if (len <= p->radius()) {
                if (mSelectedPlanets.contains(p)) {
                    mSelectedPlanets.remove(p);
                } else {
                    mSelectedPlanets.insert(p);
                }
            }
        }
        if (mMode == EditorMode && mSelectedPlanets.isEmpty()) {
            Planet *planet = new Planet(QVector2D(mouseEvent->pos()), 50, 0);
            mPlanets.insert(planet);
            mSelectedPlanets.insert(planet);
        }
    } else if (mouseEvent->button() == Qt::RightButton) {
        if (mMode == GameMode) {
            mTarget = NULL;
            if (!mSelectedPlanets.isEmpty()) {
                foreach (Planet *p, mPlanets) {
                    qreal len = (p->position() - QVector2D(mouseEvent->pos())).length();
                    if (len <= p->radius()) {
                        foreach (Planet *selectedPlanet, mSelectedPlanets) {
                            if (selectedPlanet != p) {
                                int res = selectedPlanet->resources() / 2;
                                Ship *ship = new Ship(selectedPlanet->position(), p, res, selectedPlanet->color());
                                selectedPlanet->setResources(selectedPlanet->resources() - res);
                                mShips.insert(ship);
                            }
                        }
                        mTarget = p;
                        break;
                    }
                }
            }
        }
    }
}

void Canvas::mouseDoubleClickEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        foreach (Planet *p, mPlanets) {
            mSelectedPlanets.insert(p);
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (mMode == EditorMode) {
        if (mouseEvent->modifiers() & Qt::AltModifier) {
            foreach (Planet *planet, mSelectedPlanets) {
                qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                planet->setRadius(int(len));
            }
        } else {
            foreach (Planet *planet, mSelectedPlanets) {
                planet->setPosition(QVector2D(mouseEvent->pos()));
            }
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    //    mPlanet = NULL;
}

void Canvas::paintEvent(QPaintEvent *paintEvent)
{
    mPainter.begin(this);

    // clear canvas
    if (mMode == GameMode) {
        mPainter.drawImage(rect(), mBackgroundImage);
    } else {
        mPainter.setBrush(Qt::black);
        mPainter.drawRect(rect());
    }

    // planets
    foreach (Planet *planet, mPlanets) {
        planet->draw(mPainter);
    }
    if (mTarget != NULL) {
        mPainter.setPen(Qt::transparent);
        QRadialGradient radialGradient(mTarget->position().toPoint(), mTarget->radius());
        radialGradient.setColorAt(0.8, Qt::transparent);
        radialGradient.setColorAt(0.9, Qt::red);
        radialGradient.setColorAt(1.0, Qt::transparent);
        mPainter.setBrush(radialGradient);
        mPainter.drawEllipse(mTarget->rect());
    }

    // ships
    foreach (Ship *ship, mShips) {
        ship->draw(mPainter);
    }

    // status overlay
    QFontMetrics fontMetrics = mPainter.fontMetrics();
    QString statusText = QString("Mode = %1\nFPS = %2\nPlanet Count = %3 (%4)\nShip Count = %5")
            .arg(ModeStrings[mMode]).arg(mFPS).arg(mPlanets.count()).arg(mSelectedPlanets.count()).arg(mShips.count());
    QSize statusSize = fontMetrics.size(0, statusText);
    QRect statusBoundingRect(10, 10, statusSize.width() + 20, statusSize.height() + 10);
    mPainter.setPen(Qt::transparent);
    mPainter.setBrush(QColor(64,64,64,128));
    mPainter.drawRoundedRect(statusBoundingRect, 10, 10);
    mPainter.setPen(Qt::white);
    statusBoundingRect.translate(10, 0);
    mPainter.drawText(statusBoundingRect, Qt::AlignLeft | Qt::AlignVCenter, statusText);

    mPainter.end();
}
