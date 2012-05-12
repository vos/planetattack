#ifndef SCRIPTEDPLAYERINTELLIGENCE_H
#define SCRIPTEDPLAYERINTELLIGENCE_H

#include "playerintelligence.h"

#include <QScriptEngine>
#include <QScriptProgram>

class ScriptedPlayerIntelligence : public PlayerIntelligence
{
    Q_OBJECT
    Q_PROPERTY(int delay READ delay WRITE setDelay)

public:
    explicit ScriptedPlayerIntelligence(QScriptEngine *engine, Player *parent = NULL);

    void setPlayer(Player *player);

    inline const QScriptProgram& intelligenceProgram() const { return m_intelligenceProgram; }
    bool setIntelligenceProgram(const QScriptProgram &scriptProgram);
    bool setIntelligenceProgram(const QString &sourceCode);
    bool setIntelligenceProgramFile(const QString &fileName);

    inline int delay() const { return m_delay; }
    inline void setDelay(int delay) { m_delay = delay; }

    static bool checkScriptSyntax(const QString &sourceCode, const QString &fileName = QString());

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
