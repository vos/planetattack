#ifndef SCRIPTEDPLAYERINTELLIGENCE_H
#define SCRIPTEDPLAYERINTELLIGENCE_H

#include "playerintelligence.h"

#include <QScriptEngine>
#include <QScriptProgram>
#include <QFile>

class ScriptedPlayerIntelligence : public PlayerIntelligence
{
    Q_OBJECT

public:
    explicit ScriptedPlayerIntelligence(QScriptEngine *engine, Player *parent = NULL);

    void setPlayer(Player *player);

    inline const QScriptProgram& intelligenceProgram() const { return m_intelligenceProgram; }
    inline void setIntelligenceProgram(const QScriptProgram &scriptProgram) { m_intelligenceProgram = scriptProgram; }
    void setIntelligenceProgram(const QString &sourceCode);
    void setIntelligenceProgram(QFile &sourceFile);

    void think(const GameTime &gameTime);

private:
    QScriptEngine *m_scriptEngine;
    QScriptProgram m_intelligenceProgram;
    QScriptValue m_this;

    QElapsedTimer m_timer;

};

#endif // SCRIPTEDPLAYERINTELLIGENCE_H
