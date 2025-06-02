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
#include "updatecheckermessagebox.h"
#include "subwindowsmodel.h"
#include "windowslistdlg.h"
#include "selectresourcedlg.h"
#include "resapplication.h"
#include "options/resoptions.h"
#include "options/recentlbrlist.h"
#include <QMdiSubWindow>
#include <QMdiArea>
#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QWidgetAction>
#include <QComboBox>
#include <QFileDialog>
#include <aboutdlg.h>
#include <QThreadPool>
#include <QKeySequence>
#include <QSettings>
#include "savefilesdlg.h"

MainWindow::MainWindow(QWidget *parent)
    : SARibbonMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_LastActiveWindow(nullptr),
    m_pLbrObj(nullptr),
    m_FlagMassCloseMode(false)
{
    ui->setupUi(this);
    SARibbonBar* ribbon = ribbonBar();

    SARibbonCategory *mainPage = new SARibbonCategory("Главная");
    ribbon->addCategoryPage(mainPage);

    SARibbonCategory *viewPage = new SARibbonCategory("Вид");
    ribbon->addCategoryPage(viewPage);

    ResApplication *app = (ResApplication*)qApp;

    QAbstractButton* btn = ribbon->applicationButton();
    if (!btn)
    {
        btn = new SARibbonApplicationButton(this);
        ribbon->setApplicationButton(btn);
    }
    btn->setText(tr("Список ресурсов"));

    m_ResListDock = new ResListDockWidget(this);
    m_PropertyDock = new PropertyDockWidget(this);
    m_ToolBoxDock = new ToolBoxDockWidget(this);

    m_Mdi = new QMdiArea();
    m_Mdi->setDocumentMode(true);
    m_Mdi->setTabsClosable(true);
    m_Mdi->setViewMode(QMdiArea::TabbedView);
    setCentralWidget(m_Mdi);
    SetupMenus();

    setWindowIcon(QIcon(":/img/lbrlogo.png"));
    InitQuickAccessBar();
    InitButtonBar();
    InitLbrPanel(mainPage);
    InitLbrResourcePanel(mainPage);
    //InitNewGallary(mainPage);
    InitViewBar(viewPage);

    pUpdateChecker = new UpdateChecker();
    pUpdateChecker->setAutoDelete(false);
    pUpdateChecker->setProgramName("RsWorkMaintenanceTool.exe");
    pUpdateChecker->setSettings(app->settings());

    m_ResListDock->setObjectName("ResListDock");
    m_PropertyDock->setObjectName("PropertyDock");
    m_ToolBoxDock->setObjectName("ToolBoxDock");

    addDockWidget(Qt::LeftDockWidgetArea, m_ResListDock);
    addDockWidget(Qt::LeftDockWidgetArea, m_ToolBoxDock);
    addDockWidget(Qt::RightDockWidgetArea, m_PropertyDock);

    tabifyDockWidget(m_ToolBoxDock, m_ResListDock);
    setTabPosition(Qt::LeftDockWidgetArea, QTabWidget::West);
    setTabShape(QTabWidget::Triangular);

    m_ResListKey = new QShortcut(QKeySequence(tr("Alt+1")), this);
    m_ToolsListKey = new QShortcut(QKeySequence(tr("Alt+2")), this);

    QThreadPool::globalInstance()->start(pUpdateChecker);

    //ui->actionOpenRes->setShortcut(QKeySequence("Alt+O"));

    QSettings *s = app->settings();
    restoreGeometry(s->value("Geometry").toByteArray());
    restoreState(s->value("State").toByteArray());

    UpdateActions();

    connect(m_ResListKey, &QShortcut::activated, m_ResListDock, &QDockWidget::raise);
    connect(m_ToolsListKey, &QShortcut::activated, m_ToolBoxDock, &QDockWidget::raise);
    connect(m_ResListDock, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
    connect(m_ResListDock, &ResListDockWidget::deleteRequest, this, &MainWindow::OnDeleteRequest);

    connect(m_ResListDock, &ResListDockWidget::selectionChanged, this, &MainWindow::OnResListSelectionChanged);
    /*connect(pUpdateChecker, &UpdateChecker::checkFinished, this, &MainWindow::checkUpdateFinished);
    connect(m_ResListDock, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
    connect(m_ResListDock, &ResListDockWidget::deleteRequest, this, &MainWindow::OnDeleteRequest);
    connect(m_Mdi, &QMdiArea::subWindowActivated, this, &MainWindow::subWindowActivated);
    connect(ui->actionNew, &QAction::triggered, this, &MainWindow::onNew);
    connect(ui->actionOpen, &QAction::triggered, this, &MainWindow::onOpen);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAbout);
    connect(ui->actionOpenRes, &QAction::triggered, this, &MainWindow::onOpenRes);*/
}

MainWindow::~MainWindow()
{
    delete m_ResListKey;
    delete m_ToolsListKey;
    //delete ui;
}

void MainWindow::InitQuickAccessBar()
{
    ResApplication *app = (ResApplication*)qApp;
    m_RecentLbrList.reset(new RecentLbrList(app->settings()));

    SARibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar();

    QAction* actionNew = createAction(tr("Создать"), "NewFile");
    actionNew->setShortcut(QKeySequence::New);
    actionNew->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(actionNew);
    quickAccessBar->addSeparator();

    QAction* actionUndo = createAction(tr("Отменить"), "Undo");
    actionUndo->setShortcut(QKeySequence("Ctrl+z"));
    actionUndo->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(actionUndo);

    QAction* actionRedo = createAction(tr("Повторить"), "redo");
    actionRedo->setShortcut(QKeySequence("Ctrl+y"));
    actionRedo->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(actionRedo);

    QList<QAction*> actions = m_RecentLbrList->actions();

    if (!actions.empty())
    {
        QMenu* RecentLbrMenu = new QMenu(tr("Недавние файлы"), this);
        RecentLbrMenu->setIcon(QIcon::fromTheme("History"));
        quickAccessBar->addSeparator();

        for (QAction *action : std::as_const(actions))
        {
            RecentLbrMenu->addAction(action);
            action->setIcon(QIcon::fromTheme("Library"));
            action->setObjectName(action->text());
            action->setParent(this);
            connect(action, SIGNAL(triggered(bool)), this, SLOT(onOpenRecent()));
        }

        quickAccessBar->addMenu(RecentLbrMenu, Qt::ToolButtonIconOnly, QToolButton::InstantPopup);
    }

    connect(actionNew, &QAction::triggered, this, &MainWindow::onNew);
}

void MainWindow::InitButtonBar()
{
    SARibbonSystemButtonBar* wbar = windowButtonBar();
    if (!wbar)
        return;

    pWindowsComboBox = new QComboBox(this);
    pWindowsComboBox->setMinimumWidth(250);
    wbar->addWidget(pWindowsComboBox);
    wbar->addSeparator();

    QAction* optionsAction = createAction(tr("Параметры"), "Settings");
    wbar->addAction(optionsAction);

    wbar->addSeparator();
    QAction* about = wbar->addAction(tr("О программе"), QIcon::fromTheme("HelpApplication"), Qt::ToolButtonIconOnly);
    wbar->addSeparator();

    connect(about, &QAction::triggered, this, &MainWindow::onAbout);
    connect(optionsAction, &QAction::triggered, this, &MainWindow::onOptions);
    connect(pWindowsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(subWindowIndexChanged(int)));
}

void MainWindow::InitLbrPanel(SARibbonCategory *category)
{
    SARibbonPannel* libPannel = new SARibbonPannel(tr("Бибилтотека"));
    category->addPannel(libPannel);

    m_pActionNew = createAction(tr("Создать"), "NewLibrary");
    //m_pActionNew->setShortcut(QKeySequence("Ctrl+N"));
    libPannel->addLargeAction(m_pActionNew);

    m_pActionOpen = createAction(tr("Открыть"), "OpenLibrary");
    m_pActionOpen->setShortcut(QKeySequence::Open);
    libPannel->addLargeAction(m_pActionOpen);

    libPannel->addSeparator();
    m_ImportXml = createAction(tr("Импорт XML файла"), "ImportXml");
    libPannel->addSmallAction(m_ImportXml);

    m_pImportXmlFolder = createAction(tr("Импорт из каталога"), "ImportCatalogPart");
    libPannel->addSmallAction(m_pImportXmlFolder);

    m_pExportXmlFolder = createAction(tr("Экспорт в XML файл"), "ExportXml");
    libPannel->addSmallAction(m_pExportXmlFolder);

    connect(m_pActionOpen, &QAction::triggered, this, &MainWindow::onOpen);
}

void MainWindow::InitLbrResourcePanel(SARibbonCategory *category)
{
    SARibbonPannel* resPannel = new SARibbonPannel(tr("Ресурс"));
    category->addPannel(resPannel);

    m_pActionNewPanel = createAction(tr("Создать PANEL"), "NewDialog");
    resPannel->addLargeAction(m_pActionNewPanel);

    m_pActionNewBScrol = createAction(tr("Создать BSCROL"), "NewTable");
    resPannel->addLargeAction(m_pActionNewBScrol);

    m_pActionEditRes = createAction(tr("Редактировать"), "EditDocument");
    resPannel->addMediumAction(m_pActionEditRes);

    m_pActionDeleteRes = createAction(tr("Удалить"), "DeleteTag");
    resPannel->addMediumAction(m_pActionDeleteRes);

    connect(m_pActionEditRes, &QAction::triggered, [=]()
    {
        QString name = m_pActionEditRes->property("Name").toString();
        int type = m_pActionEditRes->property("Type").toInt();

        doubleResClicked(name, type);
    });

    connect(m_pActionDeleteRes, &QAction::triggered, [=]()
    {
        QString name = m_pActionDeleteRes->property("Name").toString();
        int type = m_pActionDeleteRes->property("Type").toInt();

        OnDeleteRequest(name, type);
    });
}

void MainWindow::InitNewGallary(SARibbonCategory *category)
{
    SARibbonPannel* pannel = new SARibbonPannel(tr("Создать"));
    category->addPannel(pannel);

    SARibbonGallery* gallery = pannel->addGallery();
    gallery->setGalleryButtonMaximumWidth(80);

    LbrObjectInterface *TmpInterface = nullptr;
    CreateLbrObject(&TmpInterface, this);

    NewItemsDlg dlg(TmpInterface);
    dlg.buildStandartNewItems();

    QList<QAction*> galleryActions;
    QStringList groups = dlg.getGroups();

    for (const QString &grp : std::as_const(groups))
    {
        const QList<GroupInfoMap> &info = dlg.groupInfo(grp);

        for (const GroupInfoMap &elem : info)
        {
            QAction *action = createAction(elem[RoleTitle].toString(), elem[RoleIconName].toString());
            galleryActions.append(action);
        }
    }

    gallery->addCategoryActions(tr("Новые элементы"), galleryActions);
}

void MainWindow::InitViewBar(SARibbonCategory *category)
{
    SARibbonPannel* viewPannel = new SARibbonPannel(tr("Панель"));
    category->addPannel(viewPannel);

    QAction *actionResList = m_ResListDock->toggleViewAction();
    QAction *actionProperty = m_PropertyDock->toggleViewAction();
    QAction *actionToolBox = m_ToolBoxDock->toggleViewAction();

    actionProperty->setIcon(QIcon::fromTheme("Property"));
    actionResList->setIcon(QIcon::fromTheme("ResourceView"));
    actionToolBox->setIcon(QIcon::fromTheme("ToolBox"));

    viewPannel->addLargeAction(actionResList);
    viewPannel->addLargeAction(actionProperty);
    viewPannel->addLargeAction(actionToolBox);

    m_pFilterRibbonPanel = new SARibbonPannel(tr("Фильтр"));
    category->addPannel(m_pFilterRibbonPanel);

    const QList<qint16> types = RsResCore::types();
    const QList<qint16> stdtypes = RsResCore::stdTypes();
    for (const qint16 &type : types)
    {
        QString name = RsResCore::typeNameFromResType(type);
        QString iconName = RsResCore::iconNameFromResType(type);

        QAction *typeAction = createAction(name, iconName);
        typeAction->setCheckable(true);
        typeAction->setChecked(true);
        typeAction->setProperty("type", type);

        if (!stdtypes.contains(type))
            typeAction->setVisible(false);

        m_pFilterRibbonPanel->addSmallAction(typeAction);

        connect(typeAction, &QAction::toggled, this, &MainWindow::UpdateFilterResTypes);
    }
    m_pFilterRibbonPanel->resetToolButtonSize();

    pWindowsModel = new SubWindowsModel(this);
    pWindowsComboBox->setModel(pWindowsModel);

    SARibbonPannel* windowsPannel = new SARibbonPannel(tr("Окно"));
    category->addPannel(windowsPannel);

    QAction* optAct = new QAction(this);
    windowsPannel->setOptionAction(optAct);
    connect(optAct, &QAction::triggered, this, [this](bool on)
    {
        Q_UNUSED(on);
        showWindowList();
    });

    QAction *nextWindow = createAction(tr("Следующее"), "NextDocument");
    QAction *prevWindow = createAction(tr("Предыдущее"), "PreviousDocument");
    windowsPannel->addMediumAction(nextWindow);
    windowsPannel->addMediumAction(prevWindow);
    windowsPannel->addSeparator();

    QAction *closeWindow = createAction(tr("Закрыть"), "CloseDocument");
    QAction *closeAllWindow = createAction(tr("Закрыть все"), "CloseDocumentGroup");
    windowsPannel->addMediumAction(closeWindow);
    windowsPannel->addMediumAction(closeAllWindow);

    connect(nextWindow, SIGNAL(triggered(bool)), m_Mdi, SLOT(activateNextSubWindow()));
    connect(prevWindow, SIGNAL(triggered(bool)), m_Mdi, SLOT(activatePreviousSubWindow()));
    connect(closeWindow, SIGNAL(triggered(bool)), m_Mdi, SLOT(closeActiveSubWindow()));
    connect(closeAllWindow, SIGNAL(triggered(bool)), this, SLOT(closeAllSubWindows()));
}

QAction* MainWindow::createAction(const QString& text, const QString& iconname)
{
    QAction* act = new QAction(this);
    act->setText(text);
    act->setIcon(QIcon::fromTheme(iconname));
    act->setObjectName(text);
    return act;
}

void MainWindow::UpdateActions()
{
    bool EnableIfOpenLbr = (m_pLbrObj != nullptr);
    bool EnableIfOpenLbrSelRes = (m_pLbrObj != nullptr && m_ResListDock->hasSelection());

    m_pImportXmlFolder->setEnabled(EnableIfOpenLbr);
    m_ImportXml->setEnabled(EnableIfOpenLbr);
    m_pExportXmlFolder->setEnabled(EnableIfOpenLbr);

    m_pActionNewPanel->setEnabled(EnableIfOpenLbr);
    m_pActionNewBScrol->setEnabled(EnableIfOpenLbr);
    m_pActionDeleteRes->setEnabled(EnableIfOpenLbrSelRes);
    m_pActionEditRes->setEnabled(EnableIfOpenLbrSelRes);
}

void MainWindow::onAbout()
{
    AboutDlg dlg(":/json/AboutDlg.json", this);
    dlg.exec();
}

void MainWindow::SetupMenus()
{
    ResApplication *app = (ResApplication*)qApp;
    m_RecentLbrList.reset(new RecentLbrList(app->settings()));

    /*ui->actionNew->setIcon(QIcon(":/img/DocumentHS.png"));
    ui->actionNew->setShortcuts(QKeySequence::New);

    ui->actionOpen->setIcon(QIcon(":/img/openHS.png"));
    ui->actionOpen->setShortcuts(QKeySequence::Open);*/

    /*ui->toolBar->addAction(ui->actionNew);
    ui->toolBar->addAction(ui->actionOpen);
    ui->toolBar->addSeparator();
    ui->toolBar->addAction(ui->actionOpenRes);*/

    /*ui->viewMenu->addAction(m_ResListDock->toggleViewAction());
    ui->viewMenu->addAction(m_PropertyDock->toggleViewAction());
    ui->viewMenu->addAction(m_ToolBoxDock->toggleViewAction());
    ui->viewMenu->addSeparator();
    ui->viewMenu->addAction(ui->toolBar->toggleViewAction());
    ui->viewMenu->addAction(ui->windowToolBar->toggleViewAction());*/

    /*QList<QAction*> actions = m_RecentLbrList->actions();
    for (QAction *action : actions)
    {
        ui->menuFile->addAction(action);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(onOpenRecent()));
    }

    ui->menuFile->addSeparator();
    QAction *exit = ui->menuFile->addAction(tr("Выход"));
    connect(exit, &QAction::triggered, [=]()
    {
        qApp->quit();
    });*/
}

void MainWindow::onOptions()
{
    ResApplication *app = (ResApplication*)qApp;
    ResOptions dlg(app->settings(), this);
    dlg.setAutoUnloadDir(m_AutoUnloadDir);
    dlg.exec();
}

void MainWindow::doubleResClicked(const QString &name, const int &type)
{
    QMdiSubWindow *wnd = IsExistsResWindow(name, type);
    if (wnd)
    {
        m_Mdi->setActiveSubWindow(wnd);
        return;
    }

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

    QModelIndex index = pWindowsModel->addWindow(wnd);
    pWindowsComboBox->setCurrentIndex(index.row());
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

    if (errorMsg.isEmpty())
    {
        QSettings *settings = ((ResApplication*)qApp)->settings();

        if (!m_AutoUnloadDir.isEmpty())
            RsResCore::inst()->saveResToXml(type, name, m_pLbrObj, m_AutoUnloadDir);

        if (settings->value("AutoUnload", false).toBool())
        {
            QStringList lst = settings->value("AutoUnloadDirs").toStringList();

            for (const QString &dir : lst)
                RsResCore::inst()->saveResToXml(type, name, m_pLbrObj, dir);
        }
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
        m_PropertyDock->setStructModel(nullptr);
        m_LastActiveWindow = nullptr;
        return;
    }

    if (m_LastActiveWindow == window)
        return;

    BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(window->widget());

    if (!wnd)
    {
        m_ToolBoxDock->setModel(nullptr);
        m_PropertyDock->setPropertyModel(nullptr);
        m_PropertyDock->setStructModel(nullptr);
    }
    else
    {
        m_ToolBoxDock->setModel(wnd->toolBox());
        m_PropertyDock->setPropertyModel(wnd->propertyModel());
        m_PropertyDock->setStructModel(wnd->structModel());

        QModelIndex index = pWindowsModel->findWindow(window);
        pWindowsComboBox->setCurrentIndex(index.row());
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

void MainWindow::open(const QString &filename)
{
    if (!filename.isEmpty())
    {
        if (m_pLbrObj)
        {
            m_ResListDock->setModel(nullptr);
            delete m_pLbrObj;
            m_pLbrObj = nullptr;
        }

        CreateLbrObject(&m_pLbrObj, this);

        if (m_pLbrObj->open(filename))
        {
            m_ResListDock->setModel(m_pLbrObj->list());
            m_RecentLbrList->addFile(filename);

            UpdateFilterResTypes();
            UpdateActions();
        }
    }
}

void MainWindow::onOpenRecent()
{
    QAction *action = qobject_cast<QAction*>(sender());

    if (!action)
        return;

    open(action->data().toString());
}

void MainWindow::onOpen()
{
    ResApplication *app = (ResApplication*)qApp;
    QSettings *Settings = app->settings();

    QList<QUrl> urls;
    QStringList dirs;
    int size = Settings->beginReadArray(LBR_RECENTFOLDERS_CONTEXT);
    for (int i = 0; i < size; i++)
    {
        Settings->setArrayIndex(i);

        QString dir = Settings->value(DIR_SECTION).toString();
        urls.append(QUrl::fromLocalFile(dir));
        dirs.append(dir);
    }
    Settings->endArray();

    QFileDialog dlg(this);
    dlg.setWindowTitle(tr("Выбор библиотеки"));
    dlg.setWindowIcon(QIcon::fromTheme("OpenLibrary"));
    dlg.setFileMode(QFileDialog::ExistingFile);
    dlg.setNameFilter(tr("Библиотека ресурсов (*.lbr)"));
    dlg.setOption(QFileDialog::DontUseNativeDialog, true);
    dlg.setSidebarUrls(urls);

    if (dlg.exec() == QDialog::Accepted)
    {
        QString filename = dlg.selectedFiles().at(0);

        QFileInfo fi(filename);
        dirs.append(fi.path());

        open(filename);

        Settings->beginWriteArray(LBR_RECENTFOLDERS_CONTEXT, dirs.size());
        for (int i = 0; i < dirs.size(); i++)
        {
            Settings->setArrayIndex(i);
            Settings->setValue(DIR_SECTION, dirs[i]);
        }
        Settings->endArray();
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
            qint32 id = reinterpret_cast<qint32>(editor);

            if (m_FlagMassCloseMode)
            {
                if (m_WindowsToSaveOnClose.contains(id))
                {
                    //readySave(editor);
                    qDebug() << "readySave(editor);";
                    event->accept();
                }
                else
                    event->accept();
            }
            else
            {
                QList<FileInfo> files;
                qint32 id = reinterpret_cast<qint32>(editor);
                files.append(
                {
                    id,
                    editor->name(),
                    RsResCore::inst()->typeNameFromResType(editor->type()),
                    RsResCore::inst()->iconFromResType(editor->type())
                });

                SaveFilesDlg dlg(files, this);
                if (dlg.exec() == QDialog::Accepted)
                {
                    if (dlg.saveModeResult() == SaveFilesDlg::ResultSave)
                    {
                        //readySave(editor);
                        qDebug() << "readySave(editor);";
                        event->accept();
                    }
                    else
                        event->accept();
                }
                else
                {
                    if (dlg.saveModeResult() == SaveFilesDlg::ResultDiscard)
                        event->accept();
                    else
                        event->ignore();
                }
            }
            /*QString typeName = RsResCore::inst()->typeNameFromResType(editor->type());
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
            }*/
            return true;
        }
    }

    return SARibbonMainWindow::eventFilter(watched, event);
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

void MainWindow::setAutoUnloadDir(const QString &filename)
{
    m_AutoUnloadDir = filename;
}

void MainWindow::checkUpdateFinished(bool hasUpdates, const CheckDataList &updatedata)
{
    if (hasUpdates)
    {
        pUpdateChecker->setCheckUpdateFlag(false);
        UpdateCheckerMessageBox dlg(this);
        dlg.setList(updatedata);
        dlg.exec();
        pUpdateChecker->setCheckUpdateFlag(true);
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    ResApplication *app = (ResApplication*)qApp;
    QSettings *s = app->settings();

    s->setValue("Geometry", saveGeometry());
    s->setValue("State", saveState());

    pUpdateChecker->requestInterruption();
    pUpdateChecker->deleteLater();
}

void MainWindow::subWindowIndexChanged(const int &index)
{
    /*QMdiSubWindow *wnd = pWindowsModel->window(pWindowsModel->index(index, 0));

    if (wnd)
        SetActiveWindow(wnd);*/
}

void MainWindow::SetActiveWindow(QMdiSubWindow *wnd)
{
    //wnd->setWindowState(Qt::WindowNoState);
    m_Mdi->setActiveSubWindow(wnd);
}

void MainWindow::showWindowList()
{
    WindowsListDlg dlg(pWindowsModel, m_Mdi, this);
    dlg.exec();
}

void MainWindow::onOpenRes()
{
    if (!m_pLbrObj)
        return;

    SelectResourceDlg dlg(this);
    dlg.setModel(m_pLbrObj->list());
    if (dlg.exec() == QDialog::Accepted)
    {
        int m_type;
        QString m_name;

        dlg.getRes(m_type, m_name);
        doubleResClicked(m_name, m_type);
    }
}

void MainWindow::OnResListSelectionChanged()
{
    UpdateActions();

    if (!m_ResListDock->hasSelection())
    {
        m_pActionEditRes->setText(tr("Редактировать"));
        m_pActionDeleteRes->setText(tr("Удалить"));
        return;
    }

    QString name;
    int type = 0;
    m_ResListDock->selectedResource(name, type);

    m_pActionEditRes->setText(tr("Редактировать [%1]").arg(name));
    m_pActionDeleteRes->setText(tr("Удалить [%1]").arg(name));

    m_pActionEditRes->setProperty("Name", name);
    m_pActionEditRes->setProperty("Type", type);

    m_pActionDeleteRes->setProperty("Name", name);
    m_pActionDeleteRes->setProperty("Type", type);
}

void MainWindow::UpdateFilterResTypes(bool state)
{
    Q_UNUSED(state)
    QList<qint16> FilterTypes;
    QList<QAction*> actions = m_pFilterRibbonPanel->actions();

    for (const QAction *typeAction : qAsConst(actions))
    {
        qint16 type = typeAction->property("type").value<qint16>();

        if (typeAction->isChecked())
            FilterTypes.append(type);
    }

    m_ResListDock->setFilterTypes(FilterTypes);
}

void MainWindow::closeAllSubWindows()
{
    QList<FileInfo> files;
    QList<QMdiSubWindow*> windows = m_Mdi->subWindowList();
    for (QMdiSubWindow *wnd : std::as_const(windows))
    {
        BaseEditorWindow *editor = dynamic_cast<BaseEditorWindow*>(wnd->widget());

        if (!editor)
            continue;

        if (editor->isChanged())
        {
            qint32 id = reinterpret_cast<qint32>(editor);

            files.append(
            {
                id,
                editor->name(),
                RsResCore::inst()->typeNameFromResType(editor->type()),
                RsResCore::inst()->iconFromResType(editor->type())
            });
        }
    }

    m_FlagMassCloseMode = true;
    m_WindowsToSaveOnClose.clear();
    if (!files.isEmpty())
    {
        SaveFilesDlg dlg(files, this);
        if (dlg.exec() == QDialog::Accepted)
        {
            if (dlg.saveModeResult() == SaveFilesDlg::ResultSave)
            {
                m_WindowsToSaveOnClose = dlg.selectedIds();
                m_Mdi->closeAllSubWindows();
            }
            else
                m_Mdi->closeAllSubWindows();
        }
        else
        {
            if (dlg.saveModeResult() == SaveFilesDlg::ResultDiscard)
                m_Mdi->closeAllSubWindows();
        }
    }
    else
        m_Mdi->closeAllSubWindows();

    m_FlagMassCloseMode = false;
    m_WindowsToSaveOnClose.clear();
}

QMdiSubWindow *MainWindow::IsExistsResWindow(const QString &name, const int &type)
{
    QList<QMdiSubWindow*> windows = m_Mdi->subWindowList();
    for (QMdiSubWindow *wnd : std::as_const(windows))
    {
        BaseEditorWindow *editor = dynamic_cast<BaseEditorWindow*>(wnd->widget());

        if (!editor)
            continue;

        if (!name.compare(editor->name(), Qt::CaseInsensitive) && editor->type() == type)
            return wnd;
    }

    return nullptr;
}
