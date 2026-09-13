#ifndef MENUUNDOCOMMANDS_H
#define MENUUNDOCOMMANDS_H

#include <QObject>
#include <QPointer>
#include <QUndoCommand>
#include <QVariant>

class QAction;
class EditorMenu;
class EditorMenuBar;
class MenuEditorWindow;
class QWidget;

namespace MenuUndo {

enum class ItemType
{
    PlainItem,
    Separator,
    Submenu
};

// ---------------------------------------------------------------------------
// Добавление пункта/разделителя/подменю
// ---------------------------------------------------------------------------
class AddMenuItemCommand : public QUndoCommand
{
public:
    AddMenuItemCommand(QWidget *container, QAction *before, ItemType type,
                       const QString &text, QUndoCommand *parent = nullptr);

    QAction *createdAction() const;
    EditorMenu *createdSubmenu() const;

    void redo() override;
    void undo() override;

private:
    QPointer<QWidget> m_container;
    QPointer<QAction> m_before;
    ItemType m_type;
    QString m_text;

    QPointer<QAction> m_action;      // для PlainItem / Separator
    QPointer<EditorMenu> m_submenu; // для Submenu
};

// ---------------------------------------------------------------------------
// Удаление пункта/подменю
// ---------------------------------------------------------------------------
class DeleteMenuItemCommand : public QUndoCommand
{
public:
    DeleteMenuItemCommand(QWidget *container, QAction *action,
                          QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<QWidget> m_container;
    QPointer<QAction> m_action;
    QPointer<EditorMenu> m_submenu;
    QPointer<QAction> m_before;
};

// ---------------------------------------------------------------------------
// Переименование пункта или заголовка меню
// ---------------------------------------------------------------------------
class RenameMenuItemCommand : public QUndoCommand
{
public:
    RenameMenuItemCommand(QAction *action, const QString &newText,
                          bool isMenuTitle, QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<QAction> m_action;
    QPointer<EditorMenu> m_menu;
    QString m_oldText;
    QString m_newText;
    bool m_isMenuTitle;
};

// ---------------------------------------------------------------------------
// Перемещение пункта/подменю между контейнерами или внутри одного
// ---------------------------------------------------------------------------
class MoveMenuItemCommand : public QUndoCommand
{
public:
    MoveMenuItemCommand(QWidget *srcContainer, QAction *srcBefore,
                        QWidget *dstContainer, QAction *dstBefore,
                        QAction *item, QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    bool m_isMenu;
    QPointer<QAction> m_item;
    QPointer<QAction> m_srcBefore;
    QPointer<QAction> m_dstBefore;
    QPointer<QWidget> m_srcContainer;
    QPointer<QWidget> m_dstContainer;
};

// ---------------------------------------------------------------------------
// Преобразование обычного пункта в подменю
// ---------------------------------------------------------------------------
class ConvertToSubmenuCommand : public QUndoCommand
{
public:
    ConvertToSubmenuCommand(EditorMenu *parentMenu, QAction *action,
                            QUndoCommand *parent = nullptr);

    EditorMenu *createdSubmenu() const;

    void redo() override;
    void undo() override;

private:
    QPointer<EditorMenu> m_parentMenu;
    QPointer<QAction> m_plainAction;
    QPointer<EditorMenu> m_submenu;
    QPointer<QAction> m_before;
    QString m_oldText;
};

// ---------------------------------------------------------------------------
// Сворачивание пустого подменю в обычный пункт
// ---------------------------------------------------------------------------
class CollapseSubmenuCommand : public QUndoCommand
{
public:
    CollapseSubmenuCommand(EditorMenu *parentMenu, EditorMenu *submenu,
                           QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<EditorMenu> m_parentMenu;
    QPointer<EditorMenu> m_submenu;
    QPointer<QAction> m_plainAction;
    QPointer<QAction> m_before;
};

// ---------------------------------------------------------------------------
// Изменение произвольного свойства пункта или меню (helpId, commandId, ...)
// ---------------------------------------------------------------------------
class ChangeMenuPropertyCommand : public QUndoCommand
{
public:
    ChangeMenuPropertyCommand(QObject *target, const QString &propertyName,
                              const QVariant &oldValue, const QVariant &newValue,
                              QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    void applyValue(const QVariant &value);

    QPointer<QObject> m_target;
    QString m_propertyName;
    QVariant m_oldValue;
    QVariant m_newValue;
};

// ---------------------------------------------------------------------------
// Изменение комментария меню (MenuEditorWindow::m_menuComment)
// ---------------------------------------------------------------------------
class ChangeMenuCommentCommand : public QUndoCommand
{
public:
    ChangeMenuCommentCommand(MenuEditorWindow *wnd, const QString &newText,
                             QUndoCommand *parent = nullptr);

    void redo() override;
    void undo() override;

private:
    QPointer<MenuEditorWindow> m_wnd;
    QString m_oldText;
    QString m_newText;
};

} // namespace MenuUndo

#endif // MENUUNDOCOMMANDS_H
