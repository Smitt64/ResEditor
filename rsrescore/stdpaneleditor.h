#ifndef STDPANELEDITOR_H
#define STDPANELEDITOR_H

#include "baseeditorwindow.h"
#include <QProcess>

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
class QTemporaryDir;
class PanelStructModel;
class ErrorsModel;
class ResSpellStringsDlg;
class PropertyWidgetMapper;
class SARibbonCategory;
class SARibbonGallery;
class StdPanelEditor : public BaseEditorWindow
{
    Q_OBJECT
    Q_CLASSINFO(CLASSINFO_TOOLBOX_FILE, ":/json/StdPanelEditorToolBox.json")
public:
    StdPanelEditor(const qint16 &Type, QWidget *parent = nullptr);
    virtual ~StdPanelEditor();

    virtual void setupEditor() Q_DECL_FINAL;
    void setPanel(ResPanel *panel, const QString &comment = QString());

    virtual bool save(ResBuffer *res, QString *error) Q_DECL_OVERRIDE;
    virtual QString name() const Q_DECL_OVERRIDE;
    virtual QString title() const Q_DECL_OVERRIDE;
    virtual qint16 type() const Q_DECL_OVERRIDE;

    virtual QAbstractItemModel *propertyModel() Q_DECL_OVERRIDE;
    virtual QAbstractItemModel *structModel() Q_DECL_OVERRIDE;

    virtual void updateRibbonTabs() Q_DECL_OVERRIDE;
    virtual void clearRibbonTabs() Q_DECL_OVERRIDE;

    //virtual QList<SARibbonContextCategory*> contextCategoryes() Q_DECL_OVERRIDE;

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
    void onViewEasyWin();
    void onViewCmd();
    void onInsertControl();
    void ViewerFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void CheckSpelling();

protected:
    virtual bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    void addCodeWindow(const QString &title, const QString &text);
    virtual void initRibbonPanels();

private:
    enum FillItemsChildMode
    {
        FICMode_ChildBeforeParent,
        FICMode_ParentBeforeChild
    };

    void CheckSpellingUpdateTexts(ResSpellStringsDlg *dlg);
    /*QAction *addAction(const QIcon &icon, const QString &text, const QKeySequence &key = QKeySequence());
    QAction *addAction(QMenu *menu, const QIcon &icon, const QString &text, const QKeySequence &key = QKeySequence());*/
    void updateSizeStatus();
    void setupNameLine();
    void setupScrolAreaAction();
    //void setupPropertyAction();
    void showCheckError(int stat, ErrorsModel *model);

    void fillResPanel(ResPanel *resPanel);
    void fillItems(const QList<QGraphicsItem*> &selectedItems, QSet<CustomRectItem*> &result,
                   const FillItemsChildMode &mode);

    const char *resTypeStr(int tp);
    void ViewResource(bool EwFlag);

    void MakeResRibbonCategory(SARibbonCategory* category);
    void MakeBorderRaibbonGallary(SARibbonGallery* gallery);
    void MakeStyleRaibbonGallary(SARibbonGallery* gallery);

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

    QAction *m_pContrst, *m_pDelete, *m_pProperty, *m_pScrolAreaAction;
    QAction *m_pCutAction, *m_pCopyAction, *m_pPasteAction;
    QAction *m_pCheckAction, *m_EwViewAction, *m_ViewAction, *m_Statistic;
    QAction *m_SaveToXml;
    QAction *m_pCreateControl, *m_pSpellCheckAction;

    //QToolBar *m_pToolBar;
    QClipboard *m_pClipboard;

    PanelStructModel *m_pStructModel;

    QScopedPointer<QTemporaryDir> m_ViewerDir;
    SARibbonCategory* m_pPanelCategory;

    QScopedPointer<PropertyWidgetMapper> m_RibbonMapper;
};

#endif // STDPANELEDITOR_H
