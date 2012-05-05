#include "canvas.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

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

    m_painter.begin(this);
    m_painter.setRenderHints(QPainter::Antialiasing |
                            QPainter::TextAntialiasing |
                            QPainter::HighQualityAntialiasing);
    m_painter.end();

    m_target = NULL;
    m_mode = EditorMode;

    m_FPSCounter = 0;
    m_FPS = 60;

    m_backgroundImage = QImage("background.jpg");

    m_gameTime.start();
    m_FPSTimer.start();

    startTimer(int(1000.0 / 60)); // start the "game loop"
}

void Canvas::timerEvent(QTimerEvent *timerEvent)
{
    if (m_mode == GameMode) {
        foreach (Planet *planet, m_planets) {
            planet->update(m_gameTime);
        }
        foreach (Ship *ship, m_ships) {
            ship->update(m_gameTime);
            if (ship->target() == NULL) {
                m_ships.remove(ship);
                delete ship;
            }
        }
    }

    m_FPSCounter++;
    if (m_FPSTimer.hasExpired(1000)) {
        m_FPS = m_FPSCounter;
        m_FPSCounter = 0;
        m_FPSTimer.restart();
    }

    update();
    m_gameTime.update();
}

void Canvas::paintEvent(QPaintEvent *paintEvent)
{
    m_painter.begin(this);

    // clear canvas
    if (m_mode == GameMode) {
        m_painter.drawImage(rect(), m_backgroundImage);
    } else {
        m_painter.setBrush(Qt::black);
        m_painter.drawRect(rect());
    }

    // planets
    foreach (Planet *planet, m_planets) {
        planet->draw(m_painter);
    }
    // target
    if (m_target != NULL) {
        m_painter.setPen(Qt::transparent);
        QRadialGradient radialGradient(m_target->position().toPoint(), m_target->radius());
        radialGradient.setColorAt(0.8, Qt::transparent);
        radialGradient.setColorAt(0.9, Qt::red);
        radialGradient.setColorAt(1.0, Qt::transparent);
        m_painter.setBrush(radialGradient);
        m_painter.drawEllipse(m_target->rect());
    }

    // ships
    foreach (Ship *ship, m_ships) {
        ship->draw(m_painter);
    }

    // status overlay
    QFontMetrics fontMetrics = m_painter.fontMetrics();
    QString statusText = QString("Mode = %1\n"
                                 "Game Time = %2 (%3) ms\n"
                                 "FPS = %4\n"
                                 "Planet Count = %5 (%6)\n"
                                 "Ship Count = %7")
            .arg(ModeStrings[m_mode])
            .arg(m_gameTime.totalGameTime()).arg(m_gameTime.elapsedGameTime())
            .arg(m_FPS)
            .arg(m_planets.count()).arg(m_selectedPlanets.count())
            .arg(m_ships.count());
    QSize statusSize = fontMetrics.size(0, statusText);
    QRect statusBoundingRect(10, 10, statusSize.width() + 20, statusSize.height() + 10);
    m_painter.setPen(Qt::transparent);
    m_painter.setBrush(QColor(64,64,64,128));
    m_painter.drawRoundedRect(statusBoundingRect, 10, 10);
    m_painter.setPen(Qt::white);
    statusBoundingRect.translate(10, 0);
    m_painter.drawText(statusBoundingRect, Qt::AlignLeft | Qt::AlignVCenter, statusText);

    m_painter.end();
}


void Canvas::keyReleaseEvent(QKeyEvent *keyEvent)
{
//    qDebug() << "keyPressEvent = " << keyEvent;
    switch (keyEvent->key()) {
    case Qt::Key_F1: {
        m_mode = m_mode == EditorMode ? GameMode : EditorMode;
        break;
    }
    case Qt::Key_Delete:
        if (m_mode == EditorMode) {
            foreach (Planet *planet, m_selectedPlanets) {
                if (planet == m_target) {
                    m_target = NULL;
                }
                m_selectedPlanets.remove(planet);
                m_planets.remove(planet);
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
            m_selectedPlanets.clear();
        }
        foreach (Planet *p, m_planets) {
            qreal len = (p->position() - QVector2D(mouseEvent->pos())).length();
            if (len <= p->radius()) {
                if (m_selectedPlanets.contains(p)) {
                    m_selectedPlanets.remove(p);
                } else {
                    m_selectedPlanets.insert(p);
                }
            }
        }
        if (m_mode == EditorMode && m_selectedPlanets.isEmpty()) {
            Planet *planet = new Planet(QVector2D(mouseEvent->pos()), 50, 0);
            m_planets.insert(planet);
            m_selectedPlanets.insert(planet);
        }
    } else if (mouseEvent->button() == Qt::RightButton) {
        if (m_mode == GameMode) {
            m_target = NULL;
            if (!m_selectedPlanets.isEmpty()) {
                foreach (Planet *p, m_planets) {
                    qreal len = (p->position() - QVector2D(mouseEvent->pos())).length();
                    if (len <= p->radius()) {
                        foreach (Planet *selectedPlanet, m_selectedPlanets) {
                            if (selectedPlanet != p) {
                                int res = selectedPlanet->resources() / 2;
                                Ship *ship = new Ship(selectedPlanet->position(), p, res, selectedPlanet->color());
                                selectedPlanet->setResources(selectedPlanet->resources() - res);
                                m_ships.insert(ship);
                            }
                        }
                        m_target = p;
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
        foreach (Planet *p, m_planets) {
            m_selectedPlanets.insert(p);
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (m_mode == EditorMode) {
        if (mouseEvent->modifiers() & Qt::AltModifier) {
            foreach (Planet *planet, m_selectedPlanets) {
                qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                planet->setRadius(int(len));
            }
        } else {
            foreach (Planet *planet, m_selectedPlanets) {
                planet->setPosition(QVector2D(mouseEvent->pos()));
            }
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    //    mPlanet = NULL;
}
