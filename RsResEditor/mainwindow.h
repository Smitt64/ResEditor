#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QShortcut>
#include "updatechecker.h"
#include "SARibbon.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ResLib;
class LbrObjectInterface;
class QMdiArea;
class ResListDockWidget;
class PropertyDockWidget;
class ToolBoxDockWidget;
class QMdiSubWindow;
class BaseEditorWindow;
class QComboBox;
class QCloseEvent;
class SubWindowsModel;
class RecentLbrList;
class SARibbonPannel;
class ProxyAction;
class UndoActionWidget;
class MainWindow : public SARibbonMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;
    void setAutoUnloadDir(const QString &filename);

public slots:
    void open(const QString &filename);

private slots:
    void doubleResClicked(const QString &name, const int &type);
    void subWindowActivated(QMdiSubWindow *window);
    void onNew();
    void onOpen();
    void onAbout();
    void OnDeleteRequest(const QString &name, const int &type);
    void readySave(bool closeAfterSave);
    void readySave(BaseEditorWindow *editor);
    void titleChanged(const QString &title);
    void modifyChanged(bool changed);
    void checkUpdateFinished(bool hasUpdates, const CheckDataList &updatedata);
    void subWindowIndexChanged(const int &index);
    void showWindowList();
    void onOptions();

    void onOpenRes();
    void onOpenRecent();

    void UpdateFilterResTypes(bool state = false);

    void closeAllSubWindows();
    void UpdateActions();
    void OnResListSelectionChanged();

protected:
    virtual void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private:
    void InitQuickAccessBar();
    void InitButtonBar();
    void InitLbrPanel(SARibbonCategory *category);
    void InitLbrResourcePanel(SARibbonCategory *category);
    void InitNewGallary(SARibbonCategory *category);
    void InitViewBar(SARibbonCategory *category);
    void InitContextCategory();

    void setupAction(QAction *act, const QString& text, const QString& iconname);

    template<class T = QAction>
    T* createAction(const QString& text, const QString& iconname)
    {
        T *action = new T(this);
        setupAction(action, text, iconname);
        return action;
    }

    void SetActiveWindow(QMdiSubWindow *wnd);
    void AddEditorWindow(BaseEditorWindow *editor);
    void SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type,
                          const QString &name, const QString &title,
                          bool changed = false);
    void SetupMenus();
    QMdiSubWindow *IsExistsResWindow(const QString &name, const int &type);
    Ui::MainWindow *ui;

    ResListDockWidget *m_ResListDock;
    PropertyDockWidget *m_PropertyDock;
    ToolBoxDockWidget *m_ToolBoxDock;
    QMdiArea *m_Mdi;

    QComboBox *pWindowsComboBox;
    QMdiSubWindow *m_LastActiveWindow;

    LbrObjectInterface *m_pLbrObj;
    QString m_AutoUnloadDir;

    UpdateChecker *pUpdateChecker;
    SubWindowsModel *pWindowsModel;

    QAction *m_pActionNew, *m_pActionOpen, *m_pActionSave;
    QAction *m_pImportXmlFolder, *m_ImportXml, *m_pExportXmlFolder;
    QAction *m_pActionNewPanel, *m_pActionNewBScrol, *m_pActionDeleteRes, *m_pActionEditRes;

    QMenu *m_pUndoRedoMenu;
    //QToolButton *m_pRedoButton;
    UndoActionWidget *m_pUndoActionWidget;
    ProxyAction *m_pActionUndo, *m_pActionRedo;

    QShortcut *m_ResListKey, *m_ToolsListKey;

    SARibbonPannel *m_pFilterRibbonPanel;

    QScopedPointer<RecentLbrList> m_RecentLbrList;

    bool m_FlagMassCloseMode;
    QList<int> m_WindowsToSaveOnClose;
};
#endif // MAINWINDOW_H
