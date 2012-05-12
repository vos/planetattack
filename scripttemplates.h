#ifndef SCRIPTTEMPLATES_H
#define SCRIPTTEMPLATES_H

#include <QScriptEngine>

template <typename T>
QScriptValue QObjectToScriptValue(QScriptEngine *engine, T const &in)
{
    return engine->newQObject(in);
}

template <typename T>
void QObjectFromScriptValue(const QScriptValue &object, T &out)
{
    out = qobject_cast<T>(object.toQObject());
}

template<typename T>
int scriptRegisterQObjectMetaType(QScriptEngine *engine, const QScriptValue &prototype = QScriptValue())
{
    return qScriptRegisterMetaType(engine, QObjectToScriptValue<T>, QObjectFromScriptValue<T>, prototype);
}

template <class Container>
void scriptValueToSequence(const QScriptValue &value, Container &cont)
{
    quint32 len = value.property(QLatin1String("length")).toUInt32();
    for (quint32 i = 0; i < len; ++i) {
        QScriptValue item = value.property(i);
        cont << qscriptvalue_cast<typename Container::value_type>(item); // QSet has no method push_back
    }
}

template<typename Container>
int scriptRegisterContainerMetaType(QScriptEngine *engine, const QScriptValue &prototype = QScriptValue())
{
    return qScriptRegisterMetaType<Container>(engine, qScriptValueFromSequence, scriptValueToSequence, prototype);
}

#endif // SCRIPTTEMPLATES_H
