#include "canvas.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>
#include <QScriptEngine>

#include <QDebug>
#include <QScriptEngineDebugger>

#include "scriptextensions.h"

#include "humanplayer.h"
#include "computerplayer.h"
#include "scriptedplayerintelligence.h"
#include "randomutil.h"

Canvas* Canvas::Instance = NULL;

const QString Canvas::ModeStrings[] = {
    "EditorMode",
    "GameMode"
};

Canvas::Canvas(QWidget *parent) :
    QGLWidget(parent)
{
    setWindowTitle("PlanetAttack 0.2 alpha - © 2012 Alexander Vos");
    Canvas::Instance = this;

    m_painter.begin(this);
    m_painter.setRenderHints(QPainter::Antialiasing |
                            QPainter::TextAntialiasing |
                            QPainter::HighQualityAntialiasing);
    m_painter.end();

    m_mode = EditorMode;

    m_FPSCounter = 0;
    m_FPS = 60;

    m_factorSelectorColor = QColor(0,255,0,32);
    m_factorSelectionActive = false;
    m_backgroundImage = QImage("background.jpg");

    m_activePlayer = new HumanPlayer("Alex", Qt::blue, this);
    m_players.insert(m_activePlayer);

    m_globalAccess = false;
    m_selectedPlanet = NULL;

    // scripting
    m_scriptEngine = new QScriptEngine(this);
    m_scriptEngineDebugger = new QScriptEngineDebugger(this);
    addScriptExtentions(this);
    m_scriptEngineDebugger->attachTo(m_scriptEngine);

    m_gameTime.start();
    m_FPSTimer.start();

    startTimer(int(1000.0 / m_FPS)); // start the "game loop"
}

Canvas::~Canvas()
{
    qDeleteAll(m_players);
    qDeleteAll(m_planets);
}

bool Canvas::removePlayer(Player *player)
{
    if (player == NULL || m_players.count() <= 1) // don't remove the last player
        return false;
    if (player == m_activePlayer) {
        foreach (Player *p, m_players) {
            if (p != m_activePlayer) {
                m_activePlayer = p;
                break;
            }
        }
    }
    m_players.remove(player);
    delete player;
    return true;
}

void Canvas::resizeEvent(QResizeEvent *resizeEvent)
{
    m_factorSelectorRegion = QRect(0, resizeEvent->size().height() - 32, resizeEvent->size().width(), 32);
}

