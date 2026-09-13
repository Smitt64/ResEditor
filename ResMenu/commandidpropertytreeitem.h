#ifndef COMMANDIDPROPERTYTREEITEM_H
#define COMMANDIDPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>

// Редактор свойства commandId: в дереве показывает сочетание клавиш,
// соответствующее коду (ResKeyMap), для пользовательских кодов — код
// и переопределённое сочетание (shortcutText). В режиме редактирования —
// кнопку, открывающую CommandKeyDialog.
class CommandIdPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    Q_INVOKABLE CommandIdPropertyTreeItem(QObject *item, QObject *parent = nullptr);
    virtual ~CommandIdPropertyTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;

    // Общий текст отображения кода команды + переопределённого сочетания
    static QString displayText(const int &code, const QString &shortcutText);
};

Q_DECLARE_OPAQUE_POINTER(CommandIdPropertyTreeItem)

#endif // COMMANDIDPROPERTYTREEITEM_H
