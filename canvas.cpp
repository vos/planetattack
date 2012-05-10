#include "canvas.h"

#include <QPaintEvent>
#include <QPainter>
#include <QTimer>

#include <QDebug>

#include "humanplayer.h"
#include "computerplayer.h"
#include "randomplayerintelligence.h"
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

    m_factorSelectorColor = QColor(0,255,0,32);
    m_factorSelectionActive = false;
    m_backgroundImage = QImage("background.jpg");

    m_activePlayer = new HumanPlayer("Alex", Qt::blue, this);
    m_players.insert(m_activePlayer);

    // TEST AI 1
    ComputerPlayer *computerPlayer = new ComputerPlayer("GLaDOS", Qt::red, new RandomPlayerIntelligence, this);
    computerPlayer->addPlanet(QVector2D(750, 250), 100, 100);
    m_players.insert(computerPlayer);

    // TEST AI 2
    ComputerPlayer *computerPlayer2 = new ComputerPlayer("Kai", Qt::darkGreen, NULL, this);
    computerPlayer2->addPlanet(QVector2D(500, 500), 75, 75);
    computerPlayer2->addPlanet(QVector2D(650, 600), 25, 25);
    m_players.insert(computerPlayer2);

    Random::init();

    m_gameTime.start();
    m_FPSTimer.start();

    startTimer(int(1000.0 / m_FPS)); // start the "game loop"
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
                ((ComputerPlayer*)player)->intelligence()->think(m_gameTime);
            }
            foreach (Planet *planet, player->planets()) {
                planet->update(m_gameTime);
            }
            foreach (Ship *ship, player->ships()) {
                ship->update(m_gameTime);
                if (ship->target() == NULL) {
                    player->removeShip(ship);
                    delete ship;
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
        foreach (Ship *ship, player->ships()) {
            ship->draw(m_painter);
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
    m_painter.drawText(m_factorSelectorRegion, Qt::AlignCenter, QString("%1 \%").arg(m_activePlayer->resourceFactor() * 100));

    // status overlay
    QString statusText = QString("Mode = %1\n"
                                 "Game Time = %2 (%3) ms\n"
                                 "FPS = %4\n"
                                 "Active Player = %5\n"
                                 "Planet Count = %6 (%7)\n"
                                 "Ship Count = %8")
            .arg(modeString())
            .arg(m_gameTime.totalGameTime()).arg(m_gameTime.elapsedGameTime())
            .arg(m_FPS)
            .arg(m_activePlayer->name())
            .arg(m_activePlayer->planets().count()).arg(m_activePlayer->selectedPlanets().count())
            .arg(m_activePlayer->ships().count());
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
                m_activePlayer->removePlanet(planet);
                delete planet;
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
        if (mouseEvent->button() == Qt::LeftButton) {
            if (!(mouseEvent->modifiers() & Qt::ControlModifier)) {
                m_activePlayer->selectedPlanets().clear();
            }
            foreach (Planet *planet, m_activePlayer->planets()) {
                qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                if (len <= planet->radius()) {
                    if (selectedPlanets.contains(planet)) {
                        selectedPlanets.remove(planet);
                    } else {
                        selectedPlanets.insert(planet);
                        emit selectionChanged(planet); // TODO select first
                    }
                }
            }
            if (m_mode == EditorMode && selectedPlanets.isEmpty()) {
                Planet *planet = m_activePlayer->addPlanet(QVector2D(mouseEvent->pos()));
                selectedPlanets.insert(planet);
                emit selectionChanged(planet);
            }
        } else if (mouseEvent->button() == Qt::RightButton) {
            if (m_mode == GameMode) {
                m_activePlayer->setTarget(NULL);
                if (!selectedPlanets.isEmpty()) {
                    foreach (Player *player, m_players) {
                        foreach (Planet *planet, player->planets()) {
                            qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                            if (len <= planet->radius()) {
                                foreach (Planet *selectedPlanet, selectedPlanets) {
                                    if (selectedPlanet != planet) {
                                        m_activePlayer->addShip(selectedPlanet, planet);
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
}

void Canvas::mouseDoubleClickEvent(QMouseEvent *mouseEvent)
{
    if (mouseEvent->button() == Qt::LeftButton) {
        foreach (Planet *planet, m_activePlayer->planets()) {
            m_activePlayer->selectedPlanets().insert(planet);
        }
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *mouseEvent)
{
    if (m_factorSelectionActive) {
        m_activePlayer->setResourceFactor(qRound(100.0 * mouseEvent->x() / m_factorSelectorRegion.width()) / 100.0);
    } else {
        if (m_mode == EditorMode) {
            if (mouseEvent->modifiers() & Qt::AltModifier) {
                foreach (Planet *planet, m_activePlayer->selectedPlanets()) {
                    qreal len = (planet->position() - QVector2D(mouseEvent->pos())).length();
                    planet->setRadius(int(len));
                    emit selectionChanged(planet); // TODO select first
                }
            } else {
                foreach (Planet *planet, m_activePlayer->selectedPlanets()) {
                    planet->setPosition(QVector2D(mouseEvent->pos()));
                    emit selectionChanged(planet); // TODO select first
                }
            }
        }
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    m_factorSelectionActive = false;
}
