#include "enumlistmodel.h"
#include <QJsonObject>

EnumListModel::EnumListModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

EnumListModel::~EnumListModel()
{

}

void EnumListModel::loadFromJsonArray(const QJsonArray &array)
{
    for (const auto &item : qAsConst(array))
    {
        QJsonObject itemobj = item.toObject();
        QString key = itemobj["key"].toString();
        QString alias = itemobj["alias"].toString();

        m_EnumKey.append({key, alias});
    }
}

QString EnumListModel::alias(const QString &key) const
{
    QList<EnumlistElement>::const_iterator iter =
            std::find_if(m_EnumKey.begin(), m_EnumKey.end(),
                         [key](const EnumlistElement &element) -> bool
    {
        return element.key == key;
    });

    return iter == m_EnumKey.end() ? QString() : (*iter).alias;
}

QString EnumListModel::key(const QString &alias) const
{
    QList<EnumlistElement>::const_iterator iter =
            std::find_if(m_EnumKey.begin(), m_EnumKey.end(),
                         [alias](const EnumlistElement &element) -> bool
    {
        return element.alias == alias;
    });

    return iter == m_EnumKey.end() ? QString() : (*iter).key;
}

void EnumListModel::setMetaEnum(const QMetaEnum &menum)
{
    m_MetaEnum = menum;
}

int EnumListModel::keyToValue(const QString &key)
{
    return m_MetaEnum.keyToValue(key.toLocal8Bit());
}

QString EnumListModel::valueToAlias(const int &value)
{
    QString valkey = m_MetaEnum.valueToKey(value);
    return alias(valkey);
}

QString EnumListModel::valueToKey(const int &value)
{
    QString valkey = m_MetaEnum.valueToKey(value);
    return key(valkey);
}

int EnumListModel::indexFromValue(const int &value) const
{
    QString valkey = m_MetaEnum.valueToKey(value);

    QList<EnumlistElement>::const_iterator iter =
            std::find_if(m_EnumKey.begin(), m_EnumKey.end(),
                         [valkey](const EnumlistElement &element) -> bool
    {
        return element.key == valkey;
    });

    return std::distance(m_EnumKey.begin(), iter);
}

int EnumListModel::rowCount(const QModelIndex &parent) const
{
    if (m_EnumKey.empty())
        return m_MetaEnum.keyCount();

    return m_EnumKey.size();
}

QVariant EnumListModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        if (!m_EnumKey.empty())
            return m_EnumKey[index.row()].alias;
    }
    else if (role == RoleKey)
        return m_EnumKey[index.row()].key;

    return QVariant();
}
