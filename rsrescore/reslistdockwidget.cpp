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
#include <QMenu>
#include <QWidgetAction>
#include <QListView>
#include <QStandardItemModel>

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
    qint16 type = sourceModel()->data(sourceModel()->index(source_row, fldType)).toInt();

    if (!m_Types.isEmpty())
    {
        if (!m_Types.contains(type))
            return false;
    }

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

void ResFilterModel::setFilterTypes(const QList<qint16> &Types)
{
    m_Types = Types;
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
            QModelIndex source = mapToSource(this->index(index.row(), fldType));
            QVariant val = sourceModel()->data(source, Qt::EditRole);

            return RsResCore::iconFromResType(val.toInt());
        }
    }
    return QSortFilterProxyModel::data(index, role);
}

// =======================================================================

ResListDockWidget::ResListDockWidget(QWidget *parent) :
    QDockWidget(parent),
    m_pFiler(nullptr)
{
    m_List = new QTreeView();
    m_List->setRootIsDecorated(false);
    m_List->setContextMenuPolicy(Qt::CustomContextMenu);

    m_Container = new QMainWindow();

    m_pToolBar = new QToolBar();
    m_pNameFilter = new QLineEdit();
    m_FilterBtn = new QToolButton();
    m_FilterMenu = new QMenu(m_FilterBtn);
    m_FilterBtn->setMenu(m_FilterMenu);
    m_FilterBtn->setPopupMode(QToolButton::MenuButtonPopup);

    m_pNameFilter->setPlaceholderText(tr("Введите текст для поиска..."));
    m_pNameFilter->setClearButtonEnabled(true);
    m_FilterBtn->setIcon(QIcon(":/img/Filter.png"));
    m_FilterBtn->setIconSize(QSize(16,16));

    m_pTypesModel = new QStandardItemModel(this);
    m_FilterView = new QListView(this);
    m_FilterViewAction = new QWidgetAction(this);
    m_FilterViewAction->setDefaultWidget(m_FilterView);
    m_FilterMenu->addAction(m_FilterViewAction);
    m_FilterView->setModel(m_pTypesModel);

    m_FilterView->setMaximumWidth(150);

    m_pToolBar->setMovable(false);
    m_pToolBar->addWidget(m_pNameFilter);
    m_pToolBar->addWidget(m_FilterBtn);
    m_pToolBar->setIconSize(QSize(16,16));

    m_Container->setCentralWidget(m_List);
    m_Container->addToolBar(m_pToolBar);
    setWidget(m_Container);

    setWindowTitle(tr("Список ресурсов"));

    connect(m_List, &QTreeView::customContextMenuRequested, this, &ResListDockWidget::onCustomContextMenuRequested);
    connect(m_List, &QTreeView::doubleClicked, this, &ResListDockWidget::onDoubleClicked);
    connect(m_pTypesModel, &QStandardItemModel::itemChanged, this, &ResListDockWidget::typeItemChanged);
}

ResListDockWidget::~ResListDockWidget()
{
    if (m_List)
        delete m_List;
}

void ResListDockWidget::resetFilterModel()
{
    if (m_pFiler)
        delete m_pFiler;

    m_pFiler = new ResFilterModel(this);
    m_List->setModel(m_pFiler);

    connect(m_pNameFilter, &QLineEdit::textChanged, m_pFiler, &ResFilterModel::setFilterName);
}

void ResListDockWidget::setupTypesFilter(QAbstractItemModel *model)
{
    if (!model)
        return;

    m_pTypesModel->clear();
    const QList<qint16> types = RsResCore::types();

    for (int i = 0; i < types.size(); i++)
    {
        for (int j = 0; j < model->rowCount(); j++)
        {
            QVariant val = model->data(model->index(j, ResFilterModel::fldType));

            if (val.toInt() == types[i])
            {
                QStandardItem *item = new QStandardItem();
                item->setText(RsResCore::typeNameFromResType(types[i]));
                item->setCheckable(true);
                item->setCheckState(Qt::Checked);
                item->setIcon(RsResCore::iconFromResType(types[i]));
                item->setData(types[i]);

                m_pTypesModel->appendRow(item);
                break;
            }
        }
    }
}

void ResListDockWidget::setModel(QAbstractItemModel *model)
{
    if (!model)
        return;

    resetFilterModel();
    setupTypesFilter(model);
    m_pFiler->setSourceModel(model);
    m_List->header()->resizeSection(1, 50);
    m_pFiler->sort(0);
}

void ResListDockWidget::onDoubleClicked(const QModelIndex &index)
{
    QModelIndex source = m_pFiler->mapToSource(index);
    ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

    int type = 0;
    QString name;
    model->getResNameAndType(source, name, type);
    emit doubleClicked(name, type);
}

void ResListDockWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    QModelIndex index = m_pFiler->mapToSource(m_List->indexAt(pos));

    if (index.isValid())
    {
        QMenu contextMenu;
        ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

        int type = 0;
        QString name;
        model->getResNameAndType(index, name, type);

        QIcon editicon = RsResCore::iconFromResType(type);
        QAction *edit = contextMenu.addAction(editicon, tr("Редактировать ресурс [%1]").arg(name));
        QAction *del = contextMenu.addAction(QIcon(":/img/Delete.png"), tr("Удалить ресурс [%1]").arg(name));

        contextMenu.setDefaultAction(edit);
        QAction *action = contextMenu.exec(m_List->mapToGlobal(pos));

        if (action == del)
            emit deleteRequest(name, type);
        else if (action == edit)
            emit doubleClicked(name, type);
    }
}

void ResListDockWidget::typeItemChanged(QStandardItem *item)
{
    QList<qint16> Types;
    for (int i = 0; i < m_pTypesModel->rowCount(); i++)
    {
        QStandardItem *elem = m_pTypesModel->item(i, 0);

        if (elem->checkState() == Qt::Checked)
            Types.append(elem->data().toInt());
    }

    m_pFiler->setFilterTypes(Types);
}
