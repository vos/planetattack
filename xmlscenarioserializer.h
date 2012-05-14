#ifndef XMLSCENARIOSERIALIZER_H
#define XMLSCENARIOSERIALIZER_H

#include "scenarioserializer.h"

class XmlScenarioSerializer : public ScenarioSerializer
{
public:
    bool serialize(const Scenario &scenario, const QString &fileName);
    bool deserialize(const QString &fileName, Scenario &scenario);
};

#endif // XMLSCENARIOSERIALIZER_H
