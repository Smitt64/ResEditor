#ifndef EWTEXTSTYLEPROPERTYTREEITEM_H
#define EWTEXTSTYLEPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>
#include <QMetaEnum>

class EwTextStyle;
class EnumListModel;
class FlagPropertyTreeItem;
class EwTextStylePropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    enum NodeSubType
    {
        SubTypeRoot,
        SubTypeAlign,
        SubTypeTextColor,
        SubTypeTextBack
    };

    Q_INVOKABLE EwTextStylePropertyTreeItem(CustomRectItem *rectItem, QObject *parent = nullptr);
    EwTextStylePropertyTreeItem(const NodeSubType &type, CustomRectItem *rectItem, QObject *parent);
    virtual ~EwTextStylePropertyTreeItem();

    virtual void setData(const QVariant &value) Q_DECL_OVERRIDE;
    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

    virtual void initFromJson(const QJsonObject &obj) Q_DECL_OVERRIDE;

public slots:
    virtual void setPropertyName(const QString &name) Q_DECL_OVERRIDE;
    void propertyChanged();

private slots:
    void rectItemStyleChanged();
    void openTextStyleDialog();

private:
    NodeSubType m_SubType;
    QList<PropertyTreeItem*> m_EwChilds;
    EwTextStyle *m_pStyle;

    //QMap<QString,QString> m_AlignEnumKey, m_AlignEnum;

    //QMetaEnum m_MetaAlignEnum;
    FlagPropertyTreeItem *m_pFormat;
    EwTextStylePropertyTreeItem *m_pAlign, *m_pTextcolor, *m_pTextBack;

    EnumListModel *m_pAlignEnumModel;
    EnumListModel *m_pTextColorEnumModel;
    EnumListModel *m_pTextBackEnumModel;
};

Q_DECLARE_OPAQUE_POINTER(EwTextStylePropertyTreeItem)

#endif // EWTEXTSTYLEPROPERTYTREEITEM_H
