#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QObject>
#include <QPainter>

#include "gametime.h"

class Drawable : public QObject
{
    Q_OBJECT

public:
    explicit Drawable(QObject *parent = NULL);

public slots:
    virtual inline void update(const GameTime &gameTime) { Q_UNUSED(gameTime) }
    virtual void draw(QPainter &painter) = 0;
    
};

#endif // DRAWABLE_H
