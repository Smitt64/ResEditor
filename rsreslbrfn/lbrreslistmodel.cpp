#include "lbrreslistmodel.h"
#include "res_lbr.h"
#include "lbrobject.h"
#include <QTextCodec>
#include <QDateTime>

typedef struct tagModelElement : RLibDirElem
{
    QString comment;
}ModelElement;

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
    QVector<ModelElement*> m_Elements;
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
    clear();
}

void LbrResListModel::addDirElement(void *elem)
{
    Q_D(LbrResListModel);

    int size = d->m_Elements.size();
    beginInsertRows(QModelIndex(), size, size);
    ModelElement *copy = new ModelElement();
    memcpy(copy, elem, sizeof(RLibDirElem));

    const char *comment = ((char *)(((RLibDirElem*)(elem)) + 1));
    copy->comment = d->m_p866->toUnicode(comment);
    //strncpy_s(copy->comment, comment, _MAX_PATH);
    d->m_Elements.append(copy);
    endInsertRows();
}

void LbrResListModel::clear()
{
    Q_D(LbrResListModel);
    beginResetModel();
    qDeleteAll(d->m_Elements);
    d->m_Elements.clear();
    endResetModel();
}

void LbrResListModel::removeElement(const QString &name, const int &type)
{
    Q_D(LbrResListModel);
    QVector<ModelElement*>::iterator iter = std::find_if(d->m_Elements.begin(), d->m_Elements.end(), [=](RLibDirElem *item) -> bool
    {
        return name == item->name && type == item->type;
    });

    int pos = std::distance(d->m_Elements.begin(), iter);

    beginRemoveRows(QModelIndex(), pos, pos);
    ModelElement *copy = d->m_Elements.takeAt(pos);
    delete copy;
    endRemoveRows();
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
            return d->m_Elements[index.row()]->comment;
        else if (index.column() == ColumnTime)
        {
            RLibDirElem *elem = d->m_Elements[index.row()];
            const ftime &ftime = elem->ftime;
            return QDateTime(QDate(ftime.ft_year, ftime.ft_month, ftime.ft_day),
                             QTime(ftime.ft_hour, ftime.ft_min, ftime.ft_tsec));
        }
    }
    else if (role == Qt::ToolTipRole)
    {
        RLibDirElem *elem = d->m_Elements[index.row()];
        const ftime &ftime = elem->ftime;
        QDateTime dt = QDateTime(QDate(ftime.ft_year, ftime.ft_month, ftime.ft_day),
                  QTime(ftime.ft_hour, ftime.ft_min, ftime.ft_tsec));

        QString tooltip = QString("<b>Ресурс: </b>%1<br><b>Тип: </b>%2<br><b>Изменен: </b>%3")
                              .arg(elem->name)
                              .arg(LbrObjectInterface::getResTypeName(elem->type))
                              .arg(dt.toString("dd.MM.yyyy hh:mm"));

        return tooltip;
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
