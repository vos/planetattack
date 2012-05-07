#include "vector2dpropertyeditor.h"

#include <QHBoxLayout>
#include <QTextEdit>

Vector2DPropertyEditor::Vector2DPropertyEditor(QWidget *parent) :
    QWidget(parent)
{
    QHBoxLayout *layout = new QHBoxLayout;
    layout->addWidget(m_x = new QTextEdit);
    layout->addWidget(m_y = new QTextEdit);
    setLayout(layout);
}

QVariant Vector2DPropertyEditor::value() const
{
    return QVector2D(m_x->toPlainText().toDouble(), m_y->toPlainText().toDouble());
}

void Vector2DPropertyEditor::setValue(const QVariant &value)
{
    QVector2D vec = value.value<QVector2D>();
    m_x->setText(QString::number(vec.x()));
    m_y->setText(QString::number(vec.y()));
}
