#ifndef MENUACTION_H
#define MENUACTION_H

#include <QAction>
#include <QJsonObject>
#include <QKeySequence>
#include <QVariant>
#include <customrectitem.h>

class QUndoStack;

class MenuAction : public QAction
{
    Q_OBJECT
    Q_FLAGS(ActionFlags)

    Q_PROPERTY(int helpId READ helpId WRITE setHelpId NOTIFY helpIdChanged)
    Q_PROPERTY(int commandId READ commandId WRITE setCommandId NOTIFY commandIdChanged)
    Q_PROPERTY(QString shortcutText READ shortcutText WRITE setShortcutText NOTIFY shortcutTextChanged)
    Q_PROPERTY(quint32 cmdEx READ cmdEx WRITE setCmdEx NOTIFY cmdExChanged)
    Q_PROPERTY(quint32 iconID READ iconID WRITE setIconID NOTIFY iconIDChanged)
    Q_PROPERTY(ActionFlags actionFlags READ actionFlags WRITE setActionFlags NOTIFY actionFlagsChanged)
    Q_PROPERTY(bool disabledInEmptyScroll READ disabledInEmptyScroll WRITE setDisabledInEmptyScroll NOTIFY disabledInEmptyScrollChanged)

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "MENUACTION")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/MenuItem.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "MenuAction")
public:
    // Битовые флаги dFMENU_* из старого формата ресурса меню.
    enum class ActionFlag : quint32
    {
        None    = 0x00000000,
        Toolbar = 0x00000001, // dFMENU_TOOLBAR
        Context = 0x00000002  // dFMENU_CONTEXT
    };
    Q_ENUM(ActionFlag)
    Q_DECLARE_FLAGS(ActionFlags, ActionFlag)

    // DISINEMPTYSCROL — пункт меню недоступен в пустом скроллинге.
    // Бит из того же поля flags, но редактируется отдельным bool-свойством
    static constexpr quint32 FlagDisabledInEmptyScroll = 0x04;

    bool setSkipUndoStack(bool value);
    bool isSkipUndoStack() const;

    QUndoStack *undoStack() const;

protected:
    bool checkPropSameValue(const QString &propertyName, const QVariant &value);
    void pushUndoPropertyData(const QString &propertyName, const QVariant &_newValue);

public:
    explicit MenuAction(QObject *parent = nullptr);
    explicit MenuAction(const QString &text, QObject *parent = nullptr);

    int helpId() const;
    void setHelpId(int helpId);

    int commandId() const;
    void setCommandId(int commandId);

    // Сочетание клавиш в portable text (QKeySequence). Для кодов клавиш
    // (< K_USER) обычно пусто — сочетание деривируется из commandId;
    // для пользовательских команд (>= K_USER) хранит сочетание, которому
    // нет кода в TAllKeys. Явно заданное значение всегда выигрывает.
    QString shortcutText() const;
    void setShortcutText(const QString &shortcutText);

    // Отображаемое в пункте меню сочетание: shortcutText, если задан,
    // иначе сочетание, соответствующее commandId (ResKeyMap).
    QKeySequence effectiveShortcut() const;

    quint32 cmdEx() const;
    void setCmdEx(quint32 cmdEx);

    quint32 iconID() const;
    void setIconID(quint32 iconID);

    ActionFlags actionFlags() const;
    void setActionFlags(ActionFlags flags);

    // true — установлен бит FlagDisabledInEmptyScroll (DISINEMPTYSCROL)
    bool disabledInEmptyScroll() const;
    void setDisabledInEmptyScroll(bool disabled);

    QJsonObject serialize() const;
    void deserialize(const QJsonObject &obj);

    static MenuAction *clone(const MenuAction *source, QObject *parent);

signals:
    void helpIdChanged(int helpId);
    void commandIdChanged(int commandId);
    void shortcutTextChanged(const QString &shortcutText);
    void cmdExChanged(quint32 cmdEx);
    void iconIDChanged(quint32 iconID);
    void actionFlagsChanged(MenuAction::ActionFlags flags);
    void disabledInEmptyScrollChanged(bool disabled);

private:
    // QAction::shortcut — проекция effectiveShortcut(), пересчитывается
    // при смене commandId/shortcutText
    void updateShortcutProjection();

    bool m_skipUndoStack = false;

    int m_helpId = 0;
    int m_commandId = 0;
    QString m_shortcutText;
    quint32 m_cmdEx = 0;
    quint32 m_iconID = 0;
    quint32 m_flags = 0;
};

Q_DECLARE_OPERATORS_FOR_FLAGS(MenuAction::ActionFlags)
Q_DECLARE_METATYPE(MenuAction::ActionFlags)

#endif // MENUACTION_H
