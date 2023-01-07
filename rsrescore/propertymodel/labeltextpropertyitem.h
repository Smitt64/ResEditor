#ifndef LABELTEXTPROPERTYITEM_H
#define LABELTEXTPROPERTYITEM_H

#include "propertytreeitem.h"

class QGraphicsObject;
class LabelTextPropertyItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    Q_INVOKABLE LabelTextPropertyItem(QObject *rectItem, QObject *parent = nullptr);
    virtual ~LabelTextPropertyItem();

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

Q_DECLARE_OPAQUE_POINTER(LabelTextPropertyItem)

#endif // LABELTEXTPROPERTYITEM_H
