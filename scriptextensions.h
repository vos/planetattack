#ifndef SCRIPTEXTENSIONS_H
#define SCRIPTEXTENSIONS_H

#include "scripttemplates.h"

#include <QVector2D>
#include <QColor>
#include <QSet>

#include "canvas.h"
#include "randomutil.h"
#include "playerintelligence.h"

QScriptValue vector2ToScriptValue(QScriptEngine *engine, const QVector2D &v)
{
    QScriptValue obj = engine->newObject();
    obj.setProperty("x", v.x());
    obj.setProperty("y", v.y());
    return obj;
}

void vector2FromScriptValue(const QScriptValue &obj, QVector2D &v)
{
    v.setX(obj.property("x").toNumber());
    v.setY(obj.property("y").toNumber());
}

QScriptValue vector2Constructor(QScriptContext *context, QScriptEngine *engine)
{
    QVector2D v;
    switch (context->argumentCount()) {
    case 0:
        v.setX(0.0);
        v.setY(0.0);
        break;
    case 1: {
        qsreal num = context->argument(0).toNumber();
        v.setX(num);
        v.setY(num);
        break;
    }
    case 2:
        v.setX(context->argument(0).toNumber());
        v.setY(context->argument(1).toNumber());
        break;
    default:
        context->throwError(QScriptContext::SyntaxError, QString("no overloaded function of Vector2 takes %1 arguments").arg(context->argumentCount()));
    }
    return engine->toScriptValue(v);
}

QScriptValue colorToScriptValue(QScriptEngine *engine, const QColor &c)
{
    return engine->newVariant(c);
}

void colorFromScriptValue(const QScriptValue &obj, QColor &c)
{
    QString colorName = obj.toString();
    if (QColor::isValidColor(colorName)) {
        c.setNamedColor(colorName);
    } else {
        // TODO: QScriptContext::throwError
        qWarning("'%s' is not a valid color name!", qPrintable(colorName));
    }
}

void addScriptExtentions(Canvas *canvas) {
    QScriptEngine *engine = canvas->scriptEngine();

    scriptRegisterQObjectMetaType<RandomUtil*>(engine);
    scriptRegisterQObjectMetaType<GameTime*>(engine);
    scriptRegisterQObjectMetaType<Player*>(engine);
    scriptRegisterContainerMetaType<QSet<Player*> >(engine);
    scriptRegisterQObjectMetaType<Planet*>(engine);
    scriptRegisterContainerMetaType<QSet<Planet*> >(engine);
    scriptRegisterQObjectMetaType<Ship*>(engine);
    scriptRegisterContainerMetaType<QSet<Ship*> >(engine);
    scriptRegisterQObjectMetaType<PlayerIntelligence*>(engine);

    // add global properties and functions
    QScriptValue globalObject = engine->globalObject();
    globalObject.setProperty("Random", engine->newQObject(new RandomUtil(canvas)));
    QScriptValue gameObject = engine->newQObject(canvas);
    globalObject.setProperty("Game", gameObject);
    gameObject.setProperty("Time", engine->newQObject(&canvas->gameTime()));

    // Vector2
    qScriptRegisterMetaType(engine, vector2ToScriptValue, vector2FromScriptValue);
    QScriptValue vector2Ctor = engine->newFunction(vector2Constructor);
    QScriptValue vector2metaObject = engine->newQMetaObject(&QObject::staticMetaObject, vector2Ctor);
    globalObject.setProperty("Vector2", vector2metaObject);

    // Color
    qScriptRegisterMetaType(engine, colorToScriptValue, colorFromScriptValue);
}

#endif // SCRIPTEXTENSIONS_H
