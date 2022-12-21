#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "reslistdockwidget.h"
#include "reslib.h"
#include "reslibdirmodel.h"
#include "rsrescore.h"
#include "ResourceEditorInterface.h"
#include "baseeditorwindow.h"
#include "propertymodel/propertydockwidget.h"
#include "toolbox/toolboxdockwidget.h"
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

    m_Lib = new ResLib(this);
   // qDebug() << m_Lib->loadResLibDll();


    QDir dir = qApp->applicationDirPath();
    QString filename = dir.absoluteFilePath("BANK.lbr");
    m_Lib->open(filename);

    m_ResListDock->setModel(m_Lib->model());

    connect(m_ResListDock, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
    connect(m_Mdi, &QMdiArea::subWindowActivated, this, &MainWindow::subWindowActivated);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::doubleResClicked(const QModelIndex &index)
{
    const tResLibDirElement &element = m_Lib->model()->element(index.row());
    ResourceEditorInterface *interface = RsResCore::inst()->pluginForType(element.Type);

    if (interface)
    {
        BaseEditorWindow *editor = interface->editor(element.Type, element.name, m_Lib);

        if (editor)
        {
            QMdiSubWindow *wnd = m_Mdi->addSubWindow(editor, Qt::SubWindow);
            wnd->setAttribute(Qt::WA_DeleteOnClose);
            connect(editor, &BaseEditorWindow::propertyModelChanged, m_PropertyDock, &PropertyDockWidget::setPropertyModel);
            wnd->showMaximized();
        }
    }
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

