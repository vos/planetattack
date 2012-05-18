#ifndef SPACEOBJECT_H
#define SPACEOBJECT_H

#include <drawable.h>

#include <QVector2D>
#include <QColor>

class Player;

class SpaceObject : public Drawable
{
    Q_OBJECT
    Q_PROPERTY(QVector2D position READ position WRITE setPosition)
    Q_PROPERTY(qreal resources READ resources WRITE setResources)
    Q_PROPERTY(QColor color READ color WRITE setColor)
    Q_PROPERTY(Player* player READ player WRITE setPlayer)

public:
    SpaceObject(const QVector2D& position = QVector2D(), qreal resources = 0.0, const QColor &color = Qt::darkGray, Player *parent = NULL);

    inline const QVector2D& position() const { return m_position; }
    virtual void setPosition(const QVector2D &position) { m_position = position; }

    inline qreal resources() const { return m_resources; }
    virtual void setResources(qreal resources) { m_resources = resources; }
    inline qreal addResources(qreal resources) { setResources(m_resources + resources); return m_resources; }
    inline qreal subtractResources(qreal resources) { setResources(m_resources - resources); return m_resources; }

    inline const QColor& color() const { return m_color; }
    virtual void setColor(const QColor &color = Qt::darkGray) { m_color = color; }

    inline Player* player() const { return (Player*)parent(); }
    virtual bool setPlayer(Player *player);
    inline bool hasPlayer() const { return player() != NULL; }

protected:
    QVector2D m_position;
    qreal m_resources;
    QColor m_color;
    
};

#endif // SPACEOBJECT_H
