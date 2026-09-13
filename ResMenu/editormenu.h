#ifndef EDITORMENU_H
#define EDITORMENU_H

#include <QMenu>
#include <QPixmap>
#include <QPointer>
#include <QJsonObject>
#include <customrectitem.h>

class QLineEdit;
class QMimeData;
class QPainter;
class QToolButton;
class QUndoStack;

// ---- Общие помощники drag&drop для редактора меню ----
QString menuEditorMimeType();
QPixmap menuEditorDragPixmap(const QString &text);
void encodeMenuEditorMime(QMimeData *mime, qint32 kind, const QObject *ptr, const QWidget *src);
bool decodeMenuEditorMime(const QMimeData *mime, qint32 &kind, quint64 &ptr, quint64 &src);

// ---- Шаблоны из панели элементов (ToolBox) ----
// mimetype перетаскиваемых шаблонов и вставка их в контейнер меню
// (EditorMenu или EditorMenuBar) через undo-стек
QString menuTemplateMimeType();
bool insertMenuTemplate(QWidget *container, QAction *before,
                        const QJsonObject &tpl, QUndoStack *undoStack);
// Декодирует mime и отложенно (после цикла drag&drop) вставляет шаблон
// одной undo-макрокомандой; false — mime не шаблонный или битый
bool handleMenuTemplateDrop(QWidget *container, QAction *before,
                            const QMimeData *mime, QUndoStack *undoStack);

// Рисует "фантомный" пункт: полупрозрачный курсивный текст в пунктирной рамке
void paintPhantomItem(QWidget *w, QPainter &p, const QRect &r, const QString &text, bool active);

// Принудительно показывать подчёркивание у мнемоник (&X) независимо от настроек ОС/стиля
void forceUnderlineShortcuts(QWidget *w);

// Меню в "режиме редактирования" (как в Qt Designer):
//  * в конце всегда висит плейсхолдер "Введите здесь";
//  * двойной клик по плейсхолдеру — lineedit, Enter создаёт пункт;
//  * у выбранного пункта справа появляется кнопка-стрелка — клик по ней
//    превращает пункт в подменю и открывает его висящим;
//  * двойной клик по пункту — переименование, Delete — удаление,
//    пункты перетаскиваются мышью.
class EditorMenu : public QMenu
{
    Q_OBJECT

    Q_CLASSINFO(CLASSINFO_UNDOREDO, "EDITORMENU")
    Q_CLASSINFO(CLASSINFO_PROPERTYLIST, ":/json/Menu.json")
    Q_CLASSINFO(CLASSINFO_PROPERTYGROUP, "EditorMenu")

    Q_PROPERTY(int helpId READ helpId WRITE setHelpId NOTIFY helpIdChanged)

    // Переобъявление QMenu::title: у базового свойства нет NOTIFY, а
    // панель свойств и undo работают через наши сеттер/сигнал.
    // WRITE идёт в undo-aware setTitleText, QMenu::setTitle напрямую
    // использовать только через applyTitle (для undo-команд).
    Q_PROPERTY(QString title READ title WRITE setTitleText NOTIFY titleTextChanged)

public:
    explicit EditorMenu(const QString &title, QWidget *parent = nullptr);

    QAction *placeHolder() const { return m_placeholder; }
    QAction *sepPlaceholder() const { return m_sepPlaceholder; }

    QUndoStack *undoStack() const { return m_undoStack; }
    void setUndoStack(QUndoStack *stack);

    bool setSkipUndoStack(bool value);
    bool isSkipUndoStack() const;

    int helpId() const;
    void setHelpId(int helpId);

    // Наименование пункта (заголовок подменю). setTitleText — с undo,
    // applyTitle — прямое применение + NOTIFY (для undo-команд)
    void setTitleText(const QString &text);
    void applyTitle(const QString &text);

    void selectAction(QAction *a);
    void clearSelection();

    void linkSubmenu(EditorMenu *sub);

    QJsonObject serialize() const;
    void deserialize(const QJsonObject &obj);
    static EditorMenu *clone(const EditorMenu *source, QObject *parent);

public slots:
    void openSubmenu(EditorMenu *sub);

    // API для панели инструментов
    QAction    *addItemAtEnd();
    QAction    *insertItemAfter(QAction *ref);
    EditorMenu *addSubmenuAtEnd();
    EditorMenu *insertSubmenuAfter(QAction *ref);
    QAction    *addSeparatorAtEnd();
    QAction    *insertSeparatorAfter(QAction *ref);
    void        deleteAction(QAction *a);

    void startEditing(QAction *a); // a может быть и плейсхолдером

signals:
    void helpIdChanged(int helpId);
    void titleTextChanged(const QString &text);
    void itemSelected(QWidget *container, QAction *action);
    void selectionCleared();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    void hideEvent(QHideEvent *e) override;
    bool eventFilter(QObject *o, QEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private:
    void finishEditing(bool apply);
    void beginDrag(QAction *a);
    QAction *actionAfter(QAction *ref) const;
    QAction *lastRealAction() const;
    void updateSubmenuButton();
    void convertToSubmenu(QAction *a);
    void reopenChain(); // переоткрыть всю цепочку предков и само меню

    bool checkPropSameValue(const QString &propertyName, const QVariant &value);
    void pushUndoPropertyData(const QString &propertyName, const QVariant &_newValue);

    QAction    *m_placeholder;
    QAction    *m_sepPlaceholder = nullptr; // фантомный "Добавить разделитель"
    QLineEdit  *m_editor     = nullptr;
    QAction    *m_editing    = nullptr;
    QString     m_oldText;
    QToolButton *m_submenuBtn = nullptr;
    QPoint      m_dragStart;
    QAction    *m_pressed    = nullptr;
    QPointer<QAction> m_dragHover;
    QUndoStack *m_undoStack = nullptr;
    bool        m_skipUndoStack = false;
    int         m_helpId = 0;
};

#endif // EDITORMENU_H
