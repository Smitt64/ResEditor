#include "mainwindow.h"
#include "lbrobject.h"
#include "ui_mainwindow.h"
#include "reslistdockwidget.h"
#include "rsrescore.h"
#include "ResourceEditorInterface.h"
#include "baseeditorwindow.h"
#include "propertymodel/propertydockwidget.h"
#include "toolbox/toolboxdockwidget.h"
#include "newitemsdlg.h"
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_ResListDock = new ResListDockWidget(this);
    m_PropertyDock = new PropertyDockWidget(this);
    m_ToolBoxDock = new ToolBoxDockWidget(this);

    addDockWidget(Qt::LeftDockWidgetArea, m_ResListDock);
    addDockWidget(Qt::LeftDockWidgetArea, m_ToolBoxDock);
    addDockWidget(Qt::RightDockWidgetArea, m_PropertyDock);

    tabifyDockWidget(m_ToolBoxDock, m_ResListDock);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabShape(QTabWidget::Triangular);

    m_Mdi = new QMdiArea();
    m_Mdi->setDocumentMode(true);
    m_Mdi->setTabsClosable(true);
    m_Mdi->setViewMode(QMdiArea::TabbedView);
    setCentralWidget(m_Mdi);

    m_pLbrObj = new LbrObject(this);

    QDir dir = qApp->applicationDirPath();
    QString filename = dir.absoluteFilePath("BANK.lbr");
    m_pLbrObj->open(filename);

    m_ResListDock->setModel(m_pLbrObj->list());

    SetupMenus();

    connect(m_ResListDock, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
    connect(m_Mdi, &QMdiArea::subWindowActivated, this, &MainWindow::subWindowActivated);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNew);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SetupMenus()
{
    ui->actionNew->setIcon(QIcon(":/img/DocumentHS.png"));
    ui->actionNew->setShortcuts(QKeySequence::New);

    ui->actionOpen->setIcon(QIcon(":/img/openHS.png"));
    ui->actionOpen->setShortcuts(QKeySequence::Open);
}

void MainWindow::doubleResClicked(const QString &name, const int &type)
{
    ResourceEditorInterface *interface = RsResCore::inst()->pluginForType(type);

    if (interface)
    {
        BaseEditorWindow *editor = interface->editor(type, name, m_pLbrObj);

        if (editor)
            AddEditorWindow(editor);
    }
}

void MainWindow::AddEditorWindow(BaseEditorWindow *editor)
{
    QMdiSubWindow *wnd = m_Mdi->addSubWindow(editor, Qt::SubWindow);
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    connect(editor, &BaseEditorWindow::propertyModelChanged, m_PropertyDock, &PropertyDockWidget::setPropertyModel);
    wnd->showMaximized();
}

void MainWindow::subWindowActivated(QMdiSubWindow *window)
{
    if (!window)
    {
        m_ToolBoxDock->setModel(nullptr);
        return;
    }

    BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(window->widget());

    if (!wnd)
        m_ToolBoxDock->setModel(nullptr);
    else
        m_ToolBoxDock->setModel(wnd->toolBox());
}

void MainWindow::onNew()
{
    NewItemsDlg dlg(this);
    dlg.buildStandartNewItems();
    if (dlg.exec() == QDialog::Accepted)
    {
        QString guid = dlg.action();
        QString name = dlg.name();
        QString path = dlg.path();
        ResourceEditorInterface *interface = RsResCore::inst()->pluginForNewAction(guid);

        if (interface)
        {
            BaseEditorWindow *editor = interface->newItemsAction(guid, name, path);

            if (editor)
                AddEditorWindow(editor);
        }
    }
}

