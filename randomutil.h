#ifndef RANDOM_H
#define RANDOM_H

#include <QObject>
#include <QMetaType>

class RandomUtil : public QObject
{
    Q_OBJECT

public:
    static void init();

    RandomUtil(QObject *parent = NULL);

    Q_INVOKABLE static int randomInt();
    Q_INVOKABLE static int randomInt(int from, int to);
    Q_INVOKABLE static qreal randomReal(); // between 0.0 and 1.0
    Q_INVOKABLE static qreal randomReal(qreal min, qreal max);

    // TODO move to cpp!
    template <typename Container>
    static typename Container::const_iterator randomElement(const Container &container) {
        int rnd = RandomUtil::randomInt(0, container.count());
        return container.constBegin() + rnd;
    }

};

Q_DECLARE_METATYPE(RandomUtil*)

#endif // RANDOM_H
