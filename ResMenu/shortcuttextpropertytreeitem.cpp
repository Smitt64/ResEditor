#include "shortcuttextpropertytreeitem.h"

#include <QKeySequence>
#include <QKeySequenceEdit>

ShortcutTextPropertyTreeItem::ShortcutTextPropertyTreeItem(QObject *item, QObject *parent)
    : PropertyTreeItem{item, parent}
{
}

ShortcutTextPropertyTreeItem::~ShortcutTextPropertyTreeItem()
{
}

QVariant ShortcutTextPropertyTreeItem::data(const int &role) const
{
    if (role == Qt::DisplayRole && m_pItem)
    {
        const QString text = m_pItem->property(m_PropertyName.toLocal8Bit().data()).toString();
        if (text.isEmpty())
            return tr("Нет");

        const QKeySequence seq = QKeySequence::fromString(text, QKeySequence::PortableText);
        return seq.toString(QKeySequence::NativeText);
    }

    return PropertyTreeItem::data(role);
}

QWidget *ShortcutTextPropertyTreeItem::createEditor(QWidget *wparent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    return new QKeySequenceEdit(wparent);
}

bool ShortcutTextPropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(index)

    QKeySequenceEdit *edit = dynamic_cast<QKeySequenceEdit*>(editor);
    if (!edit)
        return false;

    const QString text = m_pItem->property(m_PropertyName.toLocal8Bit().data()).toString();
    edit->setKeySequence(QKeySequence::fromString(text, QKeySequence::PortableText));
    return true;
}

bool ShortcutTextPropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(model)
    Q_UNUSED(index)

    QKeySequenceEdit *edit = dynamic_cast<QKeySequenceEdit*>(editor);
    if (!edit)
        return false;

    const QString text = edit->keySequence().toString(QKeySequence::PortableText);
    if (m_pItem->property(m_PropertyName.toLocal8Bit().data()).toString() == text)
        return true;

    // запись через Q_PROPERTY (MenuAction::setShortcutText) — попадёт
    // в undo-стек, сочетание в пункте меню пересчитается в сеттере
    m_pItem->setProperty(m_PropertyName.toLocal8Bit().data(), text);
    return true;
}
