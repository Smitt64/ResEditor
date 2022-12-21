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
class StdPanelEditor : public BaseEditorWindow
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_TOOLBOX_FILE, ":/json/StdPanelEditorToolBox.json")
public:
    StdPanelEditor(QWidget *parent = nullptr);

    virtual void setupEditor() Q_DECL_FINAL;
    void setPanel(ResPanel *panel);

private slots:
    void sceneSelectionChanged();
    void sceneDeleteItems();
    void sceneCopyItems();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
     //virtual void mouseMoveEvent(QMouseEvent *event);

private:
    enum FillItemsChildMode
    {
        FICMode_ChildBeforeParent,
        FICMode_ParentBeforeChild
    };
    void updateSizeStatus();
    void setupNameLine();
    void setupContrastAction();
    void setupCopyPaste();
    void setupMenus();

    void fillItems(const QList<QGraphicsItem*> &selectedItems, QSet<CustomRectItem*> &result,
                   const FillItemsChildMode &mode);

    BaseEditorView *m_pView;

    ResPanel *m_pPanel;
    PanelItem* panelItem;

    QStatusBar *m_StatusBar;
    StatusBarElement *m_SizeText, *m_CursorText;
    QWidget *m_pStatusContainer;
    QHBoxLayout *m_pStatusContainerLayout;
    QLineEdit *m_pNameLineEdit;

    QAction *m_pContrst, *m_pDelete;
    QAction *m_pCutAction, *m_pCopyAction, *m_pPasteAction;

    QMenuBar *m_pMenuBar;
    QMenu *m_pEditMenu;

    QToolBar *m_pToolBar;
};

#endif // STDPANELEDITOR_H
