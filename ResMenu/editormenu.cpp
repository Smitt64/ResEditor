#include "editormenu.h"
#include "editormenubar.h"
#include "menuaction.h"
#include "undoredo/menuundocommands.h"

#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QJsonArray>
#include <QJsonDocument>
#include <QKeyEvent>
#include <QLineEdit>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointer>
#include <QTimer>
#include <QToolButton>
#include <QUndoStack>
#include <QProxyStyle>

// ---------------------------------------------------------------------------
// Общие помощники
// ---------------------------------------------------------------------------

class UnderlineShortcutStyle : public QProxyStyle
{
public:
    explicit UnderlineShortcutStyle(QStyle *style = nullptr) : QProxyStyle(style) {}

    int styleHint(StyleHint hint, const QStyleOption *option,
                  const QWidget *widget, QStyleHintReturn *returnData) const override
    {
        if (hint == QStyle::SH_UnderlineShortcut)
            return 1;
        return QProxyStyle::styleHint(hint, option, widget, returnData);
    }
};

void forceUnderlineShortcuts(QWidget *w)
{
    if (!w)
        return;
    QStyle *base = w->style();
    if (!base || dynamic_cast<UnderlineShortcutStyle *>(base))
        return;
    w->setStyle(new UnderlineShortcutStyle(base));
}

QString menuEditorMimeType()
{
    return QStringLiteral("application/x-menueditor-item");
}

QPixmap menuEditorDragPixmap(const QString &text)
{
    const QFontMetrics fm(QApplication::font());
    QRect r = fm.boundingRect(text).adjusted(0, 0, 14, 8);
    QPixmap pm(r.size());
    pm.fill(QColor(255, 255, 225));
    QPainter p(&pm);
    p.setPen(Qt::darkGray);
    p.drawRect(pm.rect().adjusted(0, 0, -1, -1));
    p.setPen(Qt::black);
    p.drawText(pm.rect().adjusted(7, 4, -7, -4), Qt::AlignLeft | Qt::AlignVCenter, text);
    return pm;
}

// kind: 0 — обычный QAction, 1 — QMenu (перетаскивается целое меню)
void encodeMenuEditorMime(QMimeData *mime, qint32 kind, const QObject *ptr, const QWidget *src)
{
    QByteArray ba;
    QDataStream ds(&ba, QIODevice::WriteOnly);
    ds << kind << quint64(quintptr(ptr)) << quint64(quintptr(src));
    mime->setData(menuEditorMimeType(), ba);
}

bool decodeMenuEditorMime(const QMimeData *mime, qint32 &kind, quint64 &ptr, quint64 &src)
{
    if (!mime || !mime->hasFormat(menuEditorMimeType()))
        return false;
    QByteArray ba = mime->data(menuEditorMimeType());
    QDataStream ds(&ba, QIODevice::ReadOnly);
    ds >> kind >> ptr >> src;
    return ds.status() == QDataStream::Ok;
}

// ---------------------------------------------------------------------------
// Шаблоны из панели элементов (ToolBox)
// ---------------------------------------------------------------------------

QString menuTemplateMimeType()
{
    return QStringLiteral("application/x-menueditor-template");
}

