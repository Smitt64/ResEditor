#include "editormenubar.h"
#include "editormenu.h"
#include "menuaction.h"
#include "rscoreheader.h"
#include "undoredo/menuundocommands.h"
#include <resbuffer.h>

#include <QApplication>
#include <QDrag>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QDropEvent>
#include <QKeyEvent>
#include <QKeySequence>
#include <QLineEdit>
#include <QMimeData>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointer>
#include <QTextCodec>
#include <QTimer>
#include <QUndoStack>
#include <QXmlStreamReader>

namespace {

QAction *placeholderFor(QWidget *container)
{
    if (auto *em = qobject_cast<EditorMenu *>(container))
        return em->placeHolder();
    if (auto *emb = qobject_cast<EditorMenuBar *>(container))
        return emb->placeHolder();
    return nullptr;
}

void insertPlainAction(QWidget *container, QAction *before, QAction *action)
{
    if (auto *em = qobject_cast<EditorMenu *>(container))
        em->insertAction(before, action);
    else if (auto *emb = qobject_cast<EditorMenuBar *>(container))
        emb->insertAction(before, action);
}

void insertSubMenu(QWidget *container, QAction *before, EditorMenu *menu)
{
    menu->setParent(container);
    if (auto *em = qobject_cast<EditorMenu *>(container)) {
        em->insertMenu(before, menu);
        em->linkSubmenu(menu);
        menu->setUndoStack(em->undoStack());
    } else if (auto *emb = qobject_cast<EditorMenuBar *>(container)) {
        emb->insertMenu(before, menu);
        emb->linkMenu(menu);
        menu->setUndoStack(emb->undoStack());
    }
}

// Старый формат использует ~X~ для обозначения мнемоники (подчёркивания буквы).
// В Qt для этого служит префикс '&' перед символом: ~Д~ействия → &Действия.
QString decodeMnemonic(const QString &text)
{
    QString result;
    result.reserve(text.length());
    int i = 0;
    while (i < text.length()) {
        if (text.at(i) == QLatin1Char('~')
            && i + 2 < text.length()
            && text.at(i + 2) == QLatin1Char('~')) {
            result.append(QLatin1Char('&')).append(text.at(i + 1));
            i += 3;
        } else {
            result.append(text.at(i));
            ++i;
        }
    }
    return result;
}

// Выделяет сочетание клавиш из текста вида "Название\tCtrl+D" или
// "Название    Ctrl+D". При успехе удаляет его из текста и возвращает QKeySequence.
QKeySequence extractShortcut(QString &text)
{
    auto parseShortcutText = [](const QString &shortcutText) -> QKeySequence {
        if (shortcutText.isEmpty())
            return QKeySequence();
        // Старые ресурсы используют "Alt-F1", Qt-портативный формат — "Alt+F1".
        QString normalized = shortcutText;
        normalized.replace(QLatin1Char('-'), QLatin1Char('+'));
        return QKeySequence::fromString(normalized, QKeySequence::PortableText);
    };

    // 1. Табуляция — стандартный разделитель shortcut в Windows/QMenu.
    const int tabPos = text.indexOf(QLatin1Char('\t'));
    if (tabPos >= 0) {
        const QString shortcutText = text.mid(tabPos + 1).trimmed();
        QKeySequence seq = parseShortcutText(shortcutText);
        if (!seq.isEmpty()) {
            text = text.left(tabPos);
            return seq;
        }
    }

    // 2. Несколько пробелов — распространённый формат выравнивания в старых ресурсах.
    int lastSepStart = -1;
    int lastSepEnd = -1;
    for (int i = 0; i < text.length(); ) {
        if (text.at(i).isSpace()) {
            const int start = i;
            while (i < text.length() && text.at(i).isSpace())
                ++i;
            if (i - start >= 2) {
                lastSepStart = start;
                lastSepEnd = i;
            }
        } else {
            ++i;
        }
    }

    if (lastSepEnd > 0) {
        const QString shortcutText = text.mid(lastSepEnd).trimmed();
        QKeySequence seq = parseShortcutText(shortcutText);
        if (!seq.isEmpty()) {
            text = text.left(lastSepStart).trimmed();
            return seq;
        }
    }

    return QKeySequence();
}

// Обратно к MenuEditorWindow::formatItemText: в XML label сочетание
// дописывается в конец как "    ~Ctrl+S~", мнемоники — ~X~.
// Возвращает текст пункта, сочетание (portable text) — через shortcutText
QString decodeXmlItemText(const QString &label, QString *shortcutText)
{
    QString text = label;

    if (text.endsWith(QLatin1Char('~'))) {
        const int open = text.lastIndexOf(QLatin1Char('~'), text.length() - 2);
        // перед сочетанием — пробельное выравнивание; '~' после буквы —
        // это закрывающая тильда мнемоники ~X~, а не сочетание
        if (open > 0 && text.at(open - 1).isSpace()) {
            const QString sc = text.mid(open + 1, text.length() - open - 2);
            if (!QKeySequence::fromString(sc, QKeySequence::PortableText).isEmpty()) {
                if (shortcutText)
                    *shortcutText = sc;
                text = text.left(open).trimmed();
            }
        }
    }
    return decodeMnemonic(text);
}

// Обратно к decodeMnemonic: &X → ~X~, && → &
QString encodeMnemonic(const QString &text)
{
    QString result;
    result.reserve(text.length() * 2);

    for (int i = 0; i < text.length(); ++i) {
        if (text.at(i) == QLatin1Char('&') && i + 1 < text.length()) {
            if (text.at(i + 1) == QLatin1Char('&')) {
                result.append(QLatin1Char('&'));
                ++i;
            } else {
                result.append(QLatin1Char('~')).append(text.at(i + 1)).append(QLatin1Char('~'));
                ++i;
            }
        } else {
            result.append(text.at(i));
        }
    }
    return result;
}

QByteArray encode866(const QString &str)
{
    return QTextCodec::codecForName("IBM 866")->fromUnicode(str);
}

// Имя пункта для записи: обратно к extractShortcut сочетание дописывается
// в имя через табуляцию (у extractShortcut табуляция — первый приоритет,
// поэтому round-trip точный)
QString itemName(const MenuAction *ma)
{
    QString name = encodeMnemonic(ma->text());

    const QKeySequence sc = ma->shortcut();
    if (!sc.isEmpty())
        name += QLatin1Char('\t') + sc.toString(QKeySequence::PortableText);

    return name;
}

// Сохраняемые действия контейнера: всё, кроме фантомных плейсхолдеров
// и служебных QAction (не MenuAction и не подменю)
QList<QAction *> realActions(QWidget *container)
{
    QList<QAction *> result;
    const auto acts = container->actions();

    for (QAction *a : acts) {
        if (a == placeholderFor(container))
            continue;
        if (auto *em = qobject_cast<EditorMenu *>(container))
            if (a == em->sepPlaceholder())
                continue;

        if (!a->isSeparator()
            && !qobject_cast<MenuAction *>(a)
            && !qobject_cast<EditorMenu *>(a->menu()))
            continue;

        result.append(a);
    }
    return result;
}

template <typename T>
bool writeRaw(ResBuffer *buffer, const T &value)
{
    return buffer->write(reinterpret_cast<const char *>(&value), sizeof(T)) == sizeof(T);
}

} // anonymous namespace

