#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "reslistdockwidget.h"
#include "reslib.h"
#include "reslibdirmodel.h"
#include "rsrescore.h"
#include "ResourceEditorInterface.h"
#include "baseeditorwindow.h"
#include "propertymodel/propertydockwidget.h"
#include "propertymodel/propertymodel.h"
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QDebug>
#include <QDir>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_ResListDoc = new ResListDockWidget(this);
    m_PropertyDoc = new PropertyDockWidget(this);
    addDockWidget(Qt::LeftDockWidgetArea, m_ResListDoc);
    addDockWidget(Qt::RightDockWidgetArea, m_PropertyDoc);

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

    m_ResListDoc->setModel(m_Lib->model());

    connect(m_ResListDoc, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
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
            connect(editor, &BaseEditorWindow::propertyModelChanged, m_PropertyDoc, &PropertyDockWidget::setPropertyModel);
            wnd->showMaximized();
        }
    }
}

