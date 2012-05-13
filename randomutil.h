#ifndef RANDOMUTIL_H
#define RANDOMUTIL_H

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
        if (container.isEmpty()) {
            return container.constEnd(); // TODO return invalid iterator?
        }
        int rnd = RandomUtil::randomInt(0, container.count());
        return container.constBegin() + rnd;
    }

};

Q_DECLARE_METATYPE(RandomUtil*)

#endif // RANDOMUTIL_H
