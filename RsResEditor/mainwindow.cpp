#include "mainwindow.h"
#include "lbrobject.h"
#include "ui_mainwindow.h"
#include "reslistdockwidget.h"
#include "rsrescore.h"
#include "resbuffer.h"
#include "ResourceEditorInterface.h"
#include "baseeditorwindow.h"
#include "propertymodel/propertydockwidget.h"
#include "toolbox/toolboxdockwidget.h"
#include "newitemsdlg.h"
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QWidgetAction>
#include <QComboBox>

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
    CreateWindowsCombo();

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

    wnd->installEventFilter(this);
    wnd->showMaximized();
}

void MainWindow::readySave(BaseEditorWindow *editor)
{
    if (!editor)
        return;

    quint64 oldSize = 0;
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

        m_pLbrObj->getResource(name, type, &resBuffer);

        resBuffer->open(QIODevice::ReadWrite);
        oldSize = resBuffer->size();

        int ver = resBuffer->version();
        if (ver < 2)
        {
            if (!m_pLbrObj->deleteResource(name, type))
                errorMsg = tr("Не удалось перезаписать ресурс");
        }
    }

    if (errorMsg.isEmpty() && editor->save(resBuffer, &errorMsg))
    {
        resBuffer->debugSaveToFile(QString("1_%1").arg(name));
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

void MainWindow::SetupEditorTitle(BaseEditorWindow *wnd, const qint16 &Type,
                      const QString &name, const QString &title, bool changed)
{
    QString typeName = RsResCore::inst()->typeNameFromResType(Type);
    QString titlestr = QString("%1 [%2]: %3")
            .arg(typeName, name, title);

    if (changed)
        titlestr += " 🖊";

    wnd->setWindowTitle(titlestr);
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
