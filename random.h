#ifndef RANDOM_H
#define RANDOM_H

#include <QtGlobal>

class Random
{
public:
    static void init();
    static int randomInt(int from, int to);

    // TODO move to cpp!
    template <typename Container>
    static typename Container::const_iterator randomElement(const Container &container) {
        int rnd = Random::randomInt(0, container.count());
        int i = 0;
        typename Container::const_iterator it;
        for (it = container.constBegin(); it != container.constEnd(); ++it) {
            if (rnd == i++) {
                return it;
            }
        }
        return it;
    }

};

#endif // RANDOM_H
