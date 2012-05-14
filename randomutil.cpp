#include "randomutil.h"

#include <QTime>
#include <cstdlib>

RandomUtil::RandomUtil(QObject *parent) :
    QObject(parent)
{
}

void RandomUtil::init()
{
    QTime midnight(0, 0, 0);
    qsrand(midnight.secsTo(QTime::currentTime()));
}

int RandomUtil::randomInt()
{
    return qrand();
}

int RandomUtil::randomInt(int from, int to)
{
    return from + (qrand() % (to - from));
}

qreal RandomUtil::randomReal()
{
    return qrand() / (qreal)RAND_MAX;
}

qreal RandomUtil::randomReal(qreal min, qreal max)
{
    return min + (randomReal() * (max - min));
}
