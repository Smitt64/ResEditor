#include "commandidpropertytreeitem.h"
#include "commandkeydialog.h"
#include "menuaction.h"
#include "reskeymap.h"

#include <QKeySequence>
#include <QPointer>
#include <QPushButton>
#include <QUndoStack>

// ---------------------------------------------------------------------------
// Кнопка-редактор: показывает текущий код/сочетание, по клику открывает
// CommandKeyDialog. Q_OBJECT не нужен — своих сигналов/слотов нет.
// ---------------------------------------------------------------------------

class CommandKeyEdit : public QPushButton
{
public:
    explicit CommandKeyEdit(QWidget *parent = nullptr)
        : QPushButton(parent)
    {
        connect(this, &QPushButton::clicked, this, [this]()
        {
            // Делегат редактора свойств уничтожает эту кнопку при потере
            // фокуса, т.е. сразу при открытии диалога. После exec() к
            // членам this обращаться НЕЛЬЗЯ — всё нужное копируем заранее.
            const QPointer<QObject> target = m_target;
            const QString prop = m_prop;
            const int currentCode = m_code;
            const QString currentShortcut = m_shortcutText;
            QWidget *dialogParent = window();

            int code = currentCode;
            QString shortcutText = currentShortcut;
            if (!CommandKeyDialog::getCommand(dialogParent, code, shortcutText))
                return;

            if (!target)
                return;

            if (code == currentCode && shortcutText == currentShortcut)
                return;

            // Два свойства — один шаг отката
            QUndoStack *stack = nullptr;
            if (MenuAction *action = qobject_cast<MenuAction*>(target.data()))
                stack = action->undoStack();

            if (stack)
                stack->beginMacro(tr("Изменение идентификатора команды"));

            // запись через Q_PROPERTY (MenuAction::setCommandId/
            // setShortcutText) — попадёт в undo-стек, дерево свойств
            // обновится по NOTIFY
            target->setProperty(prop.toLocal8Bit().data(), code);
            target->setProperty("shortcutText", shortcutText);

            if (stack)
                stack->endMacro();
        });
    }

    // объект и имя свойства, куда писать код команды
    void setPropertyTarget(QObject *obj, const QString &prop)
    {
        m_target = obj;
        m_prop = prop;
    }

    int commandCode() const
    {
        return m_code;
    }

    void setCommand(const int &code, const QString &shortcutText)
    {
        m_code = code;
        m_shortcutText = shortcutText;
        setText(CommandIdPropertyTreeItem::displayText(code, shortcutText));
    }

private:
    int m_code = 0;
    QString m_shortcutText;
    QPointer<QObject> m_target;
    QString m_prop;
};

// ---------------------------------------------------------------------------
// CommandIdPropertyTreeItem
// ---------------------------------------------------------------------------

CommandIdPropertyTreeItem::CommandIdPropertyTreeItem(QObject *item, QObject *parent)
    : PropertyTreeItem{item, parent}
{
}

CommandIdPropertyTreeItem::~CommandIdPropertyTreeItem()
{
}

QString CommandIdPropertyTreeItem::displayText(const int &code, const QString &shortcutText)
{
    // Сочетание: переопределённое (shortcutText) либо деривированное
    // из кода; для служебных кодов — символическое имя
    QString shortcut;

    if (!shortcutText.isEmpty())
    {
        const QKeySequence ov = QKeySequence::fromString(shortcutText, QKeySequence::PortableText);
        if (!ov.isEmpty())
            shortcut = ov.toString(QKeySequence::NativeText);
    }

    if (shortcut.isEmpty())
    {
        const QKeySequence derived = ResKeyMap::keySequenceFromCode(code);
        if (!derived.isEmpty())
            shortcut = derived.toString(QKeySequence::NativeText);
        else
            shortcut = ResKeyMap::nameFromCode(code);
    }

    if (!shortcut.isEmpty())
        return QStringLiteral("%1 (%2)").arg(shortcut).arg(code);

    if (code == 0)
        return tr("Нет");

    return QString::number(code);
}

QVariant CommandIdPropertyTreeItem::data(const int &role) const
{
    if (role == Qt::DisplayRole && m_pItem)
    {
        const int code = m_pItem->property(m_PropertyName.toLocal8Bit().data()).toInt();
        const QString shortcutText = m_pItem->property("shortcutText").toString();
        return displayText(code, shortcutText);
    }

    return PropertyTreeItem::data(role);
}

QWidget *CommandIdPropertyTreeItem::createEditor(QWidget *wparent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    Q_UNUSED(option)
    Q_UNUSED(index)

    CommandKeyEdit *edit = new CommandKeyEdit(wparent);
    edit->setPropertyTarget(m_pItem, m_PropertyName);
    return edit;
}

bool CommandIdPropertyTreeItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    Q_UNUSED(index)

    CommandKeyEdit *edit = dynamic_cast<CommandKeyEdit*>(editor);
    if (!edit)
        return false;

    edit->setCommand(m_pItem->property(m_PropertyName.toLocal8Bit().data()).toInt(),
                     m_pItem->property("shortcutText").toString());
    return true;
}

bool CommandIdPropertyTreeItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    Q_UNUSED(model)
    Q_UNUSED(index)

    CommandKeyEdit *edit = dynamic_cast<CommandKeyEdit*>(editor);
    if (!edit)
        return false;

    // Значение записывается в обработчике клика (диалог меняет сразу
    // commandId и shortcutText одним undo-макросом); здесь только
    // проверяем, что редактор жив
    return true;
}