EditorMenuBar::EditorMenuBar(QWidget *parent)
    : QMenuBar(parent)
{
    setAcceptDrops(true);
    m_placeholder = addAction(tr("Введите здесь"));

    // Отключаем stylesheet от MainWindow — меню редактора должно выглядеть нативно
    setAttribute(Qt::WA_StyleSheet, false);
}

void EditorMenuBar::paintEvent(QPaintEvent *e)
{
    QMenuBar::paintEvent(e);
    // поверх штатной отрисовки — фантомный стиль плейсхолдера
    QPainter p(this);
    paintPhantomItem(this, p, actionGeometry(m_placeholder),
                     m_placeholder->text(), activeAction() == m_placeholder);
}

void EditorMenuBar::linkMenu(EditorMenu *m)
{
    connect(m, &EditorMenu::itemSelected, this, &EditorMenuBar::itemSelected, Qt::UniqueConnection);
    connect(m, &EditorMenu::selectionCleared, this, &EditorMenuBar::selectionCleared, Qt::UniqueConnection);
    const auto acts = m->actions();
    for (QAction *a : acts)
        if (auto *sub = qobject_cast<EditorMenu *>(a->menu()))
            linkMenu(sub);
}

void EditorMenuBar::setUndoStack(QUndoStack *stack)
{
    m_undoStack = stack;
    const auto acts = actions();
    for (QAction *a : acts) {
        if (auto *sub = qobject_cast<EditorMenu *>(a->menu()))
            sub->setUndoStack(stack);
    }
}

