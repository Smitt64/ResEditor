#ifndef MENUEDITORWINDOW_H
#define MENUEDITORWINDOW_H

#include <QObject>
#include <QDateTime>
#include <QPointer>
#include <baseeditorwindow.h>

class QMdiArea;
class QToolBar;
class ResBuffer;
class EditorMenuBar;
class EditorMenu;
class CodeEditor;
class QKeySequence;
class PropertyModel;
class QMdiSubWindow;
class SARibbonCategory;
class SARibbonLineEdit;
class SARibbonGallery;
class SARibbonGalleryGroup;
class SARibbonPannel;
class QActionGroup;
class MenuAction;
class MenuEditorWindow : public BaseEditorWindow
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_TOOLBOX_FILE, ":/json/MenuToolBox.json")
public:
    MenuEditorWindow(QWidget *parent = nullptr);
    virtual ~MenuEditorWindow();

    virtual void setupEditor() Q_DECL_OVERRIDE;
    virtual bool save(ResBuffer *res, QString *error) Q_DECL_OVERRIDE;

    void setMenu(ResBuffer *buffer);

    // Имя ресурса меню (MENU <name>). Для новых меню имя приходит из
    // диалога создания (ResMenuEditorPlugin::newItemsAction)
    void setMenuName(const QString &name);

    // Загрузка меню из XML-файла без диалогов (шаблоны при создании
    // нового меню). Текущее содержимое полностью заменяется
    bool loadFromXmlFile(const QString &filename, QString *error);

    virtual QAbstractItemModel *propertyModel() Q_DECL_OVERRIDE;

    virtual QString name() const Q_DECL_OVERRIDE;
    virtual QString title() const Q_DECL_OVERRIDE;
    virtual qint16 type() const Q_DECL_OVERRIDE;

    virtual void updateRibbonTabs() Q_DECL_OVERRIDE;
    virtual void clearRibbonTabs() Q_DECL_OVERRIDE;

    // Комментарий меню. applyComment — прямое применение (для
    // undo-команды MenuUndo::ChangeMenuCommentCommand)
    QString menuComment() const { return m_menuComment; }
    void applyComment(const QString &text);

private slots:
    void updateCodeEditor();
    void resetPropertyModel();
    void onMenuItemSelected(QWidget *container, QAction *action);
    void onMenuSelectionCleared();
    void saveToXml();
    void loadFromXml();
    void editComment();

protected:
    virtual void initRibbonPanels() Q_DECL_OVERRIDE;
    // Пользовательские элементы toolbox ищутся в <...>/toolbox/menu
    virtual QString toolBoxId() const Q_DECL_OVERRIDE { return QStringLiteral("menu"); }

private:
    void setupContainer();
    void setupNameLine();
    void MakeMenuRibbonCategory(SARibbonCategory *category);
    void updateToolBar();
    void updateToolBarSelection();
    // Синхронизирует ribbon-элементы выделенного пункта меню
    // (переключатели флагов, галерея иконок, команда удаления иконки)
    // с состоянием пункта
    void updateItemRibbonState();
    // Заполняет галерею иконоками из ресурсной DLL (ResMenu)
    void makeIconGallery(SARibbonGallery *gallery);
    // Выделяет в галерее текущую иконку пункта (или снимает выделение)
    void updateIconGallerySelection();
    // Панель быстрого назначения сочетания клавиш: эксклюзивные
    // checkable-команды для популярных сочетаний (F2/F4-F9, Enter, ...)
    void makeShortcutPanel(SARibbonPannel *panel);
    // Отмечает команду, соответствующую effectiveShortcut() пункта
    void updateShortcutChecks();
    void collectToolBarActions(const QList<QAction *> &actions);
    // Проставляет шаблонам toolbox иконку плашки перетаскивания
    // (DragIconRole) — реальную иконку из ресурсной DLL по iconID;
    // иконка в списке toolbox при этом не меняется
    void updateToolBoxIcons();
    QString menuToText() const;
    QString popupToText(EditorMenu *menu, int indent) const;
    QString escapeMenuString(const QString &s) const;
    QString encodeMnemonic(const QString &s) const;
    QString formatItemText(const QString &text, const QKeySequence &shortcut) const;

    // Экспорт в XML в формате старого xmlexport.cpp: формируем вручную
    // форматированными строками, чтобы файл был 1 в 1 как в старой утилите
    QString menuToXml() const;
    QString popupToXml(EditorMenu *menu, int delta) const;
    QString itemsToXml(const QList<QAction *> &actions, int delta) const;
    QString xmlEscape(const QString &s) const;

    QMdiArea *m_pMdi;
    QMainWindow *m_pContainer;
    QMdiSubWindow *m_pMdiWnd = nullptr;
    QToolBar *m_pToolBar;
    EditorMenuBar *m_pMenuBar;

    SARibbonCategory *m_pMenuCategory = nullptr;
    SARibbonLineEdit *m_pNameLineEdit = nullptr;
    QAction *m_pSaveToXml = nullptr;
    QAction *m_pActLoadFromXml = nullptr;
    QAction *m_pEditComment = nullptr;

    // Переключатели флагов выделенного пункта меню (ribbon-панель
    // "Пункт меню") и галерея иконок (панель "Иконка").
    // m_pRibbonTarget — пункт, к чьим NOTIFY-сигналам мы подписаны
    QAction *m_pActFlagToolbar = nullptr;
    QAction *m_pActFlagContext = nullptr;
    QAction *m_pActFlagDisInEmptyScroll = nullptr;
    QAction *m_pActClearIcon = nullptr;
    SARibbonGallery *m_pIconGallery = nullptr;
    SARibbonGalleryGroup *m_pIconGalleryGroup = nullptr;
    QActionGroup *m_pShortcutGroup = nullptr;
    QPointer<MenuAction> m_pRibbonTarget;

    QString m_menuName;
    QString m_menuComment;
    QDateTime m_menuDateTime;
    qint16 m_Type;

    CodeEditor *pCodeEditor;
    PropertyModel *m_pPropertyModel = nullptr;
    QObject *m_pCurrentPropertyTarget = nullptr;

    QMenu *m_ConnectToCode;
};

#endif // MENUEDITORWINDOW_H
