#ifndef EDITORMENUBAR_H
#define EDITORMENUBAR_H

#include <QMenuBar>
#include <QPointer>

class QLineEdit;
class EditorMenu;
class QUndoStack;
class ResBuffer;
class QXmlStreamReader;

// Строка меню в "режиме редактирования" (как в Qt Designer):
//  * последний элемент — фантомный плейсхолдер "Введите здесь";
//  * двойной клик по плейсхолдеру — lineedit, Enter создаёт меню
//    верхнего уровня, и его выпадающий список открывается и висит;
//  * двойной клик по заголовку меню — переименование, Delete — удаление,
//    заголовки перетаскиваются мышью.
class EditorMenuBar : public QMenuBar
{
    Q_OBJECT
public:
    explicit EditorMenuBar(QWidget *parent = nullptr);

    QAction *placeHolder() const { return m_placeholder; }

    QUndoStack *undoStack() const { return m_undoStack; }
    void setUndoStack(QUndoStack *stack);

    bool setSkipUndoStack(bool value);
    bool isSkipUndoStack() const;

    void selectAction(QAction *a);
    void clearSelection();

    void linkMenu(EditorMenu *m);

    EditorMenu *addNewMenu();
    void appendMenu(EditorMenu *m);   // вставить готовое меню перед плейсхолдером
    void startTitleEdit(EditorMenu *m);
    void openMenu(EditorMenu *m);

    void setMenu(ResBuffer *buffer);
    // Загрузка из XML (обратная к MenuEditorWindow::saveToXml/menuToXml).
    // xml должен стоять на стартовом элементе <menu> — читаются его дети
    // до </menu>. Текущее содержимое строки меню полностью заменяется
    bool setMenuFromXml(QXmlStreamReader &xml, QString *error);
    // Запись меню в буфер — обратная к setMenu операция. comment —
    // комментарий меню (uint8 длины + байты в начале тела ресурса)
    bool writeMenu(ResBuffer *buffer, const QString &comment, QString *error);

signals:
    void itemSelected(QWidget *container, QAction *action);
    void selectionCleared();

protected:
    void paintEvent(QPaintEvent *e) override;
    void mousePressEvent(QMouseEvent *e) override;
    void mouseMoveEvent(QMouseEvent *e) override;
    void mouseReleaseEvent(QMouseEvent *e) override;
    void mouseDoubleClickEvent(QMouseEvent *e) override;
    void keyPressEvent(QKeyEvent *e) override;
    bool eventFilter(QObject *o, QEvent *e) override;
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;

private:
    void readPopUp(QWidget *container, ResBuffer *buffer);
    void readPopUpXml(QWidget *container, QXmlStreamReader &xml);
    QString ReadString(ResBuffer *buffer);

    // Подсчёт структур MENU (nitems) и размера пула строк (nameLens)
    // для заголовка тела ресурса — рекурсивно по подменю
    void countPopUp(QWidget *container, quint16 &nitems, quint32 &nameLens);
    bool writePopUp(QWidget *container, ResBuffer *buffer, QString *error);
    static bool WriteString(ResBuffer *buffer, const QString &str);

    void startPlaceholderEdit();
    void finishTitleEdit(bool apply);
    void beginDrag(QAction *a);
    void deleteTopLevel(QAction *a);
    //void linkMenu(EditorMenu *m); // рекурсивно подключить сигналы меню и подменю

    QAction    *m_placeholder;
    QLineEdit  *m_editor        = nullptr;
    EditorMenu *m_editingMenu   = nullptr;
    bool        m_editPlaceholder = false; // редактируем плейсхолдер, а не заголовок
    bool        m_openAfterEdit = false;   // после ввода имени открыть меню висящим
    QPoint      m_dragStart;
    QAction    *m_pressed       = nullptr;
    QPointer<QAction> m_dragHover;
    QUndoStack *m_undoStack     = nullptr;
    bool        m_skipUndoStack = false;
};

#endif // EDITORMENUBAR_H
