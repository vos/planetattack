#ifndef INTELLIGENCE_H
#define INTELLIGENCE_H

#include <QObject>

#include "gametime.h"

class Intelligence : public QObject
{
    Q_OBJECT

public:
    explicit Intelligence(QObject *parent = NULL);
    Intelligence(const Intelligence &);

public slots:
    virtual void think(const GameTime &gameTime) { Q_UNUSED(gameTime) } // NOTE: abstract not possible with Qt Meta System

};

#endif // INTELLIGENCE_H
