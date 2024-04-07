#ifndef SUBWINDOWSMODEL_H
#define SUBWINDOWSMODEL_H

#include <QAbstractListModel>

class QMdiSubWindow;
class SubWindowsItem;
class SubWindowsModel : public QAbstractListModel
{
    Q_OBJECT
public:
    explicit SubWindowsModel(QObject *parent = nullptr);
    virtual ~SubWindowsModel();

    QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    QModelIndex addWindow(QMdiSubWindow *wnd);

    QModelIndex findWindow(QMdiSubWindow *wnd);
    QMdiSubWindow *window(const QModelIndex &index);

signals:
    void windowsUpdated();

private slots:
    void windowClosed(QObject*);

private:
    QList<SubWindowsItem*> m_Windows;
};

#endif // SUBWINDOWSMODEL_H
