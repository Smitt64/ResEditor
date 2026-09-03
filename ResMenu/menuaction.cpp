#include "menuaction.h"

#include "editormenu.h"
#include "editormenubar.h"
#include "undoredo/menuundocommands.h"
#include "reskeymap.h"
#include "resmenu.h"

#include <QKeySequence>
#include <QMetaType>
#include <QUndoStack>

static quint32 actionFlagsToUInt(const QVariant &value)
{
    if (value.canConvert<MenuAction::ActionFlags>())
        return static_cast<quint32>(value.value<MenuAction::ActionFlags>());
    return value.toUInt();
}

namespace {
// Без этих конвертеров QVariant::toInt()/value<quint32>() для пользовательского
// типа MenuAction::ActionFlags возвращают 0 — редактор свойств читает флаги
// именно через QVariant.
bool registerActionFlagsConverters()
{
    QMetaType::registerConverter<MenuAction::ActionFlags, int>(
        [](const MenuAction::ActionFlags &flags) { return static_cast<int>(flags); });
    QMetaType::registerConverter<MenuAction::ActionFlags, quint32>(
        [](const MenuAction::ActionFlags &flags) { return static_cast<quint32>(flags); });
    return true;
}

const bool actionFlagsConvertersRegistered = registerActionFlagsConverters();
} // namespace

MenuAction::MenuAction(QObject *parent)
    : QAction(parent)
{
}

MenuAction::MenuAction(const QString &text, QObject *parent)
    : QAction(text, parent)
{
}

bool MenuAction::setSkipUndoStack(bool value)
{
    const bool old = m_skipUndoStack;
    m_skipUndoStack = value;
    return old;
}

bool MenuAction::isSkipUndoStack() const
{
    if (m_skipUndoStack)
        return true;
    if (auto *em = qobject_cast<EditorMenu *>(parent()))
        return em->isSkipUndoStack();
    if (auto *emb = qobject_cast<EditorMenuBar *>(parent()))
        return emb->isSkipUndoStack();
    return false;
}

QUndoStack *MenuAction::undoStack() const
{
    if (auto *em = qobject_cast<EditorMenu *>(parent()))
        return em->undoStack();
    if (auto *emb = qobject_cast<EditorMenuBar *>(parent()))
        return emb->undoStack();
    return nullptr;
}

bool MenuAction::checkPropSameValue(const QString &propertyName, const QVariant &value)
{
    if (propertyName == QLatin1String("helpId"))
        return m_helpId == value.toInt();
    if (propertyName == QLatin1String("commandId"))
        return m_commandId == value.toInt();
    if (propertyName == QLatin1String("shortcutText"))
        return m_shortcutText == value.toString();
    if (propertyName == QLatin1String("cmdEx"))
        return m_cmdEx == value.toUInt();
    if (propertyName == QLatin1String("iconID"))
        return m_iconID == value.toUInt();
    if (propertyName == QLatin1String("actionFlags"))
        return m_flags == actionFlagsToUInt(value);
    if (propertyName == QLatin1String("disabledInEmptyScroll"))
        return disabledInEmptyScroll() == value.toBool();
    return property(propertyName.toLocal8Bit().data()) == value;
}

void MenuAction::pushUndoPropertyData(const QString &propertyName, const QVariant &_newValue)
{
    QUndoStack *stack = undoStack();
    if (!stack)
        return;

    QVariant oldValue;
    if (propertyName == QLatin1String("helpId"))
        oldValue = m_helpId;
    else if (propertyName == QLatin1String("commandId"))
        oldValue = m_commandId;
    else if (propertyName == QLatin1String("shortcutText"))
        oldValue = m_shortcutText;
    else if (propertyName == QLatin1String("cmdEx"))
        oldValue = m_cmdEx;
    else if (propertyName == QLatin1String("iconID"))
        oldValue = m_iconID;
    else if (propertyName == QLatin1String("actionFlags"))
        oldValue = QVariant::fromValue(actionFlags());
    else if (propertyName == QLatin1String("disabledInEmptyScroll"))
        oldValue = disabledInEmptyScroll();
    else
        oldValue = property(propertyName.toLocal8Bit().data());

    stack->push(new MenuUndo::ChangeMenuPropertyCommand(
        this, propertyName, oldValue, _newValue));
}

int MenuAction::helpId() const
{
    return m_helpId;
}

void MenuAction::setHelpId(int helpId)
{
    if (checkPropSameValue("helpId", helpId))
        return;
    if (isSkipUndoStack() || !undoStack()) {
        m_helpId = helpId;
        emit helpIdChanged(m_helpId);
    } else
        pushUndoPropertyData("helpId", helpId);
}

int MenuAction::commandId() const
{
    return m_commandId;
}

void MenuAction::setCommandId(int commandId)
{
    if (checkPropSameValue("commandId", commandId))
        return;
    if (isSkipUndoStack() || !undoStack()) {
        m_commandId = commandId;
        updateShortcutProjection();
        emit commandIdChanged(m_commandId);
    } else
        pushUndoPropertyData("commandId", commandId);
}

QString MenuAction::shortcutText() const
{
    return m_shortcutText;
}

void MenuAction::setShortcutText(const QString &shortcutText)
{
    if (checkPropSameValue("shortcutText", shortcutText))
        return;
    if (isSkipUndoStack() || !undoStack()) {
        m_shortcutText = shortcutText;
        updateShortcutProjection();
        emit shortcutTextChanged(m_shortcutText);
    } else
        pushUndoPropertyData("shortcutText", shortcutText);
}