// Рекурсивная вставка шаблона. Формат tpl:
//   {"type": "item",      "action": {<MenuAction::serialize()>}}
//   {"type": "separator"}
//   {"type": "submenu",   "action": {"text", "statusTip", "helpId"},
//    "items": [<вложенные шаблоны>]}
bool insertMenuTemplate(QWidget *container, QAction *before,
                        const QJsonObject &tpl, QUndoStack *undoStack)
{
    if (!container || !undoStack)
        return false;

    const QString type = tpl[QStringLiteral("type")].toString();
    const QJsonObject props = tpl[QStringLiteral("action")].toObject();

    MenuUndo::ItemType itemType = MenuUndo::ItemType::PlainItem;
    if (type == QLatin1String("separator"))
        itemType = MenuUndo::ItemType::Separator;
    else if (type == QLatin1String("submenu"))
        itemType = MenuUndo::ItemType::Submenu;

    QString text = props[QStringLiteral("text")].toString();
    if (itemType == MenuUndo::ItemType::Separator)
        text.clear();

    auto *cmd = new MenuUndo::AddMenuItemCommand(container, before, itemType, text);
    undoStack->push(cmd);

    if (itemType == MenuUndo::ItemType::PlainItem) {
        // все свойства разом (deserialize сам глушит undo-стек)
        if (auto *ma = qobject_cast<MenuAction *>(cmd->createdAction()))
            ma->deserialize(props);
    } else if (itemType == MenuUndo::ItemType::Submenu) {
        EditorMenu *sub = cmd->createdSubmenu();
        if (!sub)
            return true;

        // заголовок уже назначен командой; helpId — из шаблона
        sub->deserialize(QJsonObject{{QStringLiteral("title"), text},
                                     {QStringLiteral("helpId"), props[QStringLiteral("helpId")].toInt()}});

        // поле "par" подменю — statusTip его menuAction
        if (QAction *ma = sub->menuAction())
            ma->setStatusTip(props[QStringLiteral("statusTip")].toString());

        const QJsonArray items = tpl[QStringLiteral("items")].toArray();
        for (const QJsonValue &v : items)
            insertMenuTemplate(sub, sub->placeHolder(), v.toObject(), undoStack);
    }

    return true;
}

bool handleMenuTemplateDrop(QWidget *container, QAction *before,
                            const QMimeData *mime, QUndoStack *undoStack)
{
    if (!container || !mime || !mime->hasFormat(menuTemplateMimeType()))
        return false;

    const QJsonDocument doc = QJsonDocument::fromJson(mime->data(menuTemplateMimeType()));
    if (!doc.isObject())
        return false;

    const QJsonObject tpl = doc.object();
    QPointer<QWidget> self(container);
    QPointer<QAction> beforeGuard(before);
    QPointer<QUndoStack> stackGuard(undoStack);

    // Отложенно — менять иерархию виджетов прямо в dropEvent
    // на Windows приводит к падению внутри QDrag::exec
    QTimer::singleShot(0, container, [self, beforeGuard, stackGuard, tpl] {
        if (!self || !stackGuard)
            return;

        stackGuard->beginMacro(QObject::tr("Вставка шаблона"));
        insertMenuTemplate(self, beforeGuard ? beforeGuard.data() : nullptr,
                           tpl, stackGuard);
        stackGuard->endMacro();
    });

    return true;
}

void paintPhantomItem(QWidget *w, QPainter &p, const QRect &r, const QString &text, bool active)
{
    if (!r.isValid())
        return;
    p.save();

    const QPalette &pal = w->palette();
    // стереть штатную отрисовку пункта (фон или подсветку наведения)
    p.fillRect(r, pal.color(active ? QPalette::Highlight : w->backgroundRole()));

    // пунктирная рамка
    QPen pen(pal.color(active ? QPalette::HighlightedText : QPalette::Mid));
    pen.setStyle(Qt::DashLine);
    p.setPen(pen);
    p.setRenderHint(QPainter::Antialiasing, true);
    p.drawRoundedRect(r.adjusted(3, 2, -4, -3), 3, 3);

    // полупрозрачный курсивный текст
    QFont f = w->font();
    f.setItalic(true);
    p.setFont(f);
    QColor tc = pal.color(active ? QPalette::HighlightedText : QPalette::Text);
    tc.setAlpha(150);
    p.setPen(tc);
    p.drawText(r, Qt::AlignCenter, text);

    p.restore();
}

// ---------------------------------------------------------------------------
// EditorMenu
// ---------------------------------------------------------------------------

