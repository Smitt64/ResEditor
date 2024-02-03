#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    virtual bool eventFilter(QObject *watched, QEvent *event) Q_DECL_OVERRIDE;

private slots:
    void doubleResClicked(const QString &name, const int &type);
    void subWindowActivated(QMdiSubWindow *window);
    void onNew();
    void onOpen();
    void OnDeleteRequest(const QString &name, const int &type);
    void readySave(bool closeAfterSave);
    void readySave(BaseEditorWindow *editor);
    void titleChanged(const QString &title);
    void modifyChanged(bool changed);

private:
    void CreateWindowsCombo();
    void AddEditorWindow(BaseEditorWindow *editor);
    void SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type,
                          const QString &name, const QString &title,
                          bool changed = false);
    void SetupMenus();
    Ui::MainWindow *ui;

    ResListDockWidget *m_ResListDock;
    PropertyDockWidget *m_PropertyDock;
    ToolBoxDockWidget *m_ToolBoxDock;
    QMdiArea *m_Mdi;

    QComboBox *pWindowsComboBox;

    LbrObjectInterface *m_pLbrObj;
};
#endif // MAINWINDOW_H
