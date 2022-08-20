#ifndef PROPERTYMODEL_H
#define PROPERTYMODEL_H

#include <QAbstractItemModel>
#include <QObject>

class CustomRectItem;
class PropertyTreeItem;
class PropertyModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    enum
    {
        Column_Property,
        Column_Value
    };
    explicit PropertyModel(CustomRectItem *item, QObject *parent = nullptr);
    virtual ~PropertyModel();

    virtual QVariant data(const QModelIndex &index, int role) const Q_DECL_OVERRIDE;
    virtual bool setData(const QModelIndex &index, const QVariant &value, int role = Qt::EditRole) Q_DECL_OVERRIDE;

    virtual Qt::ItemFlags flags(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    QModelIndex index(const PropertyTreeItem *item) const;
    virtual QModelIndex parent(const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;

private:
    void appendItem(PropertyTreeItem *item);
    void addProperty(const QJsonObject &obj, const QMetaObject *meta, const int &group);
    void setupModelData(CustomRectItem *item, PropertyTreeItem *parent);
    QString getPropertyListJson(const QMetaObject *obj);
    QString getClassInfo(const QMetaObject *obj, const char *name);
    CustomRectItem *m_pItem;

    PropertyTreeItem *rootItem;
};

#endif // PROPERTYMODEL_H
