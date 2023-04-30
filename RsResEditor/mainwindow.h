#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class ResLib;
class LbrObject;
class QMdiArea;
class ResListDockWidget;
class PropertyDockWidget;
class ToolBoxDockWidget;
class QMdiSubWindow;
class BaseEditorWindow;
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void doubleResClicked(const QString &name, const int &type);
    void subWindowActivated(QMdiSubWindow *window);
    void onNew();

private:
    void AddEditorWindow(BaseEditorWindow *editor);
    void SetupMenus();
    Ui::MainWindow *ui;

    ResListDockWidget *m_ResListDock;
    PropertyDockWidget *m_PropertyDock;
    ToolBoxDockWidget *m_ToolBoxDock;
    QMdiArea *m_Mdi;

    LbrObject *m_pLbrObj;
};
#endif // MAINWINDOW_H
