#include "player.h"

Player::Player(const QString &name, const QColor &color, bool human, QObject *parent) :
    QObject(parent)
{
    m_name = name;
    m_color = color;
    m_human = human;
    m_target = NULL;
}
