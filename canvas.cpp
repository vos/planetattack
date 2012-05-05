#include "canvas.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include <QDebug>

#include "humanplayer.h"
#include "computerplayer.h"
#include "random.h"

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

    m_mode = EditorMode;

    m_FPSCounter = 0;
    m_FPS = 60;

    m_backgroundImage = QImage("background.jpg");

    m_localPlayer = new HumanPlayer("Alex", Qt::blue, this);
    m_players.insert(m_localPlayer);
    ComputerPlayer *computerPlayer = new ComputerPlayer("GLaDOS", Qt::red, NULL, this);
    Planet *computerPlanet = new Planet(QVector2D(750, 250), 100, 100, computerPlayer->color(), computerPlayer);
    computerPlayer->planets().insert(computerPlanet);
    m_players.insert(computerPlayer);

    Random::init();

    m_gameTime.start();
    m_FPSTimer.start();

    startTimer(int(1000.0 / 60)); // start the "game loop"
}

void Canvas::timerEvent(QTimerEvent *timerEvent)
{
    if (m_mode == GameMode) {
        foreach (Player *player, m_players) {
            if (player->isComputer()) {
                ((ComputerPlayer*)player)->intelligence()->think(m_gameTime);
            }
            foreach (Planet *planet, player->planets()) {
                planet->update(m_gameTime);
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

    // players
    foreach (Player *player, m_players) {
        foreach (Planet *planet, player->planets()) {
            planet->draw(m_painter);
        }

        // target
        if (player->target() != NULL) {
            m_painter.setPen(Qt::transparent);
            QRadialGradient radialGradient(player->target()->position().toPoint(), player->target()->radius());
            radialGradient.setColorAt(0.8, Qt::transparent);
            radialGradient.setColorAt(0.9, Qt::red);
            radialGradient.setColorAt(1.0, Qt::transparent);
            m_painter.setBrush(radialGradient);
            m_painter.drawEllipse(player->target()->rect());
        }
    }

    // ships
    foreach (Player *player, m_players) {
        foreach (Planet *planet, player->planets()) {
            foreach (Ship *ship, planet->ships()) {
                ship->draw(m_painter);
            }
        }
    }

    // status overlay
    int shipCount = 0;
    foreach (Planet *planet, m_localPlayer->planets()) {
        shipCount += planet->ships().count();
    }
    QString statusText = QString("Mode = %1\n"
                                 "Game Time = %2 (%3) ms\n"
                                 "FPS = %4\n"
                                 "Planet Count = %5 (%6)\n"
                                 "Ship Count = %7")
            .arg(ModeStrings[m_mode])
            .arg(m_gameTime.totalGameTime()).arg(m_gameTime.elapsedGameTime())
            .arg(m_FPS)
            .arg(m_localPlayer->planets().count()).arg(m_localPlayer->selectedPlanets().count())
            .arg(shipCount);
    QFontMetrics fontMetrics = m_painter.fontMetrics();
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
            foreach (Planet *planet, m_localPlayer->selectedPlanets()) {
                if (planet == m_localPlayer->target()) {
                    m_localPlayer->setTarget(NULL);
                }
                m_localPlayer->selectedPlanets().remove(planet);
                m_localPlayer->planets().remove(planet);
                delete planet;
            }
        }
        break;
    }
}

void Canvas::mousePressEvent(QMouseEvent *mouseEvent)
{
    QSet<Planet*> &selectedPlanets = m_localPlayer->selectedPlanets();
    if (mouseEvent->button() == Qt::LeftButton) {
        if (!(mouseEvent->modifiers() & Qt::ControlModifier)) {
            m_localPlayer->selectedPlanets().clear();
        }
        foreach (Planet *planet, m_localPlayer->planets()) {
            qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
            if (len <= planet->radius()) {
                if (selectedPlanets.contains(planet)) {
                    selectedPlanets.remove(planet);
                } else {
                    selectedPlanets.insert(planet);
                }
            }
        }
        if (m_mode == EditorMode && selectedPlanets.isEmpty()) {
            Planet *planet = new Planet(QVector2D(mouseEvent->pos()), 50, 0, m_localPlayer->color(), m_localPlayer);
            m_localPlayer->planets().insert(planet);
            selectedPlanets.insert(planet);
        }
    } else if (mouseEvent->button() == Qt::RightButton) {
        if (m_mode == GameMode) {
            m_localPlayer->setTarget(NULL);
            if (!selectedPlanets.isEmpty()) {
                foreach (Player *player, m_players) {
                    foreach (Planet *planet, player->planets()) {
                        qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                        if (len <= planet->radius()) {
                            foreach (Planet *selectedPlanet, selectedPlanets) {
                                if (selectedPlanet != planet) {
                                    int res = selectedPlanet->resources() / 2;
                                    Ship *ship = new Ship(selectedPlanet->position(), planet, res, selectedPlanet->color(), selectedPlanet);
                                    selectedPlanet->setResources(selectedPlanet->resources() - res);
                                    selectedPlanet->ships().insert(ship);
                                }
                            }
                            m_localPlayer->setTarget(planet);
                            break;
                        }
                    }
                }
            }
        }
    }
}

void Canvas::mouseDoubleClickEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        foreach (Planet *planet, m_localPlayer->planets()) {
            m_localPlayer->selectedPlanets().insert(planet);
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (m_mode == EditorMode) {
        if (mouseEvent->modifiers() & Qt::AltModifier) {
            foreach (Planet *planet, m_localPlayer->selectedPlanets()) {
                qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                planet->setRadius(int(len));
            }
        } else {
            foreach (Planet *planet, m_localPlayer->selectedPlanets()) {
                planet->setPosition(QVector2D(mouseEvent->pos()));
            }
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
}
