#include "menuundocommands.h"
#include "../editormenu.h"
#include "../editormenubar.h"
#include "../menuaction.h"
#include "../menueditorwindow.h"

#include <QMenuBar>
#include <QUndoStack>
#include <QLatin1String>

namespace {

QAction *placeholderFor(QWidget *container)
{
    if (auto *em = qobject_cast<EditorMenu *>(container))
        return em->placeHolder();
    if (auto *emb = qobject_cast<EditorMenuBar *>(container))
        return emb->placeHolder();
    return nullptr;
}

QAction *actionAfter(QWidget *container, QAction *ref)
{
    if (!container || !ref)
        return placeholderFor(container);

    const QList<QAction *> acts = [&]() -> QList<QAction *> {
        if (auto *em = qobject_cast<EditorMenu *>(container))
            return em->actions();
        if (auto *emb = qobject_cast<EditorMenuBar *>(container))
            return emb->actions();
        return QList<QAction *>();
    }();

    const int idx = acts.indexOf(ref);
    if (idx >= 0 && idx + 1 < acts.size())
        return acts.at(idx + 1);
    return placeholderFor(container);
}

void removeItem(QWidget *container, QAction *item)
{
    if (!container || !item)
        return;

    if (auto *sub = qobject_cast<EditorMenu *>(item->menu())) {
        sub->hide();
        if (auto *em = qobject_cast<EditorMenu *>(container))
            em->removeAction(item);
        else if (auto *emb = qobject_cast<EditorMenuBar *>(container))
            emb->removeAction(item);
    } else {
        if (auto *em = qobject_cast<EditorMenu *>(container))
            em->removeAction(item);
        else if (auto *emb = qobject_cast<EditorMenuBar *>(container))
            emb->removeAction(item);
    }
}

void insertItem(QWidget *container, QAction *before, QAction *item)
{
    if (!container || !item)
        return;

    if (!before)
        before = placeholderFor(container);

    if (auto *sub = qobject_cast<EditorMenu *>(item->menu())) {
        sub->setParent(container);
        if (auto *em = qobject_cast<EditorMenu *>(container))
            em->insertMenu(before, sub);
        else if (auto *emb = qobject_cast<EditorMenuBar *>(container))
            emb->insertMenu(before, sub);
    } else {
        item->setParent(container);
        if (auto *em = qobject_cast<EditorMenu *>(container))
            em->insertAction(before, item);
        else if (auto *emb = qobject_cast<EditorMenuBar *>(container))
            emb->insertAction(before, item);
    }
}

void setSelection(QWidget *container, QAction *action)
{
    if (!container)
        return;
    if (auto *em = qobject_cast<EditorMenu *>(container)) {
        if (action)
            em->selectAction(action);
        else
            em->clearSelection();
    } else if (auto *emb = qobject_cast<EditorMenuBar *>(container)) {
        if (action)
            emb->selectAction(action);
        else
            emb->clearSelection();
    }
}

} // anonymous namespace

