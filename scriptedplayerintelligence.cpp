#include "scriptedplayerintelligence.h"

#include <QDebug>

ScriptedPlayerIntelligence::ScriptedPlayerIntelligence(QScriptEngine *engine, Player *parent) :
    PlayerIntelligence(parent)
{
    m_scriptEngine = engine;
    m_timer.start();
    m_delay = 0;
}

void ScriptedPlayerIntelligence::setPlayer(Player *player)
{
    PlayerIntelligence::setPlayer(player);
    m_this = m_scriptEngine->newQObject(player);
    m_this.setProperty("AI", m_scriptEngine->newQObject(this));
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
    Q_UNUSED(gameTime);

    if (m_intelligenceProgram.isNull())
        return;

    if (m_timer.hasExpired(m_delay)) {
        m_scriptEngine->globalObject().setProperty("Player", m_this);
        QScriptValue result = m_scriptEngine->evaluate(m_intelligenceProgram);
        if (result.isError()) {
            qCritical() << QString::fromLatin1("%0:%1: %2")
                           .arg(m_intelligenceProgram.fileName())
                           .arg(result.property("lineNumber").toInt32())
                           .arg(result.toString());
        }
        m_timer.restart();
    }
}
