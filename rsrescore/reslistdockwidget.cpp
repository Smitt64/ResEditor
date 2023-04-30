#include "reslistdockwidget.h"
#include "rsrescore.h"
#include <QMainWindow>
#include <QTreeView>
#include <QLineEdit>
#include <QToolBar>
#include <QToolButton>
#include <QHeaderView>
#include <QSortFilterProxyModel>
#include <QDebug>

ResFilterModel::ResFilterModel(QObject *parent) :
    QSortFilterProxyModel(parent)
{

}

ResFilterModel::~ResFilterModel()
{

}

bool ResFilterModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    QString name = sourceModel()->data(sourceModel()->index(source_row, 0)).toString();

    if (name.contains(m_FilterName, Qt::CaseInsensitive))
        return true;
    else
        return false;

    return QSortFilterProxyModel::filterAcceptsRow(source_row, source_parent);
}

void ResFilterModel::setFilterName(const QString &name)
{
    m_FilterName = name;
    invalidate();
}

QVariant ResFilterModel::headerData(int section, Qt::Orientation orientation, int role) const
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

    return QSortFilterProxyModel::headerData(section, orientation, role);
}

int ResFilterModel::columnCount(const QModelIndex &parent) const
{
    return fldComment + 1;
}

void ResFilterModel::getResNameAndType(const QModelIndex &index, QString &name, int &type)
{
    name = sourceModel()->data(sourceModel()->index(index.row(), ColumnName), Qt::EditRole).toString();
    type = sourceModel()->data(sourceModel()->index(index.row(), ColumnType), Qt::EditRole).toInt();
}

QVariant ResFilterModel::data(const QModelIndex &index, int role) const
{
    if (role == Qt::DisplayRole || role == Qt::EditRole)
    {
        QModelIndex source = mapToSource(index);
        if (source.column() == fldName)
            return sourceModel()->data(source, role);
        else if (source.column() == fldComment)
            return sourceModel()->data(source, role);
        else if (source.column() == fldType)
        {
            QVariant val = sourceModel()->data(source, role);

            if (role == Qt::DisplayRole)
                return RsResCore::typeNameFromResType(val.toInt());
            else
                return val;
        }
    }
    else if (role == Qt::DecorationRole)
    {
        QVariant val;
        if (index.column() == fldName)
        {
            QVariant type = sourceModel()->data(sourceModel()->index(index.row(), ColumnType));
            val = RsResCore::iconFromResType(type.toInt());
            return val;
        }
    }
    return QSortFilterProxyModel::data(index, role);
}

// =======================================================================

ResListDockWidget::ResListDockWidget(QWidget *parent) :
    QDockWidget(parent)
{
    m_List = new QTreeView();
    m_pFiler = new ResFilterModel(this);
    m_List->setModel(m_pFiler);

    m_Container = new QMainWindow();

    m_pToolBar = new QToolBar();
    m_pNameFilter = new QLineEdit();
    m_FilterBtn = new QToolButton();

    m_pNameFilter->setPlaceholderText(tr("Введите текст для поиска..."));
    m_pNameFilter->setClearButtonEnabled(true);
    m_FilterBtn->setIcon(QIcon(":/img/Filter.png"));
    m_FilterBtn->setIconSize(QSize(16,16));

    m_pToolBar->setMovable(false);
    m_pToolBar->addWidget(m_pNameFilter);
    m_pToolBar->addWidget(m_FilterBtn);
    m_pToolBar->setIconSize(QSize(16,16));

    m_Container->setCentralWidget(m_List);
    m_Container->addToolBar(m_pToolBar);
    setWidget(m_Container);

    setWindowTitle(tr("Список ресурсов"));

    connect(m_List, &QTreeView::doubleClicked, this, &ResListDockWidget::onDoubleClicked);
    connect(m_pNameFilter, &QLineEdit::textChanged, m_pFiler, &ResFilterModel::setFilterName);
}

ResListDockWidget::~ResListDockWidget()
{
    if (m_List)
        delete m_List;
}

void ResListDockWidget::setModel(QAbstractItemModel *model)
{
    m_pFiler->setSourceModel(model);
    m_List->header()->resizeSection(1, 50);
}

void ResListDockWidget::onDoubleClicked(const QModelIndex &index)
{
    QModelIndex source = m_pFiler->mapToSource(index);
    ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

    int type = 0;;
    QString name;
    model->getResNameAndType(source, name, type);
    emit doubleClicked(name, type);
}
