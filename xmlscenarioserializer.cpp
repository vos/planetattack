#include "xmlscenarioserializer.h"

#include <QDomDocument>
#include <QFile>
#include <QDir>

#include "game.h"
#include "humanplayer.h"
#include "computerplayer.h"
#include "scriptedplayerintelligence.h"
#include "planet.h"

bool XmlScenarioSerializer::serialize(const XmlScenarioSerializer::Scenario &scenario, const QString &fileName)
{
    QDomDocument doc;
    QDomElement scenarioElement = doc.createElement("scenario");
    doc.appendChild(scenarioElement);

    QHash<const Player*, int> playerIdMap;
    int nextPlayerId = 0;

    QDomElement playersElement = doc.createElement("players");
    scenarioElement.appendChild(playersElement);
    foreach (const Player *player, scenario.players) {
        QDomElement playerElement = doc.createElement("player");
        playersElement.appendChild(playerElement);
        int playerId;
        if (playerIdMap.contains(player)) {
            playerId = playerIdMap.value(player);
        } else {
            playerId = nextPlayerId++;
            playerIdMap.insert(player, playerId);
        }
        playerElement.setAttribute("id", playerId);
        playerElement.setAttribute("name", player->name());
        playerElement.setAttribute("color", player->color().name());
        playerElement.setAttribute("resourceFactor", player->resourceFactor());
        if (player->isComputer()) {
            const ComputerPlayer *computerPlayer = static_cast<const ComputerPlayer*>(player);
            if (computerPlayer->hasIntelligence()) {
                PlayerIntelligence *intelligence = computerPlayer->intelligence();
                ScriptedPlayerIntelligence *scriptedIntelligence = dynamic_cast<ScriptedPlayerIntelligence*>(intelligence);
                if (scriptedIntelligence != NULL && !scriptedIntelligence->intelligenceProgram().isNull()) {
                    const QScriptProgram &intelligenceProgram = scriptedIntelligence->intelligenceProgram();
                    QString fileName = intelligenceProgram.fileName();
                    if (!fileName.isEmpty() && QFile::exists(fileName)) {
                        if (!QDir::isRelativePath(fileName)) {
                            fileName = QDir::current().relativeFilePath(fileName);
                        }
                        playerElement.setAttribute("intelligence", fileName);
                    }  else {
                        QDomElement intelligenceElement = doc.createElement("intelligence");
                        intelligenceElement.appendChild(doc.createCDATASection(intelligenceProgram.sourceCode()));
                        playerElement.appendChild(intelligenceElement);
                    }
                }
            }
        }
    }

    scenarioElement.setAttribute("activePlayer", playerIdMap.value(scenario.activePlayer));

    QDomElement planetsElement = doc.createElement("planets");
    scenarioElement.appendChild(planetsElement);
    foreach (const Planet *planet, scenario.planets) {
        QDomElement planetElement = doc.createElement("planet");
        planetsElement.appendChild(planetElement);
        planetElement.setAttribute("positionX", int(planet->position().x()));
        planetElement.setAttribute("positionY", int(planet->position().y()));
        planetElement.setAttribute("radius", int(planet->radius()));
        planetElement.setAttribute("resources", int(planet->resources()));
        planetElement.setAttribute("productionFactor", planet->productionFactor());
        planetElement.setAttribute("color", planet->color().name());
        if (planet->hasPlayer()) {
            planetElement.setAttribute("player", playerIdMap.value(planet->player()));
        }
    }

    QFile file(fileName);
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        file.write(doc.toByteArray(4));
    } else {
        qWarning("cannot open file \"%s\" for writing!", qPrintable(fileName));
        return false;
    }

    return true;
}

bool XmlScenarioSerializer::deserialize(const QString &fileName, XmlScenarioSerializer::Scenario &scenario)
{
    QDomDocument doc;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning("cannot open file \"%s\" for reading!", qPrintable(fileName));
        return false;
    }
    QString errorMessage;
    int errorLine, errorColumn;
    if (!doc.setContent(&file, &errorMessage, &errorLine, &errorColumn)) {
        file.close();
        qWarning("error while parsing file \"%s\": %s at line %d, column %d!",
                 qPrintable(fileName), qPrintable(errorMessage), errorLine, errorColumn);
        return false;
    }
    file.close();

    QDomElement scenarioElement = doc.documentElement();
    QHash<int, Player*> idPlayerMap;

    QDomElement playersElement = scenarioElement.firstChildElement("players");
    QDomNodeList playerNodeList = playersElement.elementsByTagName("player");
    for (uint i = 0; i < playerNodeList.length(); ++i) {
        QDomElement playerElement = playerNodeList.item(i).toElement();
        Player *player;
        QString name = playerElement.attribute("name");
        QColor color(playerElement.attribute("color"));
        QString intelligenceFileName = playerElement.attribute("intelligence");
        if (!intelligenceFileName.isEmpty() && QFile::exists(intelligenceFileName)) {
            ScriptedPlayerIntelligence *intelligence = new ScriptedPlayerIntelligence(Game::instance()->scriptEngine());
            intelligence->setIntelligenceProgramFile(intelligenceFileName);
            player = new ComputerPlayer(name, color, intelligence, Game::instance());
        } else {
            QDomElement intelligenceElement = playerElement.firstChildElement("intelligence");
            if (!intelligenceElement.isNull()) {
                QDomCDATASection intelligenceData = intelligenceElement.firstChild().toCDATASection();
                ScriptedPlayerIntelligence *intelligence = new ScriptedPlayerIntelligence(Game::instance()->scriptEngine());
                intelligence->setIntelligenceProgram(intelligenceData.data());
                player = new ComputerPlayer(name, color, intelligence, Game::instance());
            } else {
                player = new HumanPlayer(name, color, Game::instance());
            }
        }
        player->setResourceFactor(playerElement.attribute("resourceFactor").toDouble());
        int id = playerElement.attribute("id").toInt();
        idPlayerMap.insert(id, player);
        scenario.players.insert(player);
    }

    int activePlayerId = scenarioElement.attribute("activePlayer").toInt();
    if (idPlayerMap.contains(activePlayerId)) {
        scenario.activePlayer = idPlayerMap.value(activePlayerId);
    } else {
        qWarning("active player id not found in player list: %d", activePlayerId);
        scenario.activePlayer = NULL;
    }

    QDomElement planetsElement = scenarioElement.firstChildElement("planets");
    QDomNodeList planetNodeList = planetsElement.elementsByTagName("planet");
    for (uint i = 0; i < planetNodeList.length(); ++i) {
        QDomElement planetElement = planetNodeList.item(i).toElement();
        int positionX = planetElement.attribute("positionX").toInt();
        int positionY = planetElement.attribute("positionY").toInt();
        int radius = planetElement.attribute("radius").toInt();
        int resources = planetElement.attribute("resources").toInt();
        QColor color(planetElement.attribute("color"));
        Planet *planet = new Planet(QVector2D(positionX, positionY), radius, resources, color);
        qreal productionFactor = planetElement.attribute("productionFactor").toDouble();
        planet->setProductionFactor(productionFactor);
        if (planetElement.hasAttribute("player")) {
            bool ok;
            int id = planetElement.attribute("player").toInt(&ok);
            if (ok && idPlayerMap.contains(id)) {
                Player *player = idPlayerMap.value(id);
                player->addPlanet(planet);
            } else {
                qWarning("player id invalid or not found in player list: %d", id);
            }
        }
        scenario.planets.insert(planet);
    }

    return true;
}
