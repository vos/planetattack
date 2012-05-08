#ifndef PLAYERINTELLIGENCEPROPERTYEDITOR_H
#define PLAYERINTELLIGENCEPROPERTYEDITOR_H

#include <QComboBox>
#include "propertyeditor.h"

class PlayerIntelligencePropertyEditor : public QComboBox, public PropertyEditor
{
    Q_OBJECT

public:
    explicit PlayerIntelligencePropertyEditor(QWidget *parent = NULL);

    QWidget* widget() const { return (QWidget*)this; }

    QVariant value() const;
    void setValue(const QVariant &value);

};

#endif // PLAYERINTELLIGENCEPROPERTYEDITOR_H
