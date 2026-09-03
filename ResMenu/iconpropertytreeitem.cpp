#include "iconpropertytreeitem.h"
#include "iconselectdialog.h"
#include "resmenu.h"

#include <QIcon>

IconPropertyTreeItem::IconPropertyTreeItem(QObject *item, QObject *parent)
    : PropertyTreeItem{item, parent}
{

}

IconPropertyTreeItem::~IconPropertyTreeItem()
{

}

QVariant IconPropertyTreeItem::data(const int &role) const
{
    if (m_pItem)
    {
        const quint32 id = iconId();

        if (role == Qt::DecorationRole)
        {
            if (id)
                return ResMenu::inst()->getResIcon(id);

            return QVariant();
        }

        // иконки нет — вместо "0" показываем пустую строку
        if ((role == Qt::DisplayRole || role == Qt::EditRole) && !id)
            return QString();
    }

    return PropertyTreeItem::data(role);
}

bool IconPropertyTreeItem::hasPopupEditor() const
{
    return true;
}

bool IconPropertyTreeItem::execPopupEditor(QWidget *parent)
{
    if (!m_pItem)
        return false;

    // Вызывается базовым классом ПОСЛЕ закрытия ячейочного редактора —
    // писать в Q_PROPERTY безопасно (undo/reset не столкнётся с
    // открытым редактором)
    const quint32 currentId = iconId();

    bool ok = false;
    const quint32 id = IconSelectDialog::getIconId(parent, currentId, &ok);

    if (!ok || id == currentId)
        return false;

    // запись через Q_PROPERTY (MenuAction::setIconID) — попадёт
    // в undo-стек, дерево свойств обновится по NOTIFY
    m_pItem->setProperty(m_PropertyName.toLocal8Bit().data(), id);
    return true;
}

bool IconPropertyTreeItem::canClearValue() const
{
    return iconId() != 0;
}

void IconPropertyTreeItem::clearValue()
{
    if (!m_pItem)
        return;

    // запись через Q_PROPERTY (MenuAction::setIconID) — попадёт
    // в undo-стек, дерево свойств обновится по NOTIFY
    m_pItem->setProperty(m_PropertyName.toLocal8Bit().data(), 0u);
}

QIcon IconPropertyTreeItem::clearIcon() const
{
    return QIcon(QStringLiteral(":/res/ActionDelete.svg"));
}

quint32 IconPropertyTreeItem::iconId() const
{
    return m_pItem ? m_pItem->property(m_PropertyName.toLocal8Bit().data()).toUInt() : 0u;
}
