#ifndef SCENARIOSERIALIZER_H
#define SCENARIOSERIALIZER_H

#include <QString>
#include <QSet>

class Player;
class Planet;

class ScenarioSerializer
{
public:
    struct Scenario {
        QSet<Player*> players;
        Player *activePlayer;
        QSet<Planet*> planets;
    };

    virtual bool serialize(const Scenario &scenario, const QString &fileName) = 0;
    virtual bool deserialize(const QString &fileName, /*out*/ Scenario &scenario) = 0;
};

#endif // SCENARIOSERIALIZER_H