EditorMenu::EditorMenu(const QString &title, QWidget *parent)
    : QMenu(title, parent)
{
    setAcceptDrops(true);
    if (parent) {
        setPalette(parent->palette());
        // Стиль виджета детям не наследуется, поэтому явно забираем стиль
        // родителя (у контейнера предпросмотра — windowsvista). Офисный
        // stylesheet MainWindow это НЕ отключает (QStyleSheetStyle оборачивает
        // любой стиль, а правила собираются по всей parent-цепочке) — но всё,
        // что не перекрыто QSS, отрисуется нативно. Сами офисные правила
        // перекрываются stylesheet'ом m_pContainer в MenuEditorWindow.
        if (parent->style() != QApplication::style())
            setStyle(parent->style());
    }
    // Подчёркивания мнемоник (&Обслуживание) рисуем всегда: это редактор
    // меню, позиция & должна быть видна. Отдельный экземпляр windowsvista
    // сам Alt не отслеживает (event-фильтр ставится только на стиль
    // приложения), поэтому без прокси подчёркивания не появятся никогда.
    forceUnderlineShortcuts(this);
    m_placeholder = addAction(tr("Введите здесь"));
    m_sepPlaceholder = addAction(tr("Добавить разделитель"));

    // Не наследуем палитру/шрифт от styled-предков: попап редактора
    // должен оставаться на палитре контейнера предпросмотра
    setAttribute(Qt::WA_StyleSheet, false);

    // кнопка-стрелка "сделать подменю" у выбранного пункта (как в Designer)
    m_submenuBtn = new QToolButton(this);
    m_submenuBtn->setArrowType(Qt::RightArrow);
    m_submenuBtn->setAutoRaise(false);
    m_submenuBtn->setCursor(Qt::ArrowCursor);
    m_submenuBtn->setToolTip(tr("Добавить подменю"));
    connect(m_submenuBtn, &QToolButton::clicked, this, [this] {
        QAction *a = activeAction();
        if (a && a != m_placeholder && !a->isSeparator() && !a->menu())
            convertToSubmenu(a);
    });
    m_submenuBtn->hide();
}

void EditorMenu::paintEvent(QPaintEvent *e)
{
    QMenu::paintEvent(e);
    // поверх штатной отрисовки — фантомный стиль обоих плейсхолдеров
    QPainter p(this);
    paintPhantomItem(this, p, actionGeometry(m_placeholder),
                     m_placeholder->text(), activeAction() == m_placeholder);
    paintPhantomItem(this, p, actionGeometry(m_sepPlaceholder),
                     m_sepPlaceholder->text(), activeAction() == m_sepPlaceholder);
}

QAction *EditorMenu::lastRealAction() const
{
    const QList<QAction *> acts = actions();
    const int idx = acts.indexOf(m_placeholder);
    return idx > 0 ? acts.at(idx - 1) : nullptr;
}

QAction *EditorMenu::actionAfter(QAction *ref) const
{
    if (!ref)
        return m_placeholder;
    const QList<QAction *> acts = actions();
    const int idx = acts.indexOf(ref);
    if (idx < 0 || idx + 1 >= acts.size())
        return m_placeholder;
    return acts.at(idx + 1);
}

void EditorMenu::linkSubmenu(EditorMenu *sub)
{
    // сигналы дочернего меню транслируем наверх, чтобы MainWindow
    // видел выбор на любой глубине вложенности
    connect(sub, &EditorMenu::itemSelected, this, &EditorMenu::itemSelected, Qt::UniqueConnection);
    connect(sub, &EditorMenu::selectionCleared, this, &EditorMenu::selectionCleared, Qt::UniqueConnection);
}

void EditorMenu::setUndoStack(QUndoStack *stack)
{
    m_undoStack = stack;
    const auto acts = actions();
    for (QAction *a : acts) {
        if (auto *sub = qobject_cast<EditorMenu *>(a->menu()))
            sub->setUndoStack(stack);
    }
}

bool EditorMenu::setSkipUndoStack(bool value)
{
    const bool old = m_skipUndoStack;
    m_skipUndoStack = value;
    return old;
}

bool EditorMenu::isSkipUndoStack() const
{
    if (m_skipUndoStack)
        return true;
    QObject *p = parent();
    if (auto *em = qobject_cast<EditorMenu *>(p))
        return em->isSkipUndoStack();
    if (auto *emb = qobject_cast<EditorMenuBar *>(p))
        return emb->isSkipUndoStack();
    return false;
}

bool EditorMenu::checkPropSameValue(const QString &propertyName, const QVariant &value)
{
    if (propertyName == QLatin1String("helpId"))
        return m_helpId == value.toInt();
    if (propertyName == QLatin1String("title"))
        return title() == value.toString();
    return property(propertyName.toLocal8Bit().data()) == value;
}

