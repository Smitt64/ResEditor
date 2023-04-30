#ifndef LBRRESLISTMODEL_H
#define LBRRESLISTMODEL_H

#include "rsreslbrfn_global.h"
#include <QAbstractTableModel>

class LbrResListModelPrivate;
class RSRESLBRFN_EXPORT LbrResListModel : public QAbstractTableModel
{
    Q_OBJECT
    friend class LbrObjectPrivate;
public:
    enum
    {
        ColumnName,
        ColumnType,
        ColumnComment,
        ColumnTime,

        ColumnCount
    };
    LbrResListModel(QObject *parent = nullptr);
    virtual ~LbrResListModel();

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

private:
    LbrResListModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(LbrResListModel);

    void addDirElement(void *elem);
};

#endif // LBRRESLISTMODEL_H