void Canvas::timerEvent(QTimerEvent *timerEvent)
{
    Q_UNUSED(timerEvent)
    if (m_mode == GameMode) {
        foreach (Player *player, m_players) {
            if (player->isComputer()) {
                ComputerPlayer *computerPlayer = static_cast<ComputerPlayer*>(player);
                if (computerPlayer->hasIntelligence()) {
                    computerPlayer->intelligence()->think(m_gameTime);
                }
            }
            foreach (Planet *planet, player->planets()) {
                planet->update(m_gameTime);
            }
            foreach (Ship *ship, player->ships()) {
                ship->update(m_gameTime);
                if (ship->target() == NULL) {
                    player->removeShip(ship);
                }
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
    Q_UNUSED(paintEvent)
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

    // target and ships
    int shipCount = 0;
    foreach (Player *player, m_players) {
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
        // ships
        foreach (Ship *ship, player->ships()) {
            ship->draw(m_painter);
            shipCount++;
        }
    }

    // resource factor region
    m_painter.setPen(Qt::transparent);
    m_painter.setBrush(m_factorSelectorColor);
    m_painter.drawRect(m_factorSelectorRegion);
    m_painter.setBrush(m_factorSelectorColor.lighter());
    QRect factorSectionRegion(m_factorSelectorRegion);
    factorSectionRegion.setWidth(m_factorSelectorRegion.width() * m_activePlayer->resourceFactor());
    m_painter.drawRect(factorSectionRegion);
    m_painter.setPen(Qt::white);
    m_painter.drawText(m_factorSelectorRegion, Qt::AlignCenter, QString("%1 %")
                       .arg(m_activePlayer->resourceFactor() * 100, 0, 'f', 0));

    // status overlay
    QString statusText = QString("Mode = %1\n"
                                 "Game Time = %2 (%3) ms\n"
                                 "FPS = %4\n"
                                 "Active Player = %5\n"
                                 "Planet Count = %6/%7 (%8)\n"
                                 "Ship Count = %9/%10")
            .arg(modeString())
            .arg(m_gameTime.total()).arg(m_gameTime.elapsed())
            .arg(m_FPS)
            .arg(m_activePlayer->name())
            .arg(m_activePlayer->planets().count())
            .arg(m_planets.count())
            .arg(m_activePlayer->selectedPlanets().count())
            .arg(m_activePlayer->ships().count())
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
        setMode(m_mode == EditorMode ? GameMode : EditorMode);
        break;
    }
    case Qt::Key_0:
        m_activePlayer->setResourceFactor(1.0);
        break;
    case Qt::Key_1:
    case Qt::Key_2:
    case Qt::Key_3:
    case Qt::Key_4:
    case Qt::Key_5:
    case Qt::Key_6:
    case Qt::Key_7:
    case Qt::Key_8:
    case Qt::Key_9:
        m_activePlayer->setResourceFactor((keyEvent->key() - Qt::Key_0) / 10.0);
        break;
    case Qt::Key_Delete:
        if (m_mode == EditorMode) {
            emit selectionChanged(NULL);
            foreach (Planet *planet, m_activePlayer->selectedPlanets()) {
                removePlanet(planet);
            }
        }
        break;
    }
}

void Canvas::mousePressEvent(QMouseEvent *mouseEvent)
{
    if (!hasFocus())
        setFocus(); // QMainWindow focus fix
    if (m_factorSelectorRegion.contains(mouseEvent->pos())) {
        m_factorSelectionActive = true;
        mouseMoveEvent(mouseEvent);
    } else {
        QSet<Planet*> &selectedPlanets = m_activePlayer->selectedPlanets();
        QSet<Planet*> &planets = m_globalAccess ? m_planets : m_activePlayer->planets();
        QVector2D mousePos(mouseEvent->pos());
        if (mouseEvent->button() == Qt::LeftButton) {
            m_selectedPlanet = NULL;
            foreach (Planet *planet, planets) {
                QVector2D mouseDiff(planet->position() - mousePos);
                if (mouseDiff.length() <= planet->radius()) {
                    if (selectedPlanets.contains(planet)) {
                        selectedPlanets.remove(planet);
                    } else {
                        selectedPlanets.insert(planet);
                    }
                    if (m_selectedPlanet == NULL) {
                        emit selectionChanged(planet);
                        m_selectedPlanet = planet;
                        m_mouseClickDiff = mouseDiff;
                    }
                }
            }
            if (m_selectedPlanet == NULL) {
                // nothing clicked -> deselect all
                selectedPlanets.clear();
            }
        } else if (mouseEvent->button() == Qt::RightButton) {
            if (m_mode == EditorMode) {
                m_selectedPlanet = m_activePlayer->addPlanet(mousePos);
                selectedPlanets.insert(m_selectedPlanet);
                emit selectionChanged(m_selectedPlanet);
            } else if (m_mode == GameMode) {
                m_activePlayer->setTarget(NULL);
                if (!selectedPlanets.isEmpty()) {
                    foreach (Planet *planet, m_planets) {
                        qreal len = (planet->position() - mousePos).length();
                        if (len <= planet->radius()) {
                            foreach (Planet *selectedPlanet, selectedPlanets) {
                                if (selectedPlanet != planet) {
                                    selectedPlanet->transferResourcesTo(planet);
                                }
                            }
                            m_activePlayer->setTarget(planet);
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
        m_activePlayer->selectedPlanets().unite(m_globalAccess ? m_planets : m_activePlayer->planets());
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (m_factorSelectionActive) {
        m_activePlayer->setResourceFactor(mouseEvent->x() / (qreal)m_factorSelectorRegion.width());
    } else {
        if (m_mode == EditorMode && m_selectedPlanet != NULL) {
            m_activePlayer->selectedPlanets().insert(m_selectedPlanet);
            if (mouseEvent->modifiers() & Qt::AltModifier) {
                qreal len = (m_selectedPlanet->position() - QVector2D(mouseEvent->pos())).length();
                m_selectedPlanet->setRadius(int(len));
                emit selectionChanged(m_selectedPlanet);
            } else {
                m_selectedPlanet->setPosition(QVector2D(mouseEvent->pos()) + m_mouseClickDiff);
                emit selectionChanged(m_selectedPlanet);
            }
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    m_factorSelectionActive = false;
}

Player* Canvas::getRandomPlayer()
{
    if (m_players.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(m_players);
}

void Canvas::addPlanet(Planet *planet)
{
    if (planet == NULL) {
        qDebug("Canvas::addPlanet() planet cannot be null");
        return;
    }
    m_planets.insert(planet);
}

Planet* Canvas::addPlanet(const QVector2D &position, qreal radius, qreal resources)
{
    Planet *planet = new Planet(position, radius, resources);
    m_planets.insert(planet);
    return planet;
}

Planet* Canvas::addPlanet(qreal xpos, qreal ypos, qreal radius, qreal resources)
{
    return addPlanet(QVector2D(xpos, ypos), radius, resources);
}

void Canvas::removePlanet(Planet *planet)
{
    if (planet == NULL)
        return;
    foreach (Player *player, m_players) {
        if (player->target() == planet) {
            player->setTarget(NULL);
        }
        foreach (Ship *ship, player->ships()) {
            if (ship->target() == planet) {
                player->removeShip(ship);
            }
        }
        player->removePlanet(planet);
    }
    if (planet == m_selectedPlanet)
        m_selectedPlanet = NULL;
    m_planets.remove(planet);
    delete planet;
}

Planet* Canvas::getRandomPlanet()
{
    if (m_planets.isEmpty())
        return NULL;
    return *RandomUtil::randomElement(m_planets);
}
