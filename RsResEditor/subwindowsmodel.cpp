#include "subwindowsmodel.h"
#include <QIcon>
#include <QMdiSubWindow>

class SubWindowsItem
{
    friend class SubWindowsModel;
public:
    SubWindowsItem(QMdiSubWindow *window)
    {
        wnd = window;
    }

    QVariant data(int role = Qt::DisplayRole) const
    {
        if (!wnd)
            return QVariant();

        if (role == Qt::DisplayRole)
            return wnd->windowTitle();
        else if (role == Qt::DecorationRole)
            return wnd->windowIcon();

        return QVariant();
    }

private:
    QMdiSubWindow *wnd;
};

SubWindowsModel::SubWindowsModel(QObject *parent)
    : QAbstractListModel{parent}
{

}

SubWindowsModel::~SubWindowsModel()
{

}

Qt::ItemFlags SubWindowsModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return Qt::ItemFlags();

    return Qt::ItemIsEnabled | Qt::ItemIsSelectable;
}

QModelIndex SubWindowsModel::index(int row, int column, const QModelIndex &parent)
    const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    return createIndex(row, column);
}

QModelIndex SubWindowsModel::parent(const QModelIndex &index) const
{
    return QModelIndex();
}

int SubWindowsModel::rowCount(const QModelIndex &parent) const
{
    return m_Windows.count();
}

QVariant SubWindowsModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (role != Qt::DisplayRole && role != Qt::DecorationRole)
        return QVariant();

    return m_Windows[index.row()]->data(role);
}

int SubWindowsModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 1;
}

QVariant SubWindowsModel::headerData(int section, Qt::Orientation orientation,
                                     int role) const
{
    return QVariant();
}

QModelIndex SubWindowsModel::addWindow(QMdiSubWindow *wnd)
{
    beginInsertRows(QModelIndex(), m_Windows.count(), m_Windows.count());
    m_Windows.append(new SubWindowsItem(wnd));
    endInsertRows();

    connect(wnd, SIGNAL(destroyed(QObject*)), SLOT(windowClosed(QObject*)));
    emit windowsUpdated();

    return createIndex(m_Windows.count() - 1, 0);
}

void SubWindowsModel::windowClosed(QObject *wnd)
{
    for (int j = 0; j < m_Windows.count(); j++)
    {
        SubWindowsItem *item = m_Windows[j];

        if (item->wnd == wnd)
        {
            beginRemoveRows(parent(createIndex(j, 0)), j, j);
            m_Windows.removeOne(item);
            endRemoveRows();
            delete item;

            break;
        }
    }
}

QModelIndex SubWindowsModel::findWindow(QMdiSubWindow *wnd)
{
    QModelIndex indx;

    for (int j = 0; j < m_Windows.count(); j++)
    {
        SubWindowsItem *item = m_Windows[j];

        if (item->wnd == wnd)
        {
            indx = createIndex(j, 0);
            break;
        }
    }

    return indx;
}

QMdiSubWindow *SubWindowsModel::window(const QModelIndex &index)
{
    QMdiSubWindow *wnd = nullptr;

    if (index.isValid())
        wnd = m_Windows[index.row()]->wnd;

    return wnd;
}
