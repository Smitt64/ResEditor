#ifndef ICONPROPERTYTREEITEM_H
#define ICONPROPERTYTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>

// Редактор свойства iconID: в дереве свойств отображает саму иконку
// (DecorationRole) рядом с id. Ячейочный редактор — стандартный
// PopupButtonEdit из PropertyEditor (кнопка "..." + кнопка очистки);
// базовый класс сам закрывает редактор и вызывает execPopupEditor
// (диалог выбора иконки) либо clearValue (сброс иконки, id = 0).
class IconPropertyTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    Q_INVOKABLE IconPropertyTreeItem(QObject *item, QObject *parent = nullptr);
    virtual ~IconPropertyTreeItem();

    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual bool hasPopupEditor() const Q_DECL_OVERRIDE;
    virtual bool execPopupEditor(QWidget *parent) Q_DECL_OVERRIDE;

    virtual bool canClearValue() const Q_DECL_OVERRIDE;
    virtual void clearValue() Q_DECL_OVERRIDE;
    virtual QIcon clearIcon() const Q_DECL_OVERRIDE;

private:
    quint32 iconId() const;
};

Q_DECLARE_OPAQUE_POINTER(IconPropertyTreeItem)

#endif // ICONPROPERTYTREEITEM_H
