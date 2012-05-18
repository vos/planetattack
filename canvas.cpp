#include "canvas.h"

#include <QPaintEvent>
#include <QPainter>

#include <QDebug>

#include "game.h"
#include "humanplayer.h"
#include "planet.h"
#include "ship.h"

Canvas* Canvas::s_instance = NULL;

Canvas::Canvas(QWidget *parent) :
    QGLWidget(parent)
{
    setWindowTitle("PlanetAttack 0.2.1 alpha - © 2012 Alexander Vos");
    Canvas::s_instance = this;

    m_game = new Game(this);
    connect(m_game, SIGNAL(updated()), SLOT(repaint()));
    connect(m_game, SIGNAL(playerRemoved(Player*)), SLOT(game_playerRemoved(Player*)), Qt::DirectConnection);
    
    m_painter.begin(this);
    m_painter.setRenderHints(QPainter::Antialiasing |
                             QPainter::TextAntialiasing |
                             QPainter::HighQualityAntialiasing);
    m_painter.end();
    
    m_factorSelectorColor = QColor(0,255,0,32);
    m_factorSelectionActive = false;
    m_backgroundImage = QImage("background.jpg");
    
    m_activePlayer = new HumanPlayer("Alex", Qt::blue, m_game);
    m_game->addPlayer(m_activePlayer);
    
    m_globalAccess = false;
    m_selectedPlanet = NULL;
    
    m_game->startGameLoop();
}

void Canvas::resizeEvent(QResizeEvent *resizeEvent)
{
    m_factorSelectorRegion = QRect(0, resizeEvent->size().height() - 32, resizeEvent->size().width(), 32);
}

void Canvas::paintEvent(QPaintEvent *paintEvent)
{
    Q_UNUSED(paintEvent)
    m_painter.begin(this);

    // clear canvas
    if (m_game->mode() == Game::GameMode) {
        m_painter.drawImage(rect(), m_backgroundImage);
    } else {
        m_painter.setBrush(Qt::black);
        m_painter.drawRect(rect());
    }

    // planets
    foreach (Planet *planet, m_game->planets()) {
        planet->draw(m_painter);
    }

    // target
    if (m_activePlayer->target() != NULL) {
        m_painter.setPen(Qt::transparent);
        QRadialGradient radialGradient(m_activePlayer->target()->position().toPoint(), m_activePlayer->target()->radius() * 0.85);
        radialGradient.setColorAt(0.8, Qt::transparent);
        radialGradient.setColorAt(0.9, Qt::red);
        radialGradient.setColorAt(1.0, Qt::transparent);
        m_painter.setBrush(radialGradient);
        m_painter.drawEllipse(m_activePlayer->target()->rect());
    }

    // ships
    int shipCount = 0;
    foreach (Player *player, m_game->players()) {
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
            .arg(m_game->modeString())
            .arg(m_game->gameTime().total()).arg(m_game->gameTime().elapsed())
            .arg(m_game->fps())
            .arg(m_activePlayer->name())
            .arg(m_activePlayer->planets().count())
            .arg(m_game->planetCount())
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
        m_game->setMode(m_game->mode() == Game::EditorMode ? Game::GameMode : Game::EditorMode);
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
        if (m_game->mode() == Game::EditorMode) {
            emit selectionChanged(NULL);
            foreach (Planet *planet, m_activePlayer->selectedPlanets()) {
                m_game->removePlanet(planet);
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
        QSet<Planet*> &planets = m_globalAccess ? m_game->planets() : m_activePlayer->planets();
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
            if (m_game->mode() == Game::EditorMode) {
                m_selectedPlanet = m_activePlayer->addPlanet(mousePos);
                selectedPlanets.insert(m_selectedPlanet);
                m_mouseClickDiff = QVector2D();
                emit selectionChanged(m_selectedPlanet);
            } else if (m_game->mode() == Game::GameMode) {
                m_activePlayer->setTarget(NULL);
                if (!selectedPlanets.isEmpty()) {
                    foreach (Planet *planet, m_game->planets()) {
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
        m_activePlayer->selectedPlanets().unite(m_globalAccess ? m_game->planets() : m_activePlayer->planets());
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (m_factorSelectionActive) {
        m_activePlayer->setResourceFactor(mouseEvent->x() / (qreal)m_factorSelectorRegion.width());
    } else {
        if (m_game->mode() == Game::EditorMode && m_selectedPlanet != NULL) {
            m_activePlayer->selectedPlanets().insert(m_selectedPlanet);
            if (mouseEvent->modifiers() & Qt::AltModifier) {
                qreal len = (m_selectedPlanet->position() - QVector2D(mouseEvent->pos())).length();
                m_selectedPlanet->setRadius(int(len));
            } else {
                m_selectedPlanet->setPosition(QVector2D(mouseEvent->pos()) + m_mouseClickDiff);
            }
            // TODO: emit planetChanged(m_selectedPlanet);
            emit selectionChanged(m_selectedPlanet);
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    m_factorSelectionActive = false;
}

void Canvas::wheelEvent(QWheelEvent *wheelEvent)
{
    qreal delta = wheelEvent->delta() > 0 ? 0.1 : -0.1;
    qreal factor = m_activePlayer->resourceFactor() + delta;
    m_activePlayer->setResourceFactor(factor);
}


void Canvas::game_playerRemoved(Player *player)
{
    if (player == m_activePlayer) {
        foreach (Player *p, m_game->players()) {
            if (p != m_activePlayer) {
                m_activePlayer = p;
                break;
            }
        }
    }
}
