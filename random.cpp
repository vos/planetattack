#include "random.h"

#include <QTime>

void Random::init()
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
}

int Random::randomInt(int from, int to)
{
    return from + (qrand() % (to - from));
}
