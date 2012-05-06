#include "random.h"

#include <QTime>

void Random::init()
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
}

int Random::randomInt()
{
    return qrand();
}

int Random::randomInt(int from, int to)
{
    return from + (qrand() % (to - from));
}

qreal Random::randomReal()
{
    return qrand() / (qreal)RAND_MAX;
}

qreal Random::randomReal(qreal min, qreal max)
{
    return min + (randomReal() * (max - min));
}
