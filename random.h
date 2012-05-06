#ifndef RANDOM_H
#define RANDOM_H

#include <QtGlobal>

class Random
{
public:
    static void init();
    static int randomInt();
    static int randomInt(int from, int to);
    static qreal randomReal(); // between 0.0 and 1.0
    static qreal randomReal(qreal min, qreal max);

    // TODO move to cpp!
    template <typename Container>
    static typename Container::const_iterator randomElement(const Container &container) {
        int rnd = Random::randomInt(0, container.count());
        return container.constBegin() + rnd;
    }

};

#endif // RANDOM_H