QKeySequence MenuAction::effectiveShortcut() const
{
    // Явно заданное сочетание (в т.ч. для пользовательских команд,
    // которым нет кода в TAllKeys) всегда выигрывает
    if (!m_shortcutText.isEmpty())
        return QKeySequence::fromString(m_shortcutText, QKeySequence::PortableText);

    return ResKeyMap::keySequenceFromCode(m_commandId);
}

void MenuAction::updateShortcutProjection()
{
    setShortcut(effectiveShortcut());
}

quint32 MenuAction::cmdEx() const
{
    return m_cmdEx;
}

void MenuAction::setCmdEx(quint32 cmdEx)
{
    if (checkPropSameValue("cmdEx", cmdEx))
        return;
    if (isSkipUndoStack() || !undoStack()) {
        m_cmdEx = cmdEx;
        emit cmdExChanged(m_cmdEx);
    } else
        pushUndoPropertyData("cmdEx", cmdEx);
}

quint32 MenuAction::iconID() const
{
    return m_iconID;
}

void MenuAction::setIconID(quint32 iconID)
{
    if (checkPropSameValue("iconID", iconID))
        return;

    if (isSkipUndoStack() || !undoStack())
    {
        m_iconID = iconID;
        setIcon(ResMenu::inst()->getResIcon(iconID));
        emit iconIDChanged(m_iconID);
    } else
        pushUndoPropertyData("iconID", iconID);
}

MenuAction::ActionFlags MenuAction::actionFlags() const
{
    return ActionFlags(static_cast<ActionFlag>(m_flags));
}

void MenuAction::setActionFlags(ActionFlags flags)
{
    if (checkPropSameValue("actionFlags", QVariant::fromValue(flags)))
        return;
    if (isSkipUndoStack() || !undoStack()) {
        const bool oldDisabled = disabledInEmptyScroll();
        m_flags = static_cast<quint32>(flags);
        emit actionFlagsChanged(ActionFlags(static_cast<ActionFlag>(m_flags)));
        if (disabledInEmptyScroll() != oldDisabled)
            emit disabledInEmptyScrollChanged(disabledInEmptyScroll());
    } else
        pushUndoPropertyData("actionFlags", QVariant::fromValue(flags));
}

bool MenuAction::disabledInEmptyScroll() const
{
    return m_flags & FlagDisabledInEmptyScroll;
}

void MenuAction::setDisabledInEmptyScroll(bool disabled)
{
    if (checkPropSameValue("disabledInEmptyScroll", disabled))
        return;
    if (isSkipUndoStack() || !undoStack()) {
        // бит живёт в общем поле flags (m_flags) вместе с actionFlags
        if (disabled)
            m_flags |= FlagDisabledInEmptyScroll;
        else
            m_flags &= ~FlagDisabledInEmptyScroll;

        emit disabledInEmptyScrollChanged(disabled);
        emit actionFlagsChanged(actionFlags());
    } else
        pushUndoPropertyData("disabledInEmptyScroll", disabled);
}

QJsonObject MenuAction::serialize() const
{
    QJsonObject obj;
    obj["text"] = text();
    obj["separator"] = isSeparator();
    obj["checkable"] = isCheckable();
    obj["checked"] = isChecked();
    obj["enabled"] = isEnabled();
    obj["visible"] = isVisible();
    obj["toolTip"] = toolTip();
    obj["statusTip"] = statusTip();
    obj["whatsThis"] = whatsThis();
    obj["shortcutText"] = shortcutText();

    obj["helpId"] = helpId();
    obj["commandId"] = commandId();
    obj["cmdEx"] = static_cast<qint64>(cmdEx());
    obj["iconID"] = static_cast<qint64>(iconID());
    obj["flags"] = static_cast<qint64>(m_flags);

    return obj;
}

void MenuAction::deserialize(const QJsonObject &obj)
{
    const bool oldSkip = setSkipUndoStack(true);

    setText(obj["text"].toString());
    setSeparator(obj["separator"].toBool());
    setCheckable(obj["checkable"].toBool());
    setChecked(obj["checked"].toBool());
    setEnabled(obj["enabled"].toBool(true));
    setVisible(obj["visible"].toBool(true));
    setToolTip(obj["toolTip"].toString());
    setStatusTip(obj["statusTip"].toString());
    setWhatsThis(obj["whatsThis"].toString());

    // Новый формат — "shortcutText"; в старых файлах сочетание лежало
    // в "shortcut" — переносим его в shortcutText (проекция в
    // QAction::shortcut пересчитается в сеттерах)
    QString shortcutText = obj["shortcutText"].toString();
    if (shortcutText.isEmpty())
        shortcutText = obj["shortcut"].toString();

    setHelpId(obj["helpId"].toInt());
    setCommandId(obj["commandId"].toInt());
    setShortcutText(shortcutText);
    setCmdEx(obj["cmdEx"].toVariant().toUInt());
    setIconID(obj["iconID"].toVariant().toUInt());
    setActionFlags(ActionFlags(static_cast<ActionFlag>(obj["flags"].toVariant().toUInt())));

    setSkipUndoStack(oldSkip);
}

MenuAction *MenuAction::clone(const MenuAction *source, QObject *parent)
{
    if (!source)
        return nullptr;
    MenuAction *a = new MenuAction(parent);
    a->deserialize(source->serialize());
    return a;
}
