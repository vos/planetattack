#include "scriptedplayerintelligence.h"

#include <QFile>
#include <QMessageBox>
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

bool ScriptedPlayerIntelligence::setIntelligenceProgram(const QScriptProgram &scriptProgram)
{
    if (checkScriptSyntax(scriptProgram.sourceCode(), scriptProgram.fileName())) {
        m_intelligenceProgram = scriptProgram;
        return true;
    }
    return false;
}

bool ScriptedPlayerIntelligence::setIntelligenceProgram(const QString &sourceCode)
{
    if (checkScriptSyntax(sourceCode)) {
        m_intelligenceProgram = QScriptProgram(sourceCode);
        return true;
    }
    return false;
}

bool ScriptedPlayerIntelligence::setIntelligenceProgramFile(const QString &fileName)
{
    QFile sourceFile(fileName);
    if (!sourceFile.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(NULL, "Script File Error",
                             QString("Cannot open script file \"%1\":\n%2")
                             .arg(fileName, sourceFile.errorString()));
        return false;
    }
    QString sourceCode = sourceFile.readAll();
    sourceFile.close();

    if (checkScriptSyntax(sourceCode, fileName)) {
        m_intelligenceProgram = QScriptProgram(sourceCode, fileName);
        return true;
    }
    return false;
}

bool ScriptedPlayerIntelligence::checkScriptSyntax(const QString &sourceCode, const QString &fileName)
{
    QScriptSyntaxCheckResult checkResult = QScriptEngine::checkSyntax(sourceCode);
    if (checkResult.state() == QScriptSyntaxCheckResult::Valid) {
        return true;
    } else {
        QMessageBox::critical(NULL, "Script Syntax Error",
                              QString("Syntax error in script \"%1\" at line %2, column %3:\n%4")
                              .arg(fileName)
                              .arg(checkResult.errorLineNumber())
                              .arg(checkResult.errorColumnNumber())
                              .arg(checkResult.errorMessage()));
        return false;
    }
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
            // TODO send error to log window when there is no debugger attached
            qCritical() << QString::fromLatin1("%0:%1: %2")
                           .arg(m_intelligenceProgram.fileName())
                           .arg(result.property("lineNumber").toInt32())
                           .arg(result.toString());
        }
        m_timer.restart();
    }
}
