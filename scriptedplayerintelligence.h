#ifndef SCRIPTEDPLAYERINTELLIGENCE_H
#define SCRIPTEDPLAYERINTELLIGENCE_H

#include "playerintelligence.h"

#include <QScriptEngine>
#include <QScriptProgram>
#include <QFile>

class ScriptedPlayerIntelligence : public PlayerIntelligence
{
    Q_OBJECT
    Q_PROPERTY(int delay READ delay WRITE setDelay)

public:
    explicit ScriptedPlayerIntelligence(QScriptEngine *engine, Player *parent = NULL);

    void setPlayer(Player *player);

    inline const QScriptProgram& intelligenceProgram() const { return m_intelligenceProgram; }
    inline void setIntelligenceProgram(const QScriptProgram &scriptProgram) { m_intelligenceProgram = scriptProgram; }
    void setIntelligenceProgram(const QString &sourceCode);
    void setIntelligenceProgram(QFile &sourceFile);

    inline int delay() const { return m_delay; }
    inline void setDelay(int delay) { m_delay = delay; }

public slots:
    void think(const GameTime &gameTime);

private:
    QScriptEngine *m_scriptEngine;
    QScriptProgram m_intelligenceProgram;
    QScriptValue m_this;

    QElapsedTimer m_timer;
    int m_delay;

};

#endif // SCRIPTEDPLAYERINTELLIGENCE_H
