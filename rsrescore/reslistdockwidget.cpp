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
#include <QCompleter>

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
    QString comment = sourceModel()->data(sourceModel()->index(source_row, fldComment)).toString();

    if (!m_Types.isEmpty())
    {
        if (!m_Types.contains(type))
            return false;
    }

    if (name.contains(m_FilterName, Qt::CaseInsensitive) || comment.contains(m_FilterName, Qt::CaseInsensitive))
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
        else if (section == fldTime)
            return tr("Изменен");
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
        else if (source.column() == fldTime)
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
    QDockWidget(parent)
{
    m_Container = MakeContainer();
    m_List = m_Container->findChild<QTreeView*>("pList");
    m_pNameFilter = m_Container->findChild<QLineEdit*>("pNameFilter");

    setWidget(m_Container);

    setWindowTitle(tr("Список ресурсов"));

    connect(m_List, &QTreeView::customContextMenuRequested, this, &ResListDockWidget::onCustomContextMenuRequested);
    connect(m_List, &QTreeView::doubleClicked, this, &ResListDockWidget::onDoubleClicked);
}

ResListDockWidget::~ResListDockWidget()
{
}

void ResListDockWidget::setModel(QAbstractItemModel *model)
{
    SetModel(m_Container, model);

    m_List->header()->hideSection(2);

    connect(m_List->selectionModel(), &QItemSelectionModel::selectionChanged, this, [=](const QItemSelection &selected, const QItemSelection &deselected)
    {
        Q_UNUSED(selected)
        Q_UNUSED(deselected)
        emit selectionChanged();
    });
}

void ResListDockWidget::setFilterTypes(const QList<qint16> &Types)
{
    QVariant var = m_Container->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    if (!m_pFiler)
        return;

    m_pFiler->setFilterTypes(Types);
}

void ResListDockWidget::selectedResource(QString &name, int &type)
{
    QVariant var = m_Container->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    if (!m_pFiler)
        return;

    if (!m_List->selectionModel()->hasSelection())
        return;

    QModelIndex source = m_pFiler->mapToSource(m_List->selectionModel()->currentIndex());
    ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);
    model->getResNameAndType(source, name, type);
}

void ResListDockWidget::onDoubleClicked(const QModelIndex &index)
{
    QVariant var = m_Container->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    if (!m_pFiler)
        return;

    QModelIndex source = m_pFiler->mapToSource(index);
    ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

    int type = 0;
    QString name;
    model->getResNameAndType(source, name, type);
    emit doubleClicked(name, type);
}

void ResListDockWidget::onCustomContextMenuRequested(const QPoint &pos)
{
    QVariant var = m_Container->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    if (!m_pFiler)
        return;

    QModelIndex index = m_pFiler->mapToSource(m_List->indexAt(pos));

    if (index.isValid())
    {
        QMenu contextMenu;
        ResFilterModel *model = qobject_cast<ResFilterModel*>(m_pFiler);

        int type = 0;
        QString name;
        model->getResNameAndType(index, name, type);

        QIcon editicon = QIcon::fromTheme("EditDocument");
        QAction *edit = contextMenu.addAction(editicon, tr("Редактировать ресурс [%1]").arg(name));
        QAction *del = contextMenu.addAction(QIcon::fromTheme("DeleteTag"), tr("Удалить ресурс [%1]").arg(name));

        contextMenu.setDefaultAction(edit);
        QAction *action = contextMenu.exec(m_List->mapToGlobal(pos));

        if (action == del)
            emit deleteRequest(name, type);
        else if (action == edit)
            emit doubleClicked(name, type);
    }
}

void ResListDockWidget::ResetFilterModel(QMainWindow *Container)
{
    QVariant var = Container->property("m_pFiler");
    ResFilterModel *m_pFiler = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    if (m_pFiler)
        delete m_pFiler;

    m_pFiler = new ResFilterModel(Container);
    QTreeView *m_List = Container->findChild<QTreeView*>("pList");
    QLineEdit *m_pNameFilter = Container->findChild<QLineEdit*>("pNameFilter");
    m_List->setModel(m_pFiler);

    Container->setProperty("m_pFiler", QVariant::fromValue<ResFilterModel*>(m_pFiler));
    connect(m_pNameFilter, &QLineEdit::textChanged, m_pFiler, &ResFilterModel::setFilterName);
}

void ResListDockWidget::SetModel(QMainWindow *Container, QAbstractItemModel *model)
{
    if (!model)
        return;

    ResetFilterModel(Container);

    QVariant var = Container->property("m_pFiler");
    QVariant comp = Container->property("m_pCompleter");
    ResFilterModel *m_pFiler = nullptr;
    QCompleter *pCompletter = nullptr;

    if (var.isValid())
        m_pFiler = var.value<ResFilterModel*>();

    if (comp.isValid())
        pCompletter = comp.value<QCompleter*>();

    if (pCompletter)
        pCompletter->setModel(model);

    if (m_pFiler)
    {
        QTreeView *m_List = Container->findChild<QTreeView*>("pList");
        m_pFiler->setSourceModel(model);
        m_List->header()->resizeSection(1, 50);
        m_pFiler->sort(0);
    }
}

bool ResListDockWidget::hasSelection()
{
    return m_List->selectionModel()->hasSelection();
}

QMainWindow *ResListDockWidget::MakeContainer()
{
    QMainWindow *Container = new QMainWindow();
    QTreeView *pList = new QTreeView();
    pList->setRootIsDecorated(false);
    pList->setContextMenuPolicy(Qt::CustomContextMenu);
    pList->setObjectName("pList");

    QToolBar *pToolBar = new QToolBar(Container);
    QLineEdit *pNameFilter = new QLineEdit(Container);
    pNameFilter->setObjectName("pNameFilter");
    pNameFilter->setPlaceholderText(tr("Текст для поиска..."));
    pNameFilter->setClearButtonEnabled(true);

    pToolBar->setMovable(false);
    pToolBar->addWidget(pNameFilter);
    pToolBar->setIconSize(QSize(16,16));

    Container->setCentralWidget(pList);
    Container->addToolBar(pToolBar);

    /*QCompleter *pCompleter = new QCompleter(Container);
    pCompleter->setCaseSensitivity(Qt::CaseInsensitive);
    Container->setProperty("m_pCompleter", QVariant::fromValue(pCompleter));
    pNameFilter->setCompleter(pCompleter);*/

    return Container;
}