bool EditorMenuBar::setSkipUndoStack(bool value)
{
    const bool old = m_skipUndoStack;
    m_skipUndoStack = value;
    return old;
}

bool EditorMenuBar::isSkipUndoStack() const
{
    return m_skipUndoStack;
}

void EditorMenuBar::selectAction(QAction *a)
{
    setActiveAction(a);
    if (a)
        emit itemSelected(this, a);
    else
        emit selectionCleared();
}

void EditorMenuBar::clearSelection()
{
    setActiveAction(nullptr);
    emit selectionCleared();
}

void EditorMenuBar::appendMenu(EditorMenu *m)
{
    if (!m)
        return;
    linkMenu(m);
    m->setParent(this);
    insertMenu(m_placeholder, m);
    if (m_undoStack)
        m->setUndoStack(m_undoStack);
}

EditorMenu *EditorMenuBar::addNewMenu()
{
    if (!m_undoStack) {
        EditorMenu *m = new EditorMenu(QString(), this);
        linkMenu(m);
        insertMenu(m_placeholder, m);
        m_openAfterEdit = true;
        startTitleEdit(m);
        emit itemSelected(this, m->menuAction());
        return m;
    }

    auto *cmd = new MenuUndo::AddMenuItemCommand(this, m_placeholder,
                                                 MenuUndo::ItemType::Submenu,
                                                 QString());
    m_undoStack->push(cmd);
    EditorMenu *m = cmd->createdSubmenu();
    if (m) {
        m_openAfterEdit = true;
        startTitleEdit(m);
    }
    return m;
}

void EditorMenuBar::openMenu(EditorMenu *m)
{
    if (!m)
        return;
    const auto acts = actions();
    for (QAction *x : acts)
        if (x->menu() && x->menu() != m)
            x->menu()->hide();
    setActiveAction(m->menuAction());
    m->popup(mapToGlobal(actionGeometry(m->menuAction()).bottomLeft()));
}

// --- inline-редактирование ---------------------------------------------------

void EditorMenuBar::startTitleEdit(EditorMenu *m)
{
    if (!m)
        return;
    finishTitleEdit(true);
    setActiveAction(nullptr);

    m_editingMenu = m;
    m_editPlaceholder = false;

    m_editor = new QLineEdit(this);
    m_editor->setPlaceholderText(tr("Новое меню"));
    m_editor->setText(m->title());
    m_editor->selectAll();
    m_editor->setGeometry(actionGeometry(m->menuAction()).adjusted(1, 1, -1, -1));
    m_editor->installEventFilter(this);
    connect(m_editor, &QLineEdit::editingFinished, this, [this] { finishTitleEdit(true); });
    m_editor->show();
    m_editor->setFocus();
}

void EditorMenuBar::startPlaceholderEdit()
{
    finishTitleEdit(true);
    setActiveAction(nullptr);

    m_editingMenu = nullptr;
    m_editPlaceholder = true;
    m_openAfterEdit = true; // созданное меню сразу раскрывается и висит

    m_editor = new QLineEdit(this);
    m_editor->setPlaceholderText(tr("Введите здесь"));
    m_editor->setGeometry(actionGeometry(m_placeholder).adjusted(1, 1, -1, -1));
    m_editor->installEventFilter(this);
    connect(m_editor, &QLineEdit::editingFinished, this, [this] { finishTitleEdit(true); });
    m_editor->show();
    m_editor->setFocus();
}