namespace MenuUndo {

// ---------------------------------------------------------------------------
// AddMenuItemCommand
// ---------------------------------------------------------------------------
AddMenuItemCommand::AddMenuItemCommand(QWidget *container, QAction *before,
                                       ItemType type, const QString &text,
                                       QUndoCommand *parent)
    : QUndoCommand(parent),
      m_container(container),
      m_before(before),
      m_type(type),
      m_text(text)
{
    switch (m_type) {
    case ItemType::PlainItem:
        setText(QObject::tr("Добавление пункта меню"));
        break;
    case ItemType::Separator:
        setText(QObject::tr("Добавление разделителя"));
        break;
    case ItemType::Submenu:
        setText(QObject::tr("Добавление подменю"));
        break;
    }
}

QAction *AddMenuItemCommand::createdAction() const
{
    return m_action;
}

EditorMenu *AddMenuItemCommand::createdSubmenu() const
{
    return m_submenu;
}

void AddMenuItemCommand::redo()
{
    if (!m_container)
        return;

    QAction *before = m_before ? m_before.data() : placeholderFor(m_container);

    switch (m_type) {
    case ItemType::PlainItem: {
        if (!m_action)
            m_action = new MenuAction(m_text.isEmpty() ? QObject::tr("Новый пункт") : m_text, m_container);
        insertItem(m_container, before, m_action);
        setSelection(m_container, m_action);
        break;
    }
    case ItemType::Separator: {
        if (!m_action) {
            m_action = new MenuAction(m_container);
            m_action->setSeparator(true);
        }
        insertItem(m_container, before, m_action);
        setSelection(m_container, m_action);
        break;
    }
    case ItemType::Submenu: {
        if (!m_submenu) {
            m_submenu = new EditorMenu(m_text.isEmpty() ? QObject::tr("Подменю") : m_text, m_container);
        }
        m_submenu->setParent(m_container);
        if (auto *em = qobject_cast<EditorMenu *>(m_container)) {
            em->insertMenu(before, m_submenu);
            em->linkSubmenu(m_submenu);
            m_submenu->setUndoStack(em->undoStack());
        } else if (auto *emb = qobject_cast<EditorMenuBar *>(m_container)) {
            emb->insertMenu(before, m_submenu);
            emb->linkMenu(m_submenu);
            m_submenu->setUndoStack(emb->undoStack());
        }
        setSelection(m_container, m_submenu->menuAction());
        break;
    }
    }
}

void AddMenuItemCommand::undo()
{
    if (!m_container)
        return;

    switch (m_type) {
    case ItemType::PlainItem:
    case ItemType::Separator:
        if (m_action) {
            removeItem(m_container, m_action);
        }
        break;
    case ItemType::Submenu:
        if (m_submenu) {
            removeItem(m_container, m_submenu->menuAction());
        }
        break;
    }

    setSelection(m_container, nullptr);
}

// ---------------------------------------------------------------------------
// DeleteMenuItemCommand
// ---------------------------------------------------------------------------
DeleteMenuItemCommand::DeleteMenuItemCommand(QWidget *container, QAction *action,
                                             QUndoCommand *parent)
    : QUndoCommand(parent),
      m_container(container),
      m_action(action),
      m_submenu(nullptr)
{
    if (m_action) {
        m_submenu = qobject_cast<EditorMenu *>(m_action->menu());
        m_before = actionAfter(m_container, m_action);
    }

    if (m_submenu)
        setText(QObject::tr("Удаление подменю"));
    else if (action && action->isSeparator())
        setText(QObject::tr("Удаление разделителя"));
    else
        setText(QObject::tr("Удаление пункта меню"));
}

void DeleteMenuItemCommand::redo()
{
    if (!m_container || !m_action)
        return;

    removeItem(m_container, m_action);
    setSelection(m_container, nullptr);
}

void DeleteMenuItemCommand::undo()
{
    if (!m_container || !m_action)
        return;

    insertItem(m_container, m_before ? m_before.data() : placeholderFor(m_container), m_action);
    setSelection(m_container, m_action);
}

// ---------------------------------------------------------------------------
// RenameMenuItemCommand
// ---------------------------------------------------------------------------
RenameMenuItemCommand::RenameMenuItemCommand(QAction *action, const QString &newText,
                                             bool isMenuTitle, QUndoCommand *parent)
    : QUndoCommand(parent),
      m_action(action),
      m_newText(newText),
      m_isMenuTitle(isMenuTitle)
{
    if (m_isMenuTitle) {
        m_menu = qobject_cast<EditorMenu *>(action ? action->menu() : nullptr);
        m_oldText = m_menu ? m_menu->title() : QString();
        setText(QObject::tr("Переименование меню"));
    } else {
        m_oldText = action ? action->text() : QString();
        setText(QObject::tr("Переименование пункта"));
    }
}

void RenameMenuItemCommand::redo()
{
    if (m_isMenuTitle && m_menu)
        m_menu->applyTitle(m_newText);
    else if (m_action)
        m_action->setText(m_newText);
}

void RenameMenuItemCommand::undo()
{
    if (m_isMenuTitle && m_menu)
        m_menu->applyTitle(m_oldText);
    else if (m_action)
        m_action->setText(m_oldText);
}

// ---------------------------------------------------------------------------
// MoveMenuItemCommand
// ---------------------------------------------------------------------------
MoveMenuItemCommand::MoveMenuItemCommand(QWidget *srcContainer, QAction *srcBefore,
                                         QWidget *dstContainer, QAction *dstBefore,
                                         QAction *item, QUndoCommand *parent)
    : QUndoCommand(parent),
      m_isMenu(false),
      m_item(item),
      m_srcBefore(srcBefore),
      m_dstBefore(dstBefore),
      m_srcContainer(srcContainer),
      m_dstContainer(dstContainer)
{
    if (m_item)
        m_isMenu = (qobject_cast<EditorMenu *>(m_item->menu()) != nullptr);

    setText(QObject::tr("Перемещение пункта меню"));
}

void MoveMenuItemCommand::redo()
{
    if (!m_item || !m_srcContainer || !m_dstContainer)
        return;

    removeItem(m_srcContainer, m_item);
    insertItem(m_dstContainer, m_dstBefore ? m_dstBefore.data() : placeholderFor(m_dstContainer), m_item);

    if (m_isMenu) {
        if (auto *sub = qobject_cast<EditorMenu *>(m_item->menu())) {
            if (auto *em = qobject_cast<EditorMenu *>(m_dstContainer)) {
                em->linkSubmenu(sub);
                sub->setUndoStack(em->undoStack());
            } else if (auto *emb = qobject_cast<EditorMenuBar *>(m_dstContainer)) {
                emb->linkMenu(sub);
                sub->setUndoStack(emb->undoStack());
            }
        }
    }

    setSelection(m_dstContainer, m_item);
}

void MoveMenuItemCommand::undo()
{
    if (!m_item || !m_srcContainer || !m_dstContainer)
        return;

    removeItem(m_dstContainer, m_item);
    insertItem(m_srcContainer, m_srcBefore ? m_srcBefore.data() : placeholderFor(m_srcContainer), m_item);

    if (m_isMenu) {
        if (auto *sub = qobject_cast<EditorMenu *>(m_item->menu())) {
            if (auto *em = qobject_cast<EditorMenu *>(m_srcContainer)) {
                em->linkSubmenu(sub);
                sub->setUndoStack(em->undoStack());
            } else if (auto *emb = qobject_cast<EditorMenuBar *>(m_srcContainer)) {
                emb->linkMenu(sub);
                sub->setUndoStack(emb->undoStack());
            }
        }
    }

    setSelection(m_srcContainer, m_item);
}

// ---------------------------------------------------------------------------
// ConvertToSubmenuCommand
// ---------------------------------------------------------------------------
ConvertToSubmenuCommand::ConvertToSubmenuCommand(EditorMenu *parentMenu, QAction *action,
                                                 QUndoCommand *parent)
    : QUndoCommand(parent),
      m_parentMenu(parentMenu),
      m_plainAction(action),
      m_submenu(nullptr),
      m_oldText(action ? action->text() : QString())
{
    if (m_parentMenu && m_plainAction)
        m_before = actionAfter(m_parentMenu, m_plainAction);
    setText(QObject::tr("Преобразование в подменю"));
}

EditorMenu *ConvertToSubmenuCommand::createdSubmenu() const
{
    return m_submenu;
}

void ConvertToSubmenuCommand::redo()
{
    if (!m_parentMenu || !m_plainAction)
        return;

    if (!m_submenu) {
        m_submenu = new EditorMenu(m_oldText, m_parentMenu);
    }

    removeItem(m_parentMenu, m_plainAction);
    m_submenu->setParent(m_parentMenu);
    m_parentMenu->insertMenu(m_before ? m_before.data() : m_parentMenu->placeHolder(), m_submenu);
    m_parentMenu->linkSubmenu(m_submenu);
    m_submenu->setUndoStack(m_parentMenu->undoStack());
    setSelection(m_parentMenu, m_submenu->menuAction());
    m_parentMenu->openSubmenu(m_submenu);
}

void ConvertToSubmenuCommand::undo()
{
    if (!m_parentMenu || !m_submenu)
        return;

    removeItem(m_parentMenu, m_submenu->menuAction());
    m_plainAction->setParent(m_parentMenu);
    m_parentMenu->insertAction(m_before ? m_before.data() : m_parentMenu->placeHolder(), m_plainAction);
    setSelection(m_parentMenu, m_plainAction);
}

// ---------------------------------------------------------------------------
// CollapseSubmenuCommand
// ---------------------------------------------------------------------------
CollapseSubmenuCommand::CollapseSubmenuCommand(EditorMenu *parentMenu, EditorMenu *submenu,
                                               QUndoCommand *parent)
    : QUndoCommand(parent),
      m_parentMenu(parentMenu),
      m_submenu(submenu),
      m_plainAction(nullptr)
{
    if (m_parentMenu && m_submenu)
        m_before = actionAfter(m_parentMenu, m_submenu->menuAction());
    setText(QObject::tr("Сворачивание подменю"));
}

void CollapseSubmenuCommand::redo()
{
    if (!m_parentMenu || !m_submenu)
        return;

    const QString title = m_submenu->title();

    removeItem(m_parentMenu, m_submenu->menuAction());

    if (!m_plainAction) {
        m_plainAction = new MenuAction(title, m_parentMenu);
    } else {
        m_plainAction->setText(title);
        m_plainAction->setParent(m_parentMenu);
    }

    m_parentMenu->insertAction(m_before ? m_before.data() : m_parentMenu->placeHolder(), m_plainAction);
    setSelection(m_parentMenu, m_plainAction);
}

void CollapseSubmenuCommand::undo()
{
    if (!m_parentMenu || !m_submenu || !m_plainAction)
        return;

    removeItem(m_parentMenu, m_plainAction);
    m_submenu->setParent(m_parentMenu);
    m_parentMenu->insertMenu(m_before ? m_before.data() : m_parentMenu->placeHolder(), m_submenu);
    m_parentMenu->linkSubmenu(m_submenu);
    m_submenu->setUndoStack(m_parentMenu->undoStack());
    setSelection(m_parentMenu, m_submenu->menuAction());
    m_parentMenu->openSubmenu(m_submenu);
}

// ---------------------------------------------------------------------------
// ChangeMenuPropertyCommand
// ---------------------------------------------------------------------------
ChangeMenuPropertyCommand::ChangeMenuPropertyCommand(QObject *target, const QString &propertyName,
                                                     const QVariant &oldValue, const QVariant &newValue,
                                                     QUndoCommand *parent)
    : QUndoCommand(parent),
      m_target(target),
      m_propertyName(propertyName),
      m_oldValue(oldValue),
      m_newValue(newValue)
{
    setText(QObject::tr("Изменение свойства [%1]").arg(propertyName));
}

void ChangeMenuPropertyCommand::applyValue(const QVariant &value)
{
    if (!m_target)
        return;

    bool oldSkip = false;
    if (auto *ma = qobject_cast<MenuAction *>(m_target.data()))
        oldSkip = ma->setSkipUndoStack(true);
    else if (auto *em = qobject_cast<EditorMenu *>(m_target.data()))
        oldSkip = em->setSkipUndoStack(true);

    m_target->setProperty(m_propertyName.toLocal8Bit().data(), value);

    if (auto *ma = qobject_cast<MenuAction *>(m_target.data()))
        ma->setSkipUndoStack(oldSkip);
    else if (auto *em = qobject_cast<EditorMenu *>(m_target.data()))
        em->setSkipUndoStack(oldSkip);
}

void ChangeMenuPropertyCommand::redo()
{
    applyValue(m_newValue);
}

void ChangeMenuPropertyCommand::undo()
{
    applyValue(m_oldValue);
}

// ---------------------------------------------------------------------------
// ChangeMenuCommentCommand
// ---------------------------------------------------------------------------
ChangeMenuCommentCommand::ChangeMenuCommentCommand(MenuEditorWindow *wnd, const QString &newText,
                                                   QUndoCommand *parent)
    : QUndoCommand(parent),
      m_wnd(wnd),
      m_newText(newText)
{
    m_oldText = wnd ? wnd->menuComment() : QString();
    setText(QObject::tr("Изменение комментария меню"));
}

void ChangeMenuCommentCommand::redo()
{
    if (m_wnd)
        m_wnd->applyComment(m_newText);
}

void ChangeMenuCommentCommand::undo()
{
    if (m_wnd)
        m_wnd->applyComment(m_oldText);
}

} // namespace MenuUndo
