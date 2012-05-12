#include "scriptedplayerintelligence.h"

#include <QDebug>

ScriptedPlayerIntelligence::ScriptedPlayerIntelligence(QScriptEngine *engine, Player *parent) :
    PlayerIntelligence(parent)
{
    m_scriptEngine = engine;
    m_timer.start();
}

void ScriptedPlayerIntelligence::setPlayer(Player *player)
{
    PlayerIntelligence::setPlayer(player);
    m_this = m_scriptEngine->newQObject(player);
}

void ScriptedPlayerIntelligence::setIntelligenceProgram(const QString &sourceCode)
{
    m_intelligenceProgram = QScriptProgram(sourceCode);
}

void ScriptedPlayerIntelligence::setIntelligenceProgram(QFile &sourceFile)
{
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        qWarning("cannot open script file \"%s\": %s",
                 qPrintable(sourceFile.fileName()),
                 qPrintable(sourceFile.errorString()));
        return;
    }
    QString sourceCode = sourceFile.readAll();
    sourceFile.close();
    m_intelligenceProgram = QScriptProgram(sourceCode, sourceFile.fileName());
}

void ScriptedPlayerIntelligence::think(const GameTime &gameTime)
{
    if (m_intelligenceProgram.isNull())
        return;

    m_scriptEngine->globalObject().setProperty("player", m_this);
//    m_scriptEngine->globalObject().setProperty("gameTime", m_scriptEngine->newQObject(&gameTime));
    QScriptValue result = m_scriptEngine->evaluate(m_intelligenceProgram);
    if (result.isError()) {
        qCritical() << QString::fromLatin1("%0:%1: %2")
                       .arg(m_intelligenceProgram.fileName())
                       .arg(result.property("lineNumber").toInt32())
                       .arg(result.toString());
    }
}
