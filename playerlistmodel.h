#ifndef PLAYERLISTMODEL_H
#define PLAYERLISTMODEL_H

#include <QAbstractListModel>

class Player;

class PlayerListModel : public QAbstractListModel
{
    Q_OBJECT

public:
    explicit PlayerListModel(QObject *parent = NULL);

    inline int rowCount(const QModelIndex & = QModelIndex()) const { return m_playerList.count(); }
    Qt::ItemFlags flags(const QModelIndex &index) const;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const;
    bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole);

    Player* playerAt(int index) const;
    Player* takePlayerAt(int index);

public slots:
    virtual int addPlayer(Player *player);
    virtual int removePlayer(Player *player);

private slots:
    void player_nameChanged(const QString &oldName, const QString &newName);

protected:
    QList<Player*> m_playerList;

};

#endif // PLAYERLISTMODEL_H
