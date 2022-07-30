#ifndef RESLISTDOCKWIDGET_H
#define RESLISTDOCKWIDGET_H

#include "rsrescore_global.h"
#include <QDockWidget>
#include <QSortFilterProxyModel>

class QTreeView;
class QMainWindow;
class QAbstractItemModel;
class QLineEdit;
class QToolBar;
class QToolButton;
class ResFilterModel;
class RSRESCORE_EXPORT ResListDockWidget : public QDockWidget
{
    Q_OBJECT
public:
    ResListDockWidget(QWidget *parent = nullptr);
    virtual ~ResListDockWidget();

    void setModel(QAbstractItemModel *model);

private slots:
    void onDoubleClicked(const QModelIndex &index);

signals:
    void doubleClicked(const QModelIndex &index);

private:
    QMainWindow *m_Container;
    QTreeView *m_List;
    QLineEdit *m_pNameFilter;
    QToolBar *m_pToolBar;
    QToolButton *m_FilterBtn;
    ResFilterModel *m_pFiler;
};

class ResFilterModel : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    ResFilterModel(QObject *parent = nullptr);
    virtual ~ResFilterModel();

public slots:
    void setFilterName(const QString &name);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_FINAL;

private:
    QString m_FilterName;
};

#endif // RESLISTDOCKWIDGET_H
