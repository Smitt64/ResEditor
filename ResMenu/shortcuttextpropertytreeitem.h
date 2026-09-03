#ifndef SHORTCUTTEXTPROPERTYTREEITEM_H
#define SHORTCUTTEXTPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>

// Редактор свойства shortcutText (сочетание-переопределение для
// пользовательских команд): inline QKeySequenceEdit, значение хранится
// как portable text.
class ShortcutTextPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    Q_INVOKABLE ShortcutTextPropertyTreeItem(QObject *item, QObject *parent = nullptr);
    virtual ~ShortcutTextPropertyTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

Q_DECLARE_OPAQUE_POINTER(ShortcutTextPropertyTreeItem)

#endif // SHORTCUTTEXTPROPERTYTREEITEM_H