void EditorMenu::pushUndoPropertyData(const QString &propertyName, const QVariant &_newValue)
{
    if (!m_undoStack)
        return;

    QVariant oldValue;
    if (propertyName == QLatin1String("helpId"))
        oldValue = m_helpId;
    else if (propertyName == QLatin1String("title"))
        oldValue = title();
    else
        oldValue = property(propertyName.toLocal8Bit().data());

    m_undoStack->push(new MenuUndo::ChangeMenuPropertyCommand(
        this, propertyName, oldValue, _newValue));
}

int EditorMenu::helpId() const
{
    return m_helpId;
}

void EditorMenu::setHelpId(int helpId)
{
    if (checkPropSameValue("helpId", helpId))
        return;
    if (isSkipUndoStack() || !m_undoStack) {
        m_helpId = helpId;
        menuAction()->setProperty("helpId", helpId);
        emit helpIdChanged(m_helpId);
    } else {
        pushUndoPropertyData("helpId", helpId);
    }
}

void EditorMenu::applyTitle(const QString &text)
{
    if (title() == text)
        return;
    setTitle(text);
    emit titleTextChanged(text);
}

void EditorMenu::setTitleText(const QString &text)
{
    if (checkPropSameValue("title", text))
        return;
    if (isSkipUndoStack() || !m_undoStack)
        applyTitle(text);
    else
        pushUndoPropertyData("title", text);
}

void EditorMenu::selectAction(QAction *a)
{
    setActiveAction(a);
    if (a)
        emit itemSelected(this, a);
    else
        emit selectionCleared();
}

void EditorMenu::clearSelection()
{
    setActiveAction(nullptr);
    emit selectionCleared();
}

QJsonObject EditorMenu::serialize() const
{
    QJsonObject obj;
    obj["title"] = title();
    obj["helpId"] = helpId();
    return obj;
}

void EditorMenu::deserialize(const QJsonObject &obj)
{
    const bool oldSkip = setSkipUndoStack(true);
    applyTitle(obj["title"].toString());
    setHelpId(obj["helpId"].toInt());
    setSkipUndoStack(oldSkip);
}

EditorMenu *EditorMenu::clone(const EditorMenu *source, QObject *parent)
{
    if (!source)
        return nullptr;
    EditorMenu *m = new EditorMenu(QString(), qobject_cast<QWidget *>(parent));
    m->deserialize(source->serialize());
    return m;
}

QAction *EditorMenu::addItemAtEnd()
{
    return insertItemAfter(lastRealAction());
}

QAction *EditorMenu::insertItemAfter(QAction *ref)
{
    if (!m_undoStack) {
        QAction *a = new MenuAction(QString(), this);
        insertAction(actionAfter(ref), a);
        setActiveAction(a);
        emit itemSelected(this, a);
        startEditing(a);
        return a;
    }

    auto *cmd = new MenuUndo::AddMenuItemCommand(this, actionAfter(ref),
                                                 MenuUndo::ItemType::PlainItem,
                                                 QString());
    m_undoStack->push(cmd);
    QAction *a = cmd->createdAction();
    if (a)
        startEditing(a);
    return a;
}

EditorMenu *EditorMenu::addSubmenuAtEnd()
{
    return insertSubmenuAfter(lastRealAction());
}

EditorMenu *EditorMenu::insertSubmenuAfter(QAction *ref)
{
    if (!m_undoStack) {
        EditorMenu *sub = new EditorMenu(QString(), this);
        linkSubmenu(sub);
        insertMenu(actionAfter(ref), sub);
        setActiveAction(sub->menuAction());
        emit itemSelected(this, sub->menuAction());
        startEditing(sub->menuAction());
        return sub;
    }

    auto *cmd = new MenuUndo::AddMenuItemCommand(this, actionAfter(ref),
                                                 MenuUndo::ItemType::Submenu,
                                                 QString());
    m_undoStack->push(cmd);
    EditorMenu *sub = cmd->createdSubmenu();
    if (sub)
        startEditing(sub->menuAction());
    return sub;
}

QAction *EditorMenu::addSeparatorAtEnd()
{
    return insertSeparatorAfter(lastRealAction());
}

