#ifndef STDPANELEDITOR_H
#define STDPANELEDITOR_H

#include "baseeditorwindow.h"

class ResPanel;
class PanelItem;
class BaseEditorView;
class QStatusBar;
class StatusBarElement;
class QHBoxLayout;
class QLineEdit;
class CustomRectItem;
class QGraphicsItem;
class ToolBoxModel;
class QClipboard;
class QTabWidget;
class StdPanelEditor : public BaseEditorWindow
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_TOOLBOX_FILE, ":/json/StdPanelEditorToolBox.json")
public:
    StdPanelEditor(const qint16 &Type, QWidget *parent = nullptr);

    virtual void setupEditor() Q_DECL_FINAL;
    void setPanel(ResPanel *panel, const QString &comment = QString());

    virtual bool save(ResBuffer *res, QString *error) Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual QString title() const Q_DECL_OVERRIDE;
    virtual qint16 type() const Q_DECL_OVERRIDE;

    virtual QAbstractItemModel *propertyModel() Q_DECL_OVERRIDE;

private slots:
    void sceneSelectionChanged();
    void sceneDeleteItems();
    void sceneCopyItems();
    void scenePasteItems();
    void sceneCutItems();
    void saveToXml();
    void onSave();
    void onCheckRes();
    void clipboardChanged();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void addCodeWindow(const QString &title, const QString &text);

private:
    enum FillItemsChildMode
    {
        FICMode_ChildBeforeParent,
        FICMode_ParentBeforeChild
    };

    QAction *addAction(const QIcon &icon, const QString &text, const QKeySequence &key = QKeySequence());
    QAction *addAction(QMenu *menu, const QIcon &icon, const QString &text, const QKeySequence &key = QKeySequence());
    void updateSizeStatus();
    void setupNameLine();
    void setupContrastAction();
    void setupScrolAreaAction();
    void setupPropertyAction();
    void setupCopyPaste();
    void setupMenus();
    void showCheckError(int stat);

    void fillResPanel(ResPanel *resPanel);
    void fillItems(const QList<QGraphicsItem*> &selectedItems, QSet<CustomRectItem*> &result,
                   const FillItemsChildMode &mode);

    const char *resTypeStr(int tp);

    BaseEditorView *m_pView;
    QTabWidget *m_TabContainer;

    ResPanel *m_pPanel;
    PanelItem* panelItem;
    qint16 m_Type;

    QStatusBar *m_StatusBar;
    StatusBarElement *m_SizeText, *m_CursorText;
    QWidget *m_pStatusContainer;
    QHBoxLayout *m_pStatusContainerLayout;
    QLineEdit *m_pNameLineEdit;

    QAction *m_pSave;
    QAction *m_pContrst, *m_pDelete, *m_pProperty, *m_pScrolAreaAction;
    QAction *m_pCutAction, *m_pCopyAction, *m_pPasteAction;
    QAction *m_pCheckAction, *m_EwViewAction, *m_Statistic;
    QAction *m_SaveToXml;

    QMenuBar *m_pMenuBar;
    QMenu *m_pEditMenu, *m_pViewMenu, *m_pResMenu;

    QToolBar *m_pToolBar;
    QClipboard *m_pClipboard;
};

#endif // STDPANELEDITOR_H
