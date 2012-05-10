#ifndef PLAYERPROPERTYEDITOR_H
#define PLAYERPROPERTYEDITOR_H

#include <QComboBox>
#include "propertyeditor.h"

class PlayerPropertyEditor : public QComboBox, public PropertyEditor
{
    Q_OBJECT

public:
    explicit PlayerPropertyEditor(QWidget *parent = NULL);

    QWidget* widget() const { return (QWidget*)this; }

    QVariant value() const;
    void setValue(const QVariant &value);

};

#endif // PLAYERPROPERTYEDITOR_H