QAction *EditorMenu::insertSeparatorAfter(QAction *ref)
{
    if (!m_undoStack) {
        QAction *sep = insertSeparator(actionAfter(ref));
        setActiveAction(sep);
        emit itemSelected(this, sep);
        return sep;
    }

    auto *cmd = new MenuUndo::AddMenuItemCommand(this, actionAfter(ref),
                                                 MenuUndo::ItemType::Separator,
                                                 QString());
    m_undoStack->push(cmd);
    return cmd->createdAction();
}

void EditorMenu::deleteAction(QAction *a)
{
    if (!a || a == m_placeholder || a == m_sepPlaceholder)
        return;
    if (m_editing == a)
        finishEditing(false);
    m_submenuBtn->hide();

    if (!m_undoStack) {
        if (QMenu *sub = a->menu()) {
            sub->hide();
            removeAction(a);
            sub->deleteLater();
        } else {
            removeAction(a);
            delete a;
        }
        emit selectionCleared();
        return;
    }

    m_undoStack->push(new MenuUndo::DeleteMenuItemCommand(this, a));
}

// Превратить обычный пункт в подменю и открыть его висящим
void EditorMenu::convertToSubmenu(QAction *a)
{
    if (!a || !m_undoStack)
        return;
    m_undoStack->push(new MenuUndo::ConvertToSubmenuCommand(this, a));
}

// Кнопка-стрелка показывается только у выбранного "листового" пункта
void EditorMenu::updateSubmenuButton()
{
    QAction *a = activeAction();
    if (!a || a == m_placeholder || a == m_sepPlaceholder
        || a->isSeparator() || a->menu() || m_editing || !isVisible()) {
        m_submenuBtn->hide();
        return;
    }
    const QRect r = actionGeometry(a);
    m_submenuBtn->setGeometry(r.right() - r.height() + 2, r.top() + 1, r.height() - 2, r.height() - 2);
    m_submenuBtn->raise();
    m_submenuBtn->show();
}

// --- inline-редактирование ---------------------------------------------------

void EditorMenu::startEditing(QAction *a)
{
    if (!a || a == m_sepPlaceholder || a->isSeparator())
        return;
    finishEditing(true);
    ensurePolished();
    m_submenuBtn->hide();

    m_editing = a;
    m_oldText = (a == m_placeholder) ? QString() : a->text();

    m_editor = new QLineEdit(this);
    m_editor->setPlaceholderText(tr("Введите здесь"));
    m_editor->setText(m_oldText);
    m_editor->selectAll();
    m_editor->setGeometry(actionGeometry(a).adjusted(1, 1, -1, -1));
    m_editor->installEventFilter(this);
    connect(m_editor, &QLineEdit::editingFinished, this, [this] { finishEditing(true); });
    m_editor->show();
    m_editor->setFocus();
}

void EditorMenu::finishEditing(bool apply)
{
    if (!m_editor)
        return;
    const QString text = m_editor->text().trimmed();
    QAction *a = m_editing;
    const bool wasVisible = isVisible();

    QLineEdit *ed = m_editor;
    m_editor = nullptr;
    m_editing = nullptr;

    ed->removeEventFilter(this);
    if (wasVisible)
        setFocus(); // фокус обратно в меню — иначе удаление редактора
    // деактивирует popup-окно и QMenu сам себя закроет
    ed->deleteLater();

    // страховка: если меню всё-таки свернулось вместе с редактором —
    // открыть заново всю цепочку (из hideEvent сюда не попадаем:
    // там isVisible() уже false)
    if (wasVisible) {
        QPointer<EditorMenu> self(this);
        QTimer::singleShot(0, this, [self] {
            if (self)
                self->reopenChain();
        });
    }

    if (!a)
        return;

    if (a == m_placeholder) {
        // Enter на плейсхолдере — создаём новый пункт перед ним
        if (apply && !text.isEmpty()) {
            if (m_undoStack) {
                m_undoStack->push(new MenuUndo::AddMenuItemCommand(this, m_placeholder,
                                                                      MenuUndo::ItemType::PlainItem,
                                                                      text));
            } else {
                QAction *na = new QAction(text, this);
                insertAction(m_placeholder, na);
                setActiveAction(na);
                emit itemSelected(this, na);
            }
            QTimer::singleShot(0, this, [this] { updateSubmenuButton(); });
        }
        return;
    }

    QString result = apply ? text : m_oldText;
    if (result.isEmpty())
        result = a->menu() ? tr("Подменю") : tr("Новый пункт");

    if (result == a->text())
        return;

    if (m_undoStack) {
        const bool isMenuTitle = (a->menu() != nullptr)
                                 && qobject_cast<EditorMenu *>(a->menu());
        m_undoStack->push(new MenuUndo::RenameMenuItemCommand(a, result, isMenuTitle));
    } else {
        a->setText(result);
    }
}

