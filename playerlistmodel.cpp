#include "playerlistmodel.h"
#include "player.h"

PlayerListModel::PlayerListModel(QObject *parent) :
    QAbstractListModel(parent)
{
}

Qt::ItemFlags PlayerListModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::NoItemFlags;
    return QAbstractListModel::flags(index) | Qt::ItemIsEditable;
}

QVariant PlayerListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || index.row() >= m_playerList.count())
        return QVariant();
    switch (role) {
    case Qt::DisplayRole:
    case Qt::EditRole:
        return m_playerList.at(index.row())->name();
    case Qt::DecorationRole:
        return m_playerList.at(index.row())->color();
    }
    return QVariant();
}

bool PlayerListModel::setData(const QModelIndex &index, const QVariant &value, int role)
{
    if (!index.isValid() || index.row() >= m_playerList.count())
        return false;
    if (role == Qt::EditRole) {
        QString name = value.toString();
        m_playerList.at(index.row())->setName(name);
        emit dataChanged(index, index);
        return true;
    }
    return false;
}

int PlayerListModel::addPlayer(Player *player)
{
    int index = m_playerList.count();
    beginInsertRows(QModelIndex(), index, index);
    m_playerList.append(player);
    endInsertRows();
    connect(player, SIGNAL(nameChanged(QString,QString)), SLOT(player_nameChanged(QString,QString)));
    return index;
}

int PlayerListModel::removePlayer(Player *player)
{
    int index = m_playerList.indexOf(player);
    if (index < 0)
        return index;
    beginRemoveRows(QModelIndex(), index, index);
    m_playerList.removeAt(index);
    endRemoveRows();
    player->disconnect(this, SLOT(player_nameChanged(QString,QString)));
    return index;
}

void PlayerListModel::player_nameChanged(const QString &, const QString &)
{
    Player *player = qobject_cast<Player*>(sender());
    Q_ASSERT(player);
    int index = m_playerList.indexOf(player);
    if (index < 0)
        return;
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex);
}

Player* PlayerListModel::playerAt(int index) const
{
    return m_playerList.at(index);
}

Player* PlayerListModel::takePlayerAt(int index)
{
    beginRemoveRows(QModelIndex(), index, index);
    Player *player = m_playerList.takeAt(index);
    endRemoveRows();
    player->disconnect(this, SLOT(player_nameChanged(QString,QString)));
    return player;
}