void EditorMenuBar::finishTitleEdit(bool apply)
{
    if (!m_editor)
        return;
    const QString text = m_editor->text().trimmed();
    EditorMenu *m = m_editingMenu;
    const bool wasPlaceholder = m_editPlaceholder;
    const bool openAfter = m_openAfterEdit;

    m_editor->removeEventFilter(this);
    m_editor->deleteLater();
    m_editor = nullptr;
    m_editingMenu = nullptr;
    m_editPlaceholder = false;
    m_openAfterEdit = false;

    if (wasPlaceholder) {
        // Enter на фантомном пункте менюбара — создать меню верхнего уровня
        if (apply && !text.isEmpty()) {
            if (m_undoStack) {
                auto *cmd = new MenuUndo::AddMenuItemCommand(this, m_placeholder,
                                                              MenuUndo::ItemType::Submenu,
                                                              text);
                m_undoStack->push(cmd);
                m = cmd->createdSubmenu();
            } else {
                m = new EditorMenu(text, this);
                linkMenu(m);
                insertMenu(m_placeholder, m);
                emit itemSelected(this, m->menuAction());
            }
            if (m && openAfter)
                openMenu(m);
        }
        return;
    }

    if (!m)
        return;
    QString result = apply ? text : m->title();
    if (result.isEmpty())
        result = tr("Новое меню");
    if (result != m->title()) {
        if (m_undoStack) {
            m_undoStack->push(new MenuUndo::RenameMenuItemCommand(m->menuAction(), result, true));
        } else {
            // applyTitle — прямое применение + NOTIFY для панели свойств
            m->applyTitle(result);
        }
    }
    if (apply && openAfter)
        openMenu(m);
}

bool EditorMenuBar::eventFilter(QObject *o, QEvent *e)
{
    if (o == m_editor && e->type() == QEvent::KeyPress) {
        auto *ke = static_cast<QKeyEvent *>(e);
        if (ke->key() == Qt::Key_Escape) {
            finishTitleEdit(false);
            return true;
        }
    }
    return QMenuBar::eventFilter(o, e);
}

// --- мышь -------------------------------------------------------------------

void EditorMenuBar::deleteTopLevel(QAction *a)
{
    if (!a || a == m_placeholder)
        return;

    if (!m_undoStack) {
        if (QMenu *m = a->menu()) {
            m->hide();
            removeAction(a);
            m->deleteLater();
        } else {
            removeAction(a);
            delete a;
        }
        setActiveAction(nullptr);
        emit selectionCleared();
        return;
    }

    m_undoStack->push(new MenuUndo::DeleteMenuItemCommand(this, a));
}

void EditorMenuBar::mousePressEvent(QMouseEvent *e)
{
    if (m_editor && !m_editor->geometry().contains(e->pos()))
        finishTitleEdit(true);
    if (e->button() == Qt::RightButton) {
        // контекстное меню, как в Designer
        QAction *act = actionAt(e->pos());
        if (act && act != m_placeholder) {
            QMenu cm;
            QAction *del = cm.addAction(tr("Удалить"));
            if (cm.exec(e->globalPos()) == del)
                deleteTopLevel(act);
        }
        return;
    }
    if (e->button() == Qt::LeftButton) {
        m_dragStart = e->pos();
        m_pressed = actionAt(e->pos());
    }
    QMenuBar::mousePressEvent(e);
    if (m_pressed == m_placeholder) {
        // одинарный клик по фантомному пункту — только выбор
        setActiveAction(m_placeholder);
        emit itemSelected(this, m_placeholder);
    } else if (m_pressed && m_pressed->menu()) {
        emit itemSelected(this, m_pressed);
    }
}

void EditorMenuBar::mouseReleaseEvent(QMouseEvent *e)
{
    QAction *pressed = m_pressed;
    m_pressed = nullptr;
    if (e->button() == Qt::LeftButton && pressed == m_placeholder)
        return; // не даём плейсхолдеру "сработать" как обычный пункт
    QMenuBar::mouseReleaseEvent(e);
}

void EditorMenuBar::mouseMoveEvent(QMouseEvent *e)
{
    if ((e->buttons() & Qt::LeftButton) && m_pressed && m_pressed != m_placeholder
        && (e->pos() - m_dragStart).manhattanLength() >= QApplication::startDragDistance()) {
        QAction *a = m_pressed;
        m_pressed = nullptr;
        beginDrag(a);
        return;
    }
    QMenuBar::mouseMoveEvent(e);
}