bool EditorMenu::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_editor && e->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Escape) {
            finishEditing(false);
            return true;
        }
    }
    return QMenu::eventFilter(o, e);
}

// --- мышь -------------------------------------------------------------------

void EditorMenu::mousePressEvent(QMouseEvent *e)
{
    if (m_editor && !m_editor->geometry().contains(e->pos()))
        finishEditing(true);
    m_submenuBtn->hide();
    if (e->button() == Qt::RightButton) {
        // контекстное меню, как в Designer
        QAction *act = actionAt(e->pos());
        if (act && act != m_placeholder && act != m_sepPlaceholder) {
            QMenu cm;
            QAction *insSep = act->isSeparator() ? nullptr
                                                 : cm.addAction(tr("Вставить разделитель"));
            QAction *del = cm.addAction(tr("Удалить"));
            QAction *chosen = cm.exec(e->globalPos());
            if (chosen == insSep && insSep) {
                if (m_undoStack) {
                    m_undoStack->push(new MenuUndo::AddMenuItemCommand(this, act,
                                                                          MenuUndo::ItemType::Separator,
                                                                          QString()));
                } else {
                    QAction *sep = insertSeparator(act); // перед текущим пунктом
                    setActiveAction(sep);
                    emit itemSelected(this, sep);
                }
            } else if (chosen == del) {
                deleteAction(act);
            }
        }
        return;
    }
    if (e->button() == Qt::LeftButton) {
        m_dragStart = e->pos();
        m_pressed = actionAt(e->pos());
    }
    QMenu::mousePressEvent(e);
}

void EditorMenu::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) && m_pressed
        && m_pressed != m_placeholder && m_pressed != m_sepPlaceholder
        && (e->pos() - m_dragStart).manhattanLength() >= QApplication::startDragDistance()) {
        QAction *a = m_pressed;
        m_pressed = nullptr;
        beginDrag(a);
        return;
    }
    QMenu::mouseMoveEvent(e);
}

void EditorMenu::mouseReleaseEvent(QMouseEvent *e)
{
    if (e->button() != Qt::LeftButton) {
        QMenu::mouseReleaseEvent(e);
        return;
    }
    m_pressed = nullptr;
    QAction *act = actionAt(e->pos());
    if (!act) { // клик мимо пунктов — поведение по умолчанию (закрытие)
        QMenu::mouseReleaseEvent(e);
        return;
    }
    if (act == m_sepPlaceholder) { // фантомный пункт — вставить разделитель
        if (m_undoStack) {
            m_undoStack->push(new MenuUndo::AddMenuItemCommand(this, m_placeholder,
                                                               MenuUndo::ItemType::Separator,
                                                               QString()));
        } else {
            QAction *sep = insertSeparator(m_placeholder);
            setActiveAction(sep);
            emit itemSelected(this, sep);
        }
        return;
    }
    setActiveAction(act);
    emit itemSelected(this, act);
    if (act->menu()) {
        m_submenuBtn->hide();
        QMenu::mouseReleaseEvent(e); // даём подменю открыться штатно
        return;
    }
    // для обычных пунктов базовый обработчик НЕ вызываем:
    // иначе пункт "сработает" и всё меню закроется
    updateSubmenuButton();
}

void EditorMenu::mouseDoubleClickEvent(QMouseEvent *e)
{
    QAction *act = actionAt(e->pos());
    if (act && !act->isSeparator()) {
        // двойной клик по плейсхолдеру, пункту или заголовку подменю —
        // открыть lineedit (открытое подменю при этом прячем)
        if (act->menu())
            act->menu()->hide();
        startEditing(act);
        return;
    }
    QMenu::mouseDoubleClickEvent(e);
}

