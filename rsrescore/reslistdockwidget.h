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
    void doubleClicked(const QString &name, const int &type);

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
    enum
    {
        fldName,
        fldType,
        fldComment
    };

    enum
    {
        ColumnName,
        ColumnType,
        ColumnComment,
        ColumnTime,

        ColumnCount
    };

    ResFilterModel(QObject *parent = nullptr);
    virtual ~ResFilterModel();

    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void getResNameAndType(const QModelIndex &index, QString &name, int &type);

public slots:
    void setFilterName(const QString &name);

protected:
    virtual bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const Q_DECL_FINAL;

private:
    QString m_FilterName;
};

#endif // RESLISTDOCKWIDGET_H