void EditorMenuBar::mouseDoubleClickEvent(QMouseEvent *e)
{
    QAction *act = actionAt(e->pos());
    if (act == m_placeholder) {
        startPlaceholderEdit(); // "пишите здесь" -> lineedit
        return;
    }
    if (act && act->menu()) {
        act->menu()->hide();
        if (auto *m = qobject_cast<EditorMenu *>(act->menu()))
            startTitleEdit(m);
        return;
    }
    QMenuBar::mouseDoubleClickEvent(e);
}

void EditorMenuBar::keyPressEvent(QKeyEvent *e)
{
    QAction *act = activeAction();
    if (e->key() == Qt::Key_Delete && act && act != m_placeholder) {
        deleteTopLevel(act);
        return;
    }
    if (e->key() == Qt::Key_F2 && act) {
        if (act == m_placeholder)
            startPlaceholderEdit();
        else if (auto *m = qobject_cast<EditorMenu *>(act->menu()))
            startTitleEdit(m);
        return;
    }
    QMenuBar::keyPressEvent(e);
}

// --- drag & drop ------------------------------------------------------------

void EditorMenuBar::beginDrag(QAction *a)
{
    finishTitleEdit(true);
    const bool isMenu = a->menu() != nullptr;
    if (isMenu)
        a->menu()->hide();
    setActiveAction(nullptr);

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

void EditorMenuBar::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->mimeData()->hasFormat(menuEditorMimeType()) ||
        e->mimeData()->hasFormat(menuTemplateMimeType()))
        e->acceptProposedAction();
}

void EditorMenuBar::dragMoveEvent(QDragMoveEvent *e)
{
    if (!e->mimeData()->hasFormat(menuEditorMimeType()) &&
        !e->mimeData()->hasFormat(menuTemplateMimeType()))
        return;
    e->acceptProposedAction();

    QAction *act = actionAt(e->pos());
    if (act && act != m_placeholder)
        setActiveAction(act);

    // наведение на заголовок меню во время перетаскивания — раскрыть его
    if (act && act != m_placeholder && act->menu()) {
        if (act != m_dragHover) {
            m_dragHover = act;
            QPointer<EditorMenuBar> self(this);
            QPointer<QAction> hovered(act);
            QTimer::singleShot(400, this, [self, hovered] {
                if (!self || !hovered || self->m_dragHover != hovered || !hovered->menu())
                    return;
                const auto acts = self->actions();
                for (QAction *x : acts)
                    if (x != hovered && x->menu())
                        x->menu()->hide();
                hovered->menu()->popup(self->mapToGlobal(
                    self->actionGeometry(hovered).bottomLeft()));
            });
        }
    } else {
        m_dragHover = nullptr;
    }
}

void EditorMenuBar::dragLeaveEvent(QDragLeaveEvent *e)
{
    m_dragHover = nullptr;
    QMenuBar::dragLeaveEvent(e);
}