void EditorMenu::keyPressEvent(QKeyEvent *e)
{
    QAction *cur = activeAction();
    if (e->key() == Qt::Key_Delete && cur && cur != m_placeholder && cur != m_sepPlaceholder) {
        deleteAction(cur);
        return;
    }
    if (e->key() == Qt::Key_F2 && cur && !cur->isSeparator()) {
        if (cur->menu())
            cur->menu()->hide();
        startEditing(cur);
        return;
    }
    QMenu::keyPressEvent(e);
    updateSubmenuButton(); // навигация стрелками меняет выбор
}

void EditorMenu::hideEvent(QHideEvent *e)
{
    finishEditing(true);
    m_submenuBtn->hide();
    m_pressed = nullptr;
    QMenu::hideEvent(e);

    // Пустое подменю (только плейсхолдер) при закрытии сворачиваем обратно
    // в обычный пункт родителя — чтобы не оставался значок дочернего меню.
    if (auto *pm = qobject_cast<EditorMenu *>(parentWidget())) {
        if (actions().size() == 2) { // только два фантомных пункта — меню пустое
            QPointer<EditorMenu> self(this);
            QPointer<EditorMenu> parent(pm);
            QTimer::singleShot(0, this, [self, parent] {
                if (!self || !parent)
                    return;
                if (parent->undoStack())
                    parent->undoStack()->push(new MenuUndo::CollapseSubmenuCommand(parent, self));
                else
                    self->deleteLater();
            });
        }
    }
}

// --- drag & drop ------------------------------------------------------------

void EditorMenu::beginDrag(QAction *a)
{
    finishEditing(true);
    m_submenuBtn->hide();
    const bool isMenu = a->menu() != nullptr;

    QMimeData *mime = new QMimeData;
    encodeMenuEditorMime(mime, isMenu ? 1 : 0,
                         isMenu ? static_cast<QObject *>(a->menu()) : static_cast<QObject *>(a),
                         this);

    QDrag *drag = new QDrag(this);
    drag->setMimeData(mime);
    drag->setPixmap(menuEditorDragPixmap(isMenu ? a->menu()->title() : a->text()));
    drag->setHotSpot(QPoint(8, 8));
    emit itemSelected(this, a);
    drag->exec(Qt::MoveAction);
}

void EditorMenu::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat(menuEditorMimeType()) ||
        e->mimeData()->hasFormat(menuTemplateMimeType())) {
        m_submenuBtn->hide();
        e->acceptProposedAction();
    }
}

void EditorMenu::dragMoveEvent(QDragMoveEvent *e)
{
    if (!e->mimeData()->hasFormat(menuEditorMimeType()) &&
        !e->mimeData()->hasFormat(menuTemplateMimeType()))
        return;
    e->acceptProposedAction();

    QAction *act = actionAt(e->pos());
    if (act && act != m_placeholder && act != m_sepPlaceholder)
        setActiveAction(act);

    // как в дизайнере: подержали курсор над подменю — оно раскрылось
    if (act && act->menu()) {
        if (act != m_dragHover) {
            m_dragHover = act;
            QPointer<EditorMenu> self(this);
            QPointer<QAction> hovered(act);
            QTimer::singleShot(400, this, [self, hovered] {
                if (!self || !hovered || self->m_dragHover != hovered || !hovered->menu())
                    return;
                const QRect r = self->actionGeometry(hovered);
                hovered->menu()->popup(self->mapToGlobal(r.topRight()) + QPoint(-2, 0));
            });
        }
    } else {
        m_dragHover = nullptr;
    }
}

void EditorMenu::dragLeaveEvent(QDragLeaveEvent *e)
{
    m_dragHover = nullptr;
    QMenu::dragLeaveEvent(e);
}

