#include "mainwindow.h"
#include "lbrobject.h"
#include "newitemsdlg.h"
#include "ui_mainwindow.h"
#include "reslistdockwidget.h"
#include "resbuffer.h"
#include "ResourceEditorInterface.h"
#include "baseeditorwindow.h"
#include "propertymodel/propertydockwidget.h"
#include "toolbox/toolboxdockwidget.h"
#include "rsrescore.h"
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QWidgetAction>
#include <QComboBox>
#include <QFileDialog>
#include <aboutdlg.h>>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_LastActiveWindow(nullptr),
    m_pLbrObj(nullptr)
{
    ui->setupUi(this);
    setWindowIcon(QIcon(":/img/lbrlogo.png"));

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

#ifdef _DEBUG
    CreateLbrObject(&m_pLbrObj, this);

    QDir dir = qApp->applicationDirPath();
    QString filename = dir.absoluteFilePath("BANK.lbr");
    m_pLbrObj->open(filename);

    m_ResListDock->setModel(m_pLbrObj->list());
#endif

    SetupMenus();
    CreateWindowsCombo();

    connect(m_ResListDock, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
    connect(m_ResListDock, &ResListDockWidget::deleteRequest, this, &MainWindow::OnDeleteRequest);
    connect(m_Mdi, &QMdiArea::subWindowActivated, this, &MainWindow::subWindowActivated);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::onAbout()
{
    AboutDlg dlg(":/json/AboutDlg.json", this);
    dlg.exec();
}

void MainWindow::SetupMenus()
{
    ui->actionNew->setIcon(QIcon(":/img/DocumentHS.png"));
    ui->actionNew->setShortcuts(QKeySequence::New);

    ui->actionOpen->setIcon(QIcon(":/img/openHS.png"));
    ui->actionOpen->setShortcuts(QKeySequence::Open);

    ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionOpen);
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
    SetupEditorTitle(editor, editor->type(), editor->name(), editor->title());
    wnd->setAttribute(Qt::WA_DeleteOnClose);
    connect(editor, &BaseEditorWindow::propertyModelChanged, m_PropertyDock, &PropertyDockWidget::setPropertyModel);
    connect(editor, &BaseEditorWindow::readySave, this, qOverload<bool>(&MainWindow::readySave));
    connect(editor, &BaseEditorWindow::titleChanged, this, &MainWindow::titleChanged);
    connect(editor, &BaseEditorWindow::modifyChanged, this, &MainWindow::modifyChanged);

    editor->setLbrObject(m_pLbrObj);
    wnd->installEventFilter(this);
    wnd->showMaximized();
    wnd->setWindowIcon(editor->windowIcon());
}

void MainWindow::readySave(BaseEditorWindow *editor)
{
    if (!editor)
        return;

    QString errorMsg;
    ResBuffer *resBuffer = nullptr;
    QString name = editor->name();
    qint16 type = editor->type();

    if (m_pLbrObj->isResExists(name, type))
    {
        QString msg = tr("Перезаписать существующий ресурс %1 [<b>%2</b>]?")
                .arg(RsResCore::inst()->typeNameFromResType(type), name);

        if (QMessageBox::question(this, tr("Сохранение"), msg, QMessageBox::Yes | QMessageBox::No) == QMessageBox::No)
            return;
        if (!m_pLbrObj->deleteResource(name, type))
            errorMsg = tr("Не удалось перезаписать ресурс");
    }

    if (m_pLbrObj->beginSaveRes(name, type, &resBuffer))
    {
        if (errorMsg.isEmpty() && editor->save(resBuffer, &errorMsg))
        {
            //resBuffer->debugSaveToFile(QString("1_%1").arg(name));
        }
        m_pLbrObj->endSaveRes(&resBuffer);
    }

    if (!errorMsg.isEmpty())
    {
        QString text = tr("Ошибка сохранения ресурса %1 [<b>%2</b>]")
                .arg(RsResCore::inst()->typeNameFromResType(type), name);

        if (errorMsg.isEmpty())
            QMessageBox::critical(this, tr("Сохранение"), text);
        else
        {
            QMessageBox dlg(this);
            dlg.setIcon(QMessageBox::Critical);
            dlg.setText(text);
            dlg.setInformativeText(errorMsg);
            dlg.exec();
        }
    }
}

void MainWindow::readySave(bool closeAfterSave)
{
    BaseEditorWindow *wnd = qobject_cast<BaseEditorWindow*>(sender());

    if (!wnd)
        return;

    readySave(wnd);
    if (closeAfterSave)
    {
        QList<QMdiSubWindow*> wndlist = m_Mdi->subWindowList();

        for (QMdiSubWindow *w : qAsConst(wndlist))
        {
            if (w->widget() == wnd)
            {
                w->close();
                break;
            }
        }
    }
}

void MainWindow::subWindowActivated(QMdiSubWindow *window)
{
    if (!window)
    {
        m_ToolBoxDock->setModel(nullptr);
        m_PropertyDock->setPropertyModel(nullptr);
        return;
    }

    if (m_LastActiveWindow == window)
        return;

    BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(window->widget());

    if (!wnd)
    {
        m_ToolBoxDock->setModel(nullptr);
        m_PropertyDock->setPropertyModel(nullptr);
    }
    else
    {
        m_ToolBoxDock->setModel(wnd->toolBox());
        m_PropertyDock->setPropertyModel(wnd->propertyModel());
    }

    m_LastActiveWindow = window;
}

void MainWindow::onNew()
{
    NewItemsDlg dlg(m_pLbrObj, this);
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

void MainWindow::onOpen()
{
    QString filename = QFileDialog::getOpenFileName(this,
                                                    tr("Выбор библиотеки"),
                                                    QString(),
                                                    tr("Библиотека ресурсов (*.lbr)"));

    if (!filename.isEmpty())
    {
        if (m_pLbrObj)
        {
            m_ResListDock->setModel(nullptr);
            delete m_pLbrObj;
            m_pLbrObj = nullptr;
        }

        CreateLbrObject(&m_pLbrObj, this);

        m_pLbrObj->open(filename);
        m_ResListDock->setModel(m_pLbrObj->list());
    }
}

void MainWindow::SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type,
                      const QString &name, const QString &title, bool changed)
{
    QString typeName = RsResCore::inst()->typeNameFromResType(Type);
    QString titlestr = QString("%1 [%2]: %3")
            .arg(typeName, name, title);

    if (changed)
        titlestr += " 🖊";

    wnd->setWindowTitle(titlestr);
    wnd->setWindowIcon(RsResCore::inst()->iconFromResType(Type));
}

void MainWindow::titleChanged(const QString &title)
{
    BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(sender());

    if (!wnd)
        return;

    SetupEditorTitle(wnd, wnd->type(), wnd->name(), title, wnd->isChanged());
}

void MainWindow::modifyChanged(bool changed)
{
    BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(sender());

    if (!wnd)
        return;

    SetupEditorTitle(wnd, wnd->type(), wnd->name(), wnd->title(), wnd->isChanged());
}

void MainWindow::CreateWindowsCombo()
{
    pWindowsComboBox = new QComboBox(this);
    pWindowsComboBox->setMinimumWidth(250);
    ui->actionNextWnd->setShortcut(QKeySequence(QKeySequence::NextChild));
    ui->actionPrevWnd->setShortcut(QKeySequence(QKeySequence::PreviousChild));

    ui->windowToolBar->addAction(ui->actionPrevWnd);
    ui->windowToolBar->addAction(ui->actionNextWnd);
    ui->windowToolBar->addWidget(pWindowsComboBox);
    ui->windowToolBar->addAction(ui->actionCloseWnd);
    ui->windowToolBar->addAction(ui->actionCloseAllWnd);
}

bool MainWindow::eventFilter(QObject *watched, QEvent *event)
{
    QMdiSubWindow *wnd = qobject_cast<QMdiSubWindow*>(watched);

    if (wnd && event->type() == QEvent::Close)
    {
        BaseEditorWindow *editor = dynamic_cast<BaseEditorWindow*>(wnd->widget());

        if (!editor)
            return QObject::eventFilter(watched, event);

        if (editor->isChanged())
        {
            QString typeName = RsResCore::inst()->typeNameFromResType(editor->type());
            QString msg = tr("Сохранить изменения в ресурсе %1 [<b>%2</b>]")
                    .arg(typeName, editor->name());

            QMessageBox::StandardButton btn = QMessageBox::question(this, tr("Сохранение"),
                                  msg, QMessageBox::Save | QMessageBox::Discard | QMessageBox::Abort);

            if (btn == QMessageBox::Abort)
                event->ignore();
            else if (btn == QMessageBox::Save)
            {
                readySave(editor);
                event->accept();
            }
            return true;
        }
    }

    return QObject::eventFilter(watched, event);
}

void MainWindow::OnDeleteRequest(const QString &name, const int &type)
{
    if (!m_pLbrObj)
        return;

    QString typeName = RsResCore::inst()->typeNameFromResType(type);
    QMessageBox::StandardButton btn =
        QMessageBox::question(this, tr("Удаление"), tr("Удалить ресурс %1 [<b>%2</b>]?")
                          .arg(typeName, name));

    if (btn == QMessageBox::Yes)
        m_pLbrObj->deleteResource(name, type);
}
