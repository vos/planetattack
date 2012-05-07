#ifndef VECTOR2DPROPERTYEDITOR_H
#define VECTOR2DPROPERTYEDITOR_H

#include <QWidget>
#include "propertyeditor.h"

#include <QVector2D>

QT_FORWARD_DECLARE_CLASS(QTextEdit)

class Vector2DPropertyEditor : public QWidget, public PropertyEditor
{
    Q_OBJECT

public:
    explicit Vector2DPropertyEditor(QWidget *parent = NULL);

    QWidget* widget() const { return (QWidget*)this; }

    QVariant value() const;
    void setValue(const QVariant &value);

private:
    QTextEdit *m_x;
    QTextEdit *m_y;

};

#endif // VECTOR2DPROPERTYEDITOR_H
