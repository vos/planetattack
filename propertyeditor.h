#ifndef PROPERTYEDITOR_H
#define PROPERTYEDITOR_H

#include <QVariant>

class PropertyEditor
{
public:
    virtual QWidget* widget() const = 0;

    virtual QVariant value() const = 0;
    virtual void setValue(const QVariant &value) = 0;

};

#endif // PROPERTYEDITOR_H
