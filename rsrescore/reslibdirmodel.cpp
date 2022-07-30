#include "reslibdirmodel.h"
#include "rsrescore.h"
#include <QIcon>

ResLibDirModel::ResLibDirModel(QObject *parent)
    : QAbstractTableModel{parent}
{

}

ResLibDirModel::~ResLibDirModel()
{

}

QVariant ResLibDirModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == fldName)
            return tr("Наименование");
        else if (section == fldType)
            return tr("Тип");
        else if (section == fldComment)
            return tr("Примечание");
    }

    return QAbstractTableModel::headerData(section, orientation, role);
}

int ResLibDirModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 3;
}

int ResLibDirModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return m_Elements.size();
}

QVariant ResLibDirModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole)
    {
        const tResLibDirElement &elem = m_Elements[index.row()];
        if (index.column() == fldName)
            return elem.name;
        else if (index.column() == fldComment)
            return elem.comment;
        else if (index.column() == fldType)
            return RsResCore::typeNameFromResType(elem.Type);
    }
    else if (role == Qt::DecorationRole)
    {
        QVariant val;
        const tResLibDirElement &elem = m_Elements[index.row()];
        if (index.column() == fldName)
            val = RsResCore::iconFromResType(elem.Type);

        return val;
    }
    return QVariant();
}

void ResLibDirModel::addElement(const quint16 &Type, const QString &name, const QString &comment)
{
    beginInsertRows(QModelIndex(), m_Elements.size(), m_Elements.size());
    tResLibDirElement elem;
    elem.Type = Type;
    elem.name = name;
    elem.comment = comment;

    m_Elements.append(elem);
    m_ResTypes.insert(Type);
    endInsertRows();
}

const tResLibDirElement &ResLibDirModel::element(const QModelIndex &index) const
{
    return element(index.row());
}

const tResLibDirElement &ResLibDirModel::element(const quint16 &index) const
{
    return m_Elements[index];
}

const ResTypesSet &ResLibDirModel::resTypes() const
{
    return m_ResTypes;
}
