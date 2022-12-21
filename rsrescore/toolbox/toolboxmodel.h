#ifndef TOOLBOXMODEL_H
#define TOOLBOXMODEL_H

#include "qicon.h"
#include <QAbstractItemModel>
#include <QIcon>

class ToolBoxTreeItem;
class ToolBoxModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum
    {
        Column_Property,
        Column_Value
    };

    enum
    {
        MimeTypeRole = Qt::UserRole + 1,
        MimeDataRole,
        TypeItemRole,
        GroupRole
    };
    explicit ToolBoxModel(QObject *parent = nullptr);
    virtual ~ToolBoxModel();

    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    //virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex index(const ToolBoxTreeItem *item) const;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

    void addCategory(const QString &name);
    void addItem(const QStringList &category, const QString &name, QMimeData *data, const QIcon &icon = QIcon());
    ToolBoxTreeItem *category(const QString &name);
private:
    void appendItem(ToolBoxTreeItem *item);

    ToolBoxTreeItem *rootItem;
    QMap<QString, ToolBoxTreeItem*> m_Groups;
};

#endif // TOOLBOXMODEL_H
