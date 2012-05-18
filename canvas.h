#ifndef CANVAS_H
#define CANVAS_H

#include <QGLWidget>
#include <QPainter>
#include <QVector2D>

class Game;
class Player;
class Planet;

class Canvas : public QGLWidget
{
    Q_OBJECT

public:
    static Canvas *s_instance; // singleton
    inline static Canvas* instance() { return s_instance; }

    explicit Canvas(QWidget *parent = NULL);

    inline Game* game() const { return m_game; }

    inline Player* activePlayer() const { return m_activePlayer; }
    inline void setActivePlayer(Player *player) { m_activePlayer = player; }

    inline bool globalAccess() const { return m_globalAccess; }
    inline void setGlobalAccess(bool access = false) { m_globalAccess = access; }

signals:
    void selectionChanged(QObject *o);

private slots:
    void game_playerRemoved(Player *player);

private:
    Game *m_game;
    QPainter m_painter;

    QRect m_factorSelectorRegion;
    QColor m_factorSelectorColor;
    bool m_factorSelectionActive;
    QImage m_backgroundImage;

    Player *m_activePlayer;
    bool m_globalAccess;

    Planet *m_selectedPlanet;
    QVector2D m_mouseClickDiff;

    void resizeEvent(QResizeEvent *resizeEvent);
    void paintEvent(QPaintEvent *paintEvent);

    void keyReleaseEvent(QKeyEvent *keyEvent);

    void mousePressEvent(QMouseEvent *mouseEvent);
    void mouseDoubleClickEvent(QMouseEvent *mouseEvent);
    void mouseMoveEvent(QMouseEvent *mouseEvent);
    void mouseReleaseEvent(QMouseEvent *mouseEvent);
    void wheelEvent(QWheelEvent *wheelEvent);

};

#endif // CANVAS_H
