#ifndef ENUMPROPERTYTREEITEM_H
#define ENUMPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>
#include <QJsonArray>
#include <QMap>

class EnumListModel;
class EnumPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    explicit EnumPropertyTreeItem(CustomRectItem *rectItem, QObject *parent = nullptr);
    virtual ~EnumPropertyTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    void loadEnumAlias(const QJsonArray &array);
    void loadEnumAlias(const QString &filename);

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

public slots:
    virtual void setPropertyName(const QString &name) Q_DECL_OVERRIDE;

private:
    typedef QMap<QString,QString> EnumAliasData;
    EnumAliasData m_KeyAlias, m_AliasKey;

    EnumListModel *m_pModel;
};

#endif // ENUMPROPERTYTREEITEM_H
