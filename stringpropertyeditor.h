#ifndef STRINGPROPERTYEDITOR_H
#define STRINGPROPERTYEDITOR_H

#include <QTextEdit>
#include "propertyeditor.h"

class StringPropertyEditor : public QTextEdit, public PropertyEditor
{
    Q_OBJECT

public:
    explicit StringPropertyEditor(QWidget *parent = NULL);

    QWidget* widget() const { return (QWidget*)this; }

    inline QVariant value() const { return this->toPlainText(); }
    inline void setValue(const QVariant &value) { this->setText(value.toString()); }

};

#endif // STRINGPROPERTYEDITOR_H
