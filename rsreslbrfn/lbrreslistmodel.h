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

    virtual void sort(int column, Qt::SortOrder order = Qt::AscendingOrder) Q_DECL_OVERRIDE;

    void addDirElement(void *elem);
    void clear();
    void removeElement(const QString &name, const int &type);

    /*virtual QModelIndexList match(const QModelIndex &start, int role,
                                  const QVariant &value, int hits = 1,
                                  Qt::MatchFlags flags = Qt::MatchFlags(Qt::MatchStartsWith|Qt::MatchWrap)) const Q_DECL_OVERRIDE;
*/
private:
    LbrResListModelPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(LbrResListModel); 
};

#endif // LBRRESLISTMODEL_H
