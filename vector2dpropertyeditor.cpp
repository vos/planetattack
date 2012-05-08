#include "vector2dpropertyeditor.h"

#include <QHBoxLayout>
#include <QLineEdit>

Vector2DPropertyEditor::Vector2DPropertyEditor(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_x = new QLineEdit);
    layout->addWidget(m_y = new QLineEdit);
    setLayout(layout);
}

QVariant Vector2DPropertyEditor::value() const
{
    return QVector2D(m_x->text().toDouble(), m_y->text().toDouble());
}

void Vector2DPropertyEditor::setValue(const QVariant &value)
{
    QVector2D vec = value.value<QVector2D>();
    m_x->setText(QString::number(vec.x()));
    m_y->setText(QString::number(vec.y()));
}
