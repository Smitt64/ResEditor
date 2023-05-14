#include "lbrreslistmodel.h"
#include "res_lbr.h"
#include "lbrobject.h"
#include <QTextCodec>
#include <QDateTime>

class LbrResListModelPrivate
{
    Q_DECLARE_PUBLIC(LbrResListModel);

    friend class LbrObjectPrivate;
public:
    LbrResListModel * q_ptr;

    LbrResListModelPrivate()
    {
        m_p866 = QTextCodec::codecForName("IBM 866");
    }

    QTextCodec *m_p866;
    QVector<RLibDirElem*> m_Elements;
};

LbrResListModel::LbrResListModel(QObject *parent)
    : QAbstractTableModel{parent},
      d_ptr(new LbrResListModelPrivate())
{
    Q_D(LbrResListModel);
    d->q_ptr = this;
}

LbrResListModel::~LbrResListModel()
{

}

void LbrResListModel::addDirElement(void *elem)
{
    Q_D(LbrResListModel);
    d->m_Elements.append(reinterpret_cast<RLibDirElem*>(elem));
}

int LbrResListModel::rowCount(const QModelIndex &parent) const
{
    Q_D(const LbrResListModel);
    return d->m_Elements.size();
}

int LbrResListModel::columnCount(const QModelIndex &parent) const
{
    return ColumnCount;
}

QVariant LbrResListModel::data(const QModelIndex &index, int role) const
{
    Q_D(const LbrResListModel);
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        if (index.column() == ColumnName)
            return d->m_Elements[index.row()]->name;
        else if(index.column() == ColumnType)
            return d->m_Elements[index.row()]->type;
        else if(index.column() == ColumnComment)
        {
            const char *comment = ((char *)((d->m_Elements[index.row()]) + 1));
            return d->m_p866->toUnicode(comment);
        }
        else if (index.column() == ColumnTime)
        {
            RLibDirElem *elem = d->m_Elements[index.row()];
            const ftime &ftime = elem->ftime;
            return QDateTime(QDate(ftime.ft_year, ftime.ft_month, ftime.ft_day),
                             QTime(ftime.ft_hour, ftime.ft_min, ftime.ft_tsec));
        }
    }
    return QVariant();
}

void LbrResListModel::sort(int column, Qt::SortOrder order)
{
    Q_D(LbrResListModel);
    std::sort(d->m_Elements.begin(), d->m_Elements.end(), [=](RLibDirElem *item1, RLibDirElem *item2)
    {
        if (column == ColumnName)
        {
            if (order == Qt::AscendingOrder)
                return strcmp(item1->name, item2->name) < 0;
            else
                return strcmp(item1->name, item2->name) > 0;
        }
        else if (column == ColumnType)
        {
            if (order == Qt::AscendingOrder)
                return item1->type < item2->type;
            else
                return item1->type > item2->type;
        }

        return false;
    });
}
