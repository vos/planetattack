#ifndef PLAYERINTELLIGENCEPROPERTYEDITOR_H
#define PLAYERINTELLIGENCEPROPERTYEDITOR_H

#include "propertyeditor.h"

#include <QComboBox>
#include <QPushButton>

class PlayerIntelligence;

class PlayerIntelligencePropertyEditor : public QWidget, public PropertyEditor
{
    Q_OBJECT

public:
    explicit PlayerIntelligencePropertyEditor(QWidget *parent = NULL);

    QWidget* widget() const { return (QWidget*)this; }

    QVariant value() const;
    void setValue(const QVariant &value);

private slots:
    void comboBox_currentIndexChanged(int index);
    void scriptButton_clicked();

private:
    PlayerIntelligence *m_playerIntelligence;
    QComboBox m_comboBox;
    QPushButton m_scriptButton;

};

#endif // PLAYERINTELLIGENCEPROPERTYEDITOR_H
