#ifndef FLAGPROPERTYTREEITEM_H
#define FLAGPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>
#include <QJsonArray>

class FlagPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    enum NodeSubType
    {
        SubTypeRoot,
        SubTypeFlag,
    };

    explicit FlagPropertyTreeItem(CustomRectItem *rectItem, const NodeSubType &type = SubTypeRoot, QObject *parent = nullptr);
    virtual ~FlagPropertyTreeItem();

    virtual void setData(const QVariant &value) Q_DECL_OVERRIDE;
    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QString propertyAlias() const Q_DECL_OVERRIDE;

    void loadEnumAlias(const QJsonArray &array);

public slots:
    virtual void setPropertyName(const QString &name) Q_DECL_OVERRIDE;

private:
    typedef QMap<QString,QString> EnumAliasData;
    EnumAliasData m_KeyAlias, m_AliasKey;

    void addChild(const QString &flagName);
    NodeSubType m_SubType;
    QString m_flagName;
};

#endif // FLAGPROPERTYTREEITEM_H
