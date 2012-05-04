#ifndef DRAWABLE_H
#define DRAWABLE_H

#include <QObject>

#include <QElapsedTimer>
#include <QPainter>

class Drawable : public QObject
{
    Q_OBJECT

public:
    explicit Drawable(QObject *parent = NULL);

public slots:
    virtual inline void update(const QElapsedTimer &gameTimer, const QElapsedTimer &frameTimer) { }
    virtual void draw(QPainter &painter) = 0;
    
};

#endif // DRAWABLE_H
