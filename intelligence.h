#ifndef INTELLIGENCE_H
#define INTELLIGENCE_H

#include <QObject>

#include "gametime.h"

class Intelligence : public QObject
{
    Q_OBJECT

public:
    explicit Intelligence(QObject *parent = NULL);

public slots:
    virtual void think(const GameTime &gameTime) = 0;

};

#endif // INTELLIGENCE_H