void EditorMenuBar::dropEvent(QDropEvent *e)
{
    // Шаблон из панели элементов (ToolBox)
    if (e->mimeData()->hasFormat(menuTemplateMimeType())) {
        QAction *beforeRaw = actionAt(e->pos());
        if (!beforeRaw || beforeRaw == m_placeholder)
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
    if (!beforeRaw || beforeRaw == m_placeholder)
        beforeRaw = m_placeholder;
    QPointer<QAction> before(beforeRaw);
    QPointer<EditorMenuBar> self(this);

    // Само перемещение — отложенно, после завершения нативного цикла drag&drop.
    if (kind == 0) { // обычный пункт подняли на верхний уровень
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
    } else { // меню — перестановка верхнего уровня или подъём подменю наверх
        QPointer<EditorMenu> m(qobject_cast<EditorMenu *>(reinterpret_cast<QMenu *>(quintptr(ptr))));
        if (!m || m->menuAction() == beforeRaw) {
            e->ignore();
            return;
        }
        QTimer::singleShot(0, this, [self, m, srcW, before] {
            if (!self || !m)
                return;
            QWidget *src = srcW ? srcW.data() : nullptr;
            QAction *ma = m->menuAction();
            QAction *srcBefore = nullptr;
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

void EditorMenuBar::setMenu(ResBuffer *buffer)
{
    const QList<QAction *> old = actions();
    for (QAction *a : old) {
        if (a == m_placeholder)
            continue;
        removeAction(a);
        if (QMenu *sub = a->menu())
            sub->deleteLater();
        else
            delete a;
    }

    unsigned char len = 0;
    quint16 nitems = 0;
    quint16 nameLens = 0;

    buffer->read(reinterpret_cast<char *>(&len), sizeof(unsigned char));
    if (len > 0) {
        QByteArray comment(len, Qt::Uninitialized);
        buffer->read(comment.data(), len);
        // Комментарий уже установлен в ResBuffer через setComment() при загрузке.
    }

    buffer->read(reinterpret_cast<char *>(&nitems), sizeof(quint16));
    buffer->read(reinterpret_cast<char *>(&nameLens), sizeof(quint16));

    const bool oldSkip = setSkipUndoStack(true);
    readPopUp(this, buffer);
    setSkipUndoStack(oldSkip);
}

void EditorMenuBar::readPopUp(QWidget *container, ResBuffer *buffer)
{
    Q_ASSERT(container);
    quint16 nItem = 0;
    buffer->read(reinterpret_cast<char *>(&nItem), sizeof(quint16));

    QAction *before = placeholderFor(container);

    for (quint16 i = 0; i < nItem; ++i) {
        qint16 tp = 0;
        buffer->read(reinterpret_cast<char *>(&tp), sizeof(qint16));

        if (tp == RES_DELIM) {
            QAction *sep = new MenuAction(container);
            sep->setSeparator(true);
            insertPlainAction(container, before, sep);
            continue;
        }

        quint16 help = 0;
        buffer->read(reinterpret_cast<char *>(&help), sizeof(quint16));

        QString name = decodeMnemonic(ReadString(buffer));
        QKeySequence shortcut = extractShortcut(name);
        QString par  = ReadString(buffer);

        if (tp == RES_MITEM) {
            quint16 x = 0;
            buffer->read(reinterpret_cast<char *>(&x), sizeof(quint16));

            MenuAction *action = new MenuAction(name, container);
            action->setStatusTip(par);
            action->setToolTip(par);
            action->setHelpId(help);
            action->setCommandId(x);

            // Сочетание из текста пункта — в shortcutText; QAction::shortcut
            // является проекцией и пересчитывается в сеттерах MenuAction
            if (!shortcut.isEmpty())
                action->setShortcutText(shortcut.toString(QKeySequence::PortableText));

            if (buffer->headerVersion() >= 2) {
                quint32 cmdEx  = 0;
                quint32 iconID = 0;
                quint32 flags  = 0;
                buffer->read(reinterpret_cast<char *>(&cmdEx),  sizeof(quint32));
                buffer->read(reinterpret_cast<char *>(&iconID), sizeof(quint32));
                buffer->read(reinterpret_cast<char *>(&flags),  sizeof(quint32));
                action->setCmdEx(cmdEx);
                action->setIconID(iconID);
                action->setActionFlags(MenuAction::ActionFlags(static_cast<MenuAction::ActionFlag>(flags)));
            }

            insertPlainAction(container, before, action);
        }
        else if (tp == RES_MPOPUP) {
            EditorMenu *sub = new EditorMenu(name, container);
            sub->setHelpId(help);
            sub->menuAction()->setStatusTip(par);
            sub->menuAction()->setToolTip(par);

            insertSubMenu(container, before, sub);
            readPopUp(sub, buffer);
        }
    }
}

QString EditorMenuBar::ReadString(ResBuffer *buffer)
{
    QString result;
    qint16 len = 0;
    buffer->read(reinterpret_cast<char *>(&len), sizeof(qint16));

    if (len > 0) {
        QByteArray bytes(len + 1, Qt::Uninitialized);
        buffer->read(bytes.data(), len);
        bytes[len] = '\0';
        result = buffer->decodeString(bytes.constData());
    }
    return result;
}

// ---------------------------------------------------------------------------
// Загрузка из XML — обратная к MenuEditorWindow::menuToXml операция
// ---------------------------------------------------------------------------

bool EditorMenuBar::setMenuFromXml(QXmlStreamReader &xml, QString *error)
{
    Q_ASSERT(xml.isStartElement() && xml.name() == QLatin1String("menu"));

    const QList<QAction *> old = actions();
    for (QAction *a : old) {
        if (a == m_placeholder)
            continue;
        removeAction(a);
        if (QMenu *sub = a->menu())
            sub->deleteLater();
        else
            delete a;
    }

    const bool oldSkip = setSkipUndoStack(true);
    readPopUpXml(this, xml);
    setSkipUndoStack(oldSkip);

    if (xml.hasError()) {
        if (error)
            *error = xml.errorString();
        return false;
    }
    return true;
}

void EditorMenuBar::readPopUpXml(QWidget *container, QXmlStreamReader &xml)
{
    Q_ASSERT(container);
    QAction *before = placeholderFor(container);

    // readNextStartElement возвращает false на </menu>/</popup> —
    // этим же завершается и рекурсия по вложенным popup
    while (xml.readNextStartElement()) {
        const QStringRef elem = xml.name();
        const QXmlStreamAttributes attrs = xml.attributes();

        if (elem == QLatin1String("delim")) {
            QAction *sep = new MenuAction(container);
            sep->setSeparator(true);
            insertPlainAction(container, before, sep);
            xml.skipCurrentElement();
        }
        else if (elem == QLatin1String("item")) {
            // label — зеркально formatItemText: мнемоники ~X~ и
            // сочетание, дописанное в конец как "    ~Ctrl+S~"
            QString shortcutText;
            QString name = decodeXmlItemText(attrs.value(QLatin1String("label")).toString(),
                                             &shortcutText);

            MenuAction *action = new MenuAction(name, container);
            const QString par = attrs.value(QLatin1String("comment")).toString();
            action->setStatusTip(par);
            action->setToolTip(par);
            action->setHelpId(attrs.value(QLatin1String("help")).toInt());
            action->setCommandId(attrs.value(QLatin1String("cmd")).toInt());

            if (!shortcutText.isEmpty())
                action->setShortcutText(shortcutText);

            // Расширенные поля версии 2 экспортируются только когда заданы
            if (attrs.hasAttribute(QLatin1String("cmdEx")))
                action->setCmdEx(attrs.value(QLatin1String("cmdEx")).toUInt());
            if (attrs.hasAttribute(QLatin1String("iconID")))
                action->setIconID(attrs.value(QLatin1String("iconID")).toUInt());
            if (attrs.hasAttribute(QLatin1String("flags")))
                action->setActionFlags(MenuAction::ActionFlags(
                    static_cast<MenuAction::ActionFlag>(attrs.value(QLatin1String("flags")).toUInt())));

            insertPlainAction(container, before, action);
            xml.skipCurrentElement();
        }
        else if (elem == QLatin1String("popup")) {
            EditorMenu *sub = new EditorMenu(decodeMnemonic(attrs.value(QLatin1String("label")).toString()),
                                             container);
            sub->setHelpId(attrs.value(QLatin1String("help")).toInt());
            const QString par = attrs.value(QLatin1String("comment")).toString();
            sub->menuAction()->setStatusTip(par);
            sub->menuAction()->setToolTip(par);

            insertSubMenu(container, before, sub);
            readPopUpXml(sub, xml);
        }
        else
            xml.skipCurrentElement();
    }
}

// ---------------------------------------------------------------------------
// Запись меню в ресурс — обратная к setMenu/readPopUp операция
// ---------------------------------------------------------------------------

bool EditorMenuBar::writeMenu(ResBuffer *buffer, const QString &comment, QString *error)
{
    // Комментарий: uint8 длины + байты в IBM866 (зеркально setMenu)
    QByteArray encComment = encode866(comment);
    if (encComment.size() > 255)
        encComment.truncate(255); // поле длины — один байт

    const uchar len = uchar(encComment.size());
    if (!writeRaw(buffer, len))
        return false;
    if (len && buffer->write(encComment.constData(), len) != len)
        return false;

    // nitems  — общее число структур MENU (пункты + подменю + разделители);
    // nameLens — размер пула строк рантайма (каждая строка: len + '\0')
    quint16 nitems = 0;
    quint32 nameLens = 0;
    countPopUp(this, nitems, nameLens);

    const quint16 nameLens16 = quint16(qMin<quint32>(nameLens, 0xFFFF));
    if (!writeRaw(buffer, nitems) || !writeRaw(buffer, nameLens16))
        return false;

    return writePopUp(this, buffer, error);
}

void EditorMenuBar::countPopUp(QWidget *container, quint16 &nitems, quint32 &nameLens)
{
    const auto acts = realActions(container);

    for (QAction *a : acts) {
        ++nitems; // каждая запись — одна структура MENU в рантайме

        if (a->isSeparator())
            continue; // разделители строк из пула не потребляют

        QString name, par;
        EditorMenu *sub = qobject_cast<EditorMenu *>(a->menu());
        if (sub) {
            name = encodeMnemonic(sub->title());
            par = sub->menuAction() ? sub->menuAction()->statusTip() : QString();
        } else {
            auto *ma = static_cast<MenuAction *>(a);
            name = itemName(ma);
            par = ma->statusTip();
        }

        nameLens += encode866(name).size() + 1;
        nameLens += encode866(par).size() + 1;

        if (sub)
            countPopUp(sub, nitems, nameLens);
    }
}

bool EditorMenuBar::writePopUp(QWidget *container, ResBuffer *buffer, QString *error)
{
    const auto acts = realActions(container);

    const quint16 nItem = quint16(acts.size());
    if (!writeRaw(buffer, nItem))
        return false;

    for (QAction *a : acts) {
        if (a->isSeparator()) {
            if (!writeRaw(buffer, qint16(RES_DELIM)))
                return false;
            continue;
        }

        EditorMenu *sub = qobject_cast<EditorMenu *>(a->menu());
        MenuAction *ma = qobject_cast<MenuAction *>(a);

        QString name, par;
        quint16 help = 0;

        if (sub) {
            if (sub->helpId() < 0 || sub->helpId() > 0xFFFF) {
                if (error)
                    *error = tr("Идентификатор помощи %1 подменю \"%2\" не помещается в uint16")
                                 .arg(sub->helpId()).arg(sub->title());
                return false;
            }
            help = quint16(sub->helpId());
            name = encodeMnemonic(sub->title());
            par = sub->menuAction() ? sub->menuAction()->statusTip() : QString();
        } else {
            if (ma->helpId() < 0 || ma->helpId() > 0xFFFF) {
                if (error)
                    *error = tr("Идентификатор помощи %1 пункта \"%2\" не помещается в uint16")
                                 .arg(ma->helpId()).arg(ma->text());
                return false;
            }
            help = quint16(ma->helpId());
            name = itemName(ma);
            par = ma->statusTip();
        }

        if (!writeRaw(buffer, qint16(sub ? RES_MPOPUP : RES_MITEM))
            || !writeRaw(buffer, help)
            || !WriteString(buffer, name)
            || !WriteString(buffer, par))
            return false;

        if (sub) {
            if (!writePopUp(sub, buffer, error))
                return false;
        } else {
            if (ma->commandId() < 0 || ma->commandId() > 0xFFFF) {
                if (error)
                    *error = tr("Код команды %1 пункта \"%2\" не помещается в uint16")
                                 .arg(ma->commandId()).arg(ma->text());
                return false;
            }

            if (!writeRaw(buffer, quint16(ma->commandId())))
                return false;

            // Тело всегда пишется в версии 2 (setResVersion в MenuEditorWindow::save)
            const quint32 cmdEx  = ma->cmdEx();
            const quint32 iconID = ma->iconID();
            // actionFlags — это всё поле m_flags, включая бит DISINEMPTYSCROL
            const quint32 flags  = static_cast<quint32>(ma->actionFlags());

            if (!writeRaw(buffer, cmdEx) || !writeRaw(buffer, iconID) || !writeRaw(buffer, flags))
                return false;
        }
    }

    return true;
}

bool EditorMenuBar::WriteString(ResBuffer *buffer, const QString &str)
{
    // Зеркально ReadString: int16 длины + байты в IBM866 (без завершающего нуля)
    const QByteArray bytes = encode866(str);
    const qint16 len = qint16(bytes.size());

    if (!writeRaw(buffer, len))
        return false;
    if (len > 0 && buffer->write(bytes.constData(), len) != len)
        return false;

    return true;
}