void EditorMenu::dropEvent(QDropEvent *e)
{
    // Шаблон из панели элементов (ToolBox)
    if (e->mimeData()->hasFormat(menuTemplateMimeType())) {
        QAction *beforeRaw = actionAt(e->pos());
        if (!beforeRaw || beforeRaw == m_placeholder || beforeRaw == m_sepPlaceholder)
            beforeRaw = m_placeholder;

        if (handleMenuTemplateDrop(this, beforeRaw, e->mimeData(), undoStack()))
            e->acceptProposedAction();
        else
            e->ignore();
        return;
    }

    qint32 kind;
    quint64 ptr, src;
    if (!decodeMenuEditorMime(e->mimeData(), kind, ptr, src))
        return;

    QPointer<QWidget> srcW(reinterpret_cast<QWidget *>(quintptr(src)));
    QAction *beforeRaw = actionAt(e->pos());
    if (!beforeRaw || beforeRaw == m_placeholder || beforeRaw == m_sepPlaceholder)
        beforeRaw = m_placeholder;
    QPointer<QAction> before(beforeRaw);
    QPointer<EditorMenu> self(this);

    // Само перемещение выполняем отложенно — после того, как завершится
    // нативный цикл drag&drop (менять иерархию виджетов прямо в dropEvent
    // на Windows приводит к падению внутри QDrag::exec).
    if (kind == 0) { // обычный пункт
        QPointer<QAction> a(reinterpret_cast<QAction *>(quintptr(ptr)));
        if (!a || a == beforeRaw) {
            e->ignore();
            return;
        }
        QTimer::singleShot(0, this, [self, a, srcW, before] {
            if (!self || !a)
                return;
            QWidget *src = srcW ? srcW.data() : nullptr;
            QAction *srcBefore = nullptr;
            if (src) {
                const auto acts = src->actions();
                const int idx = acts.indexOf(a);
                if (idx >= 0 && idx + 1 < acts.size())
                    srcBefore = acts.at(idx + 1);
            }
            if (self->undoStack()) {
                self->undoStack()->push(new MenuUndo::MoveMenuItemCommand(
                    src, srcBefore, self,
                    before ? before.data() : self->m_placeholder, a));
            }
        });
    } else { // целое меню перетащили сюда как подменю
        QPointer<EditorMenu> m(qobject_cast<EditorMenu *>(reinterpret_cast<QMenu *>(quintptr(ptr))));
        if (!m || m == this || m->menuAction() == beforeRaw) {
            e->ignore();
            return;
        }
        // защита от цикла: меню нельзя вложить в самого себя или в своего потомка
        for (QWidget *p = this; p; p = p->parentWidget()) {
            if (p == m) {
                e->ignore();
                return;
            }
        }
        QTimer::singleShot(0, this, [self, m, srcW, before] {
            if (!self || !m)
                return;
            QWidget *src = srcW ? srcW.data() : nullptr;
            QAction *srcBefore = nullptr;
            QAction *ma = m->menuAction();
            if (src && ma) {
                const auto acts = src->actions();
                const int idx = acts.indexOf(ma);
                if (idx >= 0 && idx + 1 < acts.size())
                    srcBefore = acts.at(idx + 1);
            }
            if (self->undoStack()) {
                self->undoStack()->push(new MenuUndo::MoveMenuItemCommand(
                    src, srcBefore, self,
                    before ? before.data() : self->m_placeholder, ma));
            }
        });
    }
    e->acceptProposedAction();
}

void EditorMenu::openSubmenu(EditorMenu *sub)
{
    if (!sub || !isVisible())
        return;
    const QRect r = actionGeometry(sub->menuAction());
    sub->popup(mapToGlobal(r.topRight()) + QPoint(-2, 0));
}

// Переоткрыть всю цепочку меню сверху вниз: верхний уровень — из менюбара,
// каждое следующее — как подменю своего (уже открытого) родителя.
void EditorMenu::reopenChain()
{
    QList<EditorMenu *> chain;
    for (QWidget *p = parentWidget(); p; p = p->parentWidget())
        if (auto *em = qobject_cast<EditorMenu *>(p))
            chain.prepend(em);
    chain.append(this);

    for (EditorMenu *em : chain) {
        if (em->isVisible())
            continue;
        if (auto *mb = qobject_cast<EditorMenuBar *>(em->parentWidget()))
            mb->openMenu(em);
        else if (auto *pm = qobject_cast<EditorMenu *>(em->parentWidget()))
            pm->openSubmenu(em);
    }
}
