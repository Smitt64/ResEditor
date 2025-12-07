#include "mainwindow.h"
#include "lbrobject.h"
#include "newitemsdlg.h"
#include "respanel.h"
#include "resxmlloader.h"
#include "ui_mainwindow.h"
#include "reslistdockwidget.h"
#include "resbuffer.h"
#include "ResourceEditorInterface.h"
#include "baseeditorwindow.h"
#include "propertymodel/propertydockwidget.h"
#include "toolbox/toolboxdockwidget.h"
#include "rsrescore.h"
#include "errorsmodel.h"
#include <errordlg.h>
#include "proxyaction.h"
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
#include <QUndoView>
#include <QUuid>
#include <QProgressDialog>
#include <QDirIterator>
#include <QGridLayout>
#include <QtConcurrent/QtConcurrent>
#include <QFuture>
#include "savefilesdlg.h"
#include "reslibwriter.h"
#include "xmlvalidator.h"
#include <toolsruntime.h>

class UndoActionWidget : public QWidgetAction
{
public:
    UndoActionWidget(QObject *parent) :
        QWidgetAction(parent),
        m_pPtr(nullptr),
        m_pUndoStack(nullptr)
    {
    }

    virtual ~UndoActionWidget()
    {

    }

    void setUndoStack(QUndoStack *pStack)
    {
        m_pUndoStack = pStack;

        if (m_pPtr)
            m_pPtr->setStack(m_pUndoStack);
    }

protected:
    virtual QWidget *createWidget(QWidget *parent)
    {
        m_pPtr = new QUndoView(parent);

        if (m_pUndoStack)
            m_pPtr->setStack(m_pUndoStack);

        return m_pPtr;
    }

private:
    QUndoView *m_pPtr;
    QUndoStack *m_pUndoStack;
};

MainWindow::MainWindow(QWidget *parent)
    : SARibbonMainWindow(parent)
    , ui(new Ui::MainWindow),
    m_LastActiveWindow(nullptr),
    m_pLbrObj(nullptr),
    m_FlagMassCloseMode(false)
{
    ResApplication *app = (ResApplication*)qApp;
    QSettings *Settings = app->settings();
    RsResCore::inst()->setSettings(Settings);

    setWindowIcon(QIcon("://res/appicon-blue.svg"));

    ui->setupUi(this);
    SARibbonBar* ribbon = ribbonBar();
    SARibbonCategory *mainPage = new SARibbonCategory("Главная");
    ribbon->addCategoryPage(mainPage);

    SARibbonCategory *viewPage = new SARibbonCategory("Вид");
    ribbon->addCategoryPage(viewPage);

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
    m_Mdi->setBackground(QBrush(QColor(248, 248, 248)));
    setCentralWidget(m_Mdi);
    SetupMenus();

    InitQuickAccessBar();
    InitButtonBar();
    InitLbrPanel(mainPage);
    InitLbrResourcePanel(mainPage);
    InitViewBar(viewPage);
    InitContextCategory();

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

    QSettings *s = app->settings();
    restoreGeometry(s->value("Geometry").toByteArray());
    restoreState(s->value("State").toByteArray());

    UpdateActions();

    connect(m_ResListKey, &QShortcut::activated, m_ResListDock, &QDockWidget::raise);
    connect(m_ToolsListKey, &QShortcut::activated, m_ToolBoxDock, &QDockWidget::raise);
    connect(m_ResListDock, &ResListDockWidget::doubleClicked, this, &MainWindow::doubleResClicked);
    connect(m_ResListDock, &ResListDockWidget::deleteRequest, this, &MainWindow::OnDeleteRequest);
    connect(btn, &QAbstractButton::clicked, this, &MainWindow::onOpenRes);

    connect(m_ResListDock, &ResListDockWidget::selectionChanged, this, &MainWindow::OnResListSelectionChanged);
    connect(m_Mdi, &QMdiArea::subWindowActivated, this, &MainWindow::subWindowActivated);
    connect(ribbon, &SARibbonBar::currentRibbonTabChanged, this, &MainWindow::OnCurrentRibbonTabChanged);
}

MainWindow::~MainWindow()
{
    delete m_ResListKey;
    delete m_ToolsListKey;
}

void MainWindow::InitContextCategory()
{
    (void)RsResCore::inst()->contextCategoryes(ribbonBar());
}

void MainWindow::InitQuickAccessBar()
{
    ResApplication *app = (ResApplication*)qApp;
    m_RecentLbrList.reset(new RecentLbrList(app->settings()));

    SARibbonQuickAccessBar* quickAccessBar = ribbonBar()->quickAccessBar();

    // Действие: Создание нового ресурса или файла
    QAction* actionNew = createAction(tr("Создать"), "NewFile");
    actionNew->setShortcut(QKeySequence::New);
    actionNew->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(actionNew);
    quickAccessBar->addSeparator();

    // Действие: Сохранение текущего ресурса
    m_pActionSave = createAction(tr("Сохранить"), "Save");
    m_pActionSave->setShortcut(QKeySequence::Save);
    m_pActionSave->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(m_pActionSave);
    quickAccessBar->addSeparator();

    // Действие: Отмена последней операции
    m_pActionUndo = createAction<ProxyAction>(tr("Отменить"), "Undo");
    m_pActionUndo->setShortcut(QKeySequence::Undo);
    m_pActionUndo->setShortcutContext(Qt::ApplicationShortcut);
    quickAccessBar->addAction(m_pActionUndo);

    // Добавляем подсказку для Undo
    toolAddActionWithTooltip(m_pActionUndo,
                             tr("Отменяет последнюю выполненную операцию"),
                             QKeySequence::Undo);

    // Действие: Повтор отмененной операции
    m_pActionRedo = createAction<ProxyAction>(tr("Повторить"), "Redo");
    m_pActionRedo->setShortcut(QKeySequence::Redo);
    m_pActionRedo->setShortcutContext(Qt::ApplicationShortcut);

    // Добавляем подсказку для Redo
    toolAddActionWithTooltip(m_pActionRedo,
                             tr("Повторяет отмененную операцию"),
                             QKeySequence::Redo);

    m_pUndoRedoMenu = new QMenu(tr("Повторить"), this);
    m_pUndoRedoMenu->setIcon(QIcon::fromTheme("Redo"));

    // Добавляем описание для меню Undo/Redo
    toolAddActionWithTooltip(m_pActionRedo,
                             tr("Показывает историю операций и позволяет повторять отмененные действия"),
                             QKeySequence::Redo);

    m_pUndoActionWidget = new UndoActionWidget(this);
    m_pUndoRedoMenu->addAction(m_pActionRedo);
    m_pUndoRedoMenu->addAction(m_pUndoActionWidget);

    quickAccessBar->addMenu(m_pUndoRedoMenu);

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

    // Добавляем подсказки к действиям
    toolAddActionWithTooltip(actionNew,
                             tr("Создает новый ресурс или открывает диалог создания"),
                             QKeySequence::New);

    toolAddActionWithTooltip(m_pActionSave,
                             tr("Сохраняет текущий редактируемый ресурс"),
                             QKeySequence::Save);

    connect(actionNew, &QAction::triggered, this, &MainWindow::onNew);
    connect(m_pActionSave, &QAction::triggered, this, &MainWindow::onSave);
}

void MainWindow::InitButtonBar()
{
    SARibbonSystemButtonBar* wbar = windowButtonBar();
    if (!wbar)
        return;

    pWindowsComboBox = new SARibbonComboBox(this);
    pWindowsComboBox->setMinimumWidth(250);
    pWindowsComboBox->setFrame(false);
    pWindowsComboBox->setFocusPolicy(Qt::NoFocus);
    wbar->addWidget(pWindowsComboBox);
    wbar->addSeparator();

    // Действие: Открытие диалога параметров приложения
    QAction* optionsAction = createAction(tr("Параметры"), "Settings");
    wbar->addAction(optionsAction);

    wbar->addSeparator();

    // Действие: Открытие диалога "О программе"
    QAction* about = wbar->addAction(tr("О программе"), QIcon::fromTheme("HelpApplication"), Qt::ToolButtonIconOnly);
    wbar->addSeparator();

    // Добавляем подсказки
    toolAddActionWithTooltip(optionsAction,
                             tr("Открывает диалог настроек и параметров приложения"));

    toolAddActionWithTooltip(about,
                             tr("Показывает информацию о программе, версии и авторских правах"));

    connect(about, &QAction::triggered, this, &MainWindow::onAbout);
    connect(optionsAction, &QAction::triggered, this, &MainWindow::onOptions);
    connect(pWindowsComboBox, SIGNAL(currentIndexChanged(int)), SLOT(subWindowIndexChanged(int)));
}

void MainWindow::InitLbrPanel(SARibbonCategory *category)
{
    SARibbonPannel* libPannel = new SARibbonPannel(tr("Библиотека"));
    category->addPannel(libPannel);

    // Действие: Создание новой библиотеки ресурсов
    m_pActionNew = createAction(tr("Создать"), "NewLibrary");
    libPannel->addLargeAction(m_pActionNew);

    // Действие: Открытие существующей библиотеки
    m_pActionOpen = createAction(tr("Открыть"), "OpenLibrary");
    m_pActionOpen->setShortcut(QKeySequence::Open);
    libPannel->addLargeAction(m_pActionOpen);

    libPannel->addSeparator();

    // Действие: Импорт ресурсов из XML файла
    m_ImportXml = createAction(tr("Импорт XML файла"), "ImportXml");
    libPannel->addLargeAction(m_ImportXml);

    // Действие: Импорт ресурсов из каталога с XML файлами
    m_pImportXmlFolder = createAction(tr("Импорт из каталога"), "ImportCatalogPart");
    libPannel->addSmallAction(m_pImportXmlFolder);

    // Действие: Экспорт библиотеки в XML файл
    m_pExportXmlFile = createAction(tr("Экспорт в XML файл"), "ExportXml");
    libPannel->addSmallAction(m_pExportXmlFile);

    // Действие: Экспорт библиотеки в каталог с XML файлами
    m_pExportXmlFolder = createAction(tr("Экспорт в каталог"), "ExportFolder");
    libPannel->addSmallAction(m_pExportXmlFolder);

    // Добавляем подсказки
    toolAddActionWithTooltip(m_pActionNew,
                             tr("Создает новую библиотеку ресурсов (.lbr файл)"));

    toolAddActionWithTooltip(m_pActionOpen,
                             tr("Открывает существующую библиотеку ресурсов"),
                             QKeySequence::Open);

    toolAddActionWithTooltip(m_ImportXml,
                             tr("Загружает ресурсы из выбранного XML файла в текущую библиотеку"));

    toolAddActionWithTooltip(m_pImportXmlFolder,
                             tr("Загружает ресурсы из всех XML файлов в указанном каталоге"));

    toolAddActionWithTooltip(m_pExportXmlFile,
                             tr("Экспортирует всю библиотеку в один XML файл"));

    toolAddActionWithTooltip(m_pExportXmlFolder,
                             tr("Экспортирует каждый ресурс библиотеки в отдельный XML файл"));

    connect(m_pActionNew, &QAction::triggered, this, &MainWindow::onNewLbr);
    connect(m_pActionOpen, &QAction::triggered, this, &MainWindow::onOpen);
    connect(m_ImportXml, &QAction::triggered, this, &MainWindow::OnImportXmlFile);
    connect(m_pImportXmlFolder, &QAction::triggered, this, &MainWindow::OnImportXmlDir);
    connect(m_pExportXmlFile, &QAction::triggered, this, &MainWindow::OnExportXml);
    connect(m_pExportXmlFolder, &QAction::triggered, this, &MainWindow::OnExportXmlDir);
}

void MainWindow::InitLbrResourcePanel(SARibbonCategory *category)
{
    SARibbonPannel* resPannel = new SARibbonPannel(tr("Ресурс"));
    category->addPannel(resPannel);

    LbrObjectInterface *tmpLbrInterface = nullptr;
    CreateLbrObject(&tmpLbrInterface, this);

    QScopedPointer<LbrObjectInterface> ptr(tmpLbrInterface);
    NewItemsDlg dlg(tmpLbrInterface);
    dlg.buildStandartNewItems();

    QStringList panels = dlg.ribbonPannels();
    if (!panels.isEmpty())
    {
        if (panels.size() > 1)
        {
            QMenu *panelmenu = new QMenu(this);
            panelmenu->setIcon(QIcon::fromTheme("NewDialog"));
            panelmenu->setTitle(tr("Создать PANEL"));

            for (const QString &guid : panels)
            {
                GroupInfoMap info = dlg.getInfoForItem(guid);

                QAction *action = panelmenu->addAction(info[RoleTitle].toString());
                action->setData(guid);

                if (guid == panels.at(0))
                {
                    panelmenu->setDefaultAction(action);
                    panelmenu->setActiveAction(action);
                }
            }

            // Действие: Создание нового ресурса типа PANEL (меню с подтипами)
            m_pActionNewPanel = resPannel->addLargeMenu(panelmenu, QToolButton::MenuButtonPopup);
            m_pActionNewPanel->setData(panels[0]);

            // Добавляем подсказку
            toolAddActionWithTooltip(m_pActionNewPanel,
                                     tr("Создает новый ресурс типа PANEL или его подтип"));

            connect(resPannel, &SARibbonPannel::actionTriggered, this, &MainWindow::OnNewResActionEx);
        }
        else
        {
            // Действие: Создание нового ресурса типа PANEL
            m_pActionNewPanel = createAction(tr("Создать PANEL"), "NewDialog");
            m_pActionNewPanel->setData(panels[0]);
            resPannel->addLargeAction(m_pActionNewPanel);

            // Добавляем подсказку
            toolAddActionWithTooltip(m_pActionNewPanel,
                                     tr("Создает новый ресурс типа PANEL"));
        }
    }

    QStringList scrols = dlg.ribbonScrols();
    if (!scrols.isEmpty())
    {
        if (scrols.size() > 1)
        {
            QMenu *panelmenu = new QMenu(this);
            panelmenu->setIcon(QIcon::fromTheme("NewTable"));
            panelmenu->setTitle(tr("Создать BSCROL"));

            for (const QString &guid : scrols)
            {
                GroupInfoMap info = dlg.getInfoForItem(guid);

                QAction *action = panelmenu->addAction(info[RoleTitle].toString());
                action->setData(guid);

                if (guid == scrols.at(0))
                {
                    panelmenu->setDefaultAction(action);
                    panelmenu->setActiveAction(action);
                }
            }
        }
        else
        {
            // Действие: Создание нового ресурса типа BSCROL
            m_pActionNewBScrol = createAction(tr("Создать BSCROL"), "NewTable");
            m_pActionNewBScrol->setData(scrols[0]);
            resPannel->addLargeAction(m_pActionNewBScrol);

            // Добавляем подсказку
            toolAddActionWithTooltip(m_pActionNewBScrol,
                                     tr("Создает новый ресурс типа BSCROL (скролинговая панель)"));
        }
    }

    // Действие: Редактирование выбранного ресурса
    m_pActionEditRes = createAction(tr("Редактировать"), "EditDocument");
    resPannel->addMediumAction(m_pActionEditRes);

    // Действие: Удаление выбранного ресурса
    m_pActionDeleteRes = createAction(tr("Удалить"), "DeleteTag");
    resPannel->addMediumAction(m_pActionDeleteRes);

    // Добавляем подсказки
    toolAddActionWithTooltip(m_pActionEditRes,
                             tr("Открывает выбранный ресурс для редактирования"));

    toolAddActionWithTooltip(m_pActionDeleteRes,
                             tr("Удаляет выбранный ресурс из библиотеки"),
                             QKeySequence::Delete);

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

    // Добавляем подсказки для панелей
    toolAddActionWithTooltip(actionResList,
                             tr("Показывает или скрывает список ресурсов библиотеки"),
                             QKeySequence("Alt+1"));

    toolAddActionWithTooltip(actionProperty,
                             tr("Показывает или скрывает панель свойств"),
                             QKeySequence("Alt+3"));

    toolAddActionWithTooltip(actionToolBox,
                             tr("Показывает или скрывает панель инструментов"),
                             QKeySequence("Alt+2"));

    m_pFilterRibbonPanel = new SARibbonPannel(tr("Фильтр"));
    category->addPannel(m_pFilterRibbonPanel);

    const QList<qint16> types = RsResCore::types();
    const QList<qint16> stdtypes = RsResCore::stdTypes();
    for (const qint16 &type : types)
    {
        QString name = RsResCore::typeNameFromResType(type);
        QString iconName = RsResCore::iconNameFromResType(type);

        // Действие: Фильтр по типу ресурса (переключаемое)
        QAction *typeAction = createAction(name, iconName);
        typeAction->setCheckable(true);
        typeAction->setChecked(true);
        typeAction->setProperty("type", type);

        if (!stdtypes.contains(type))
            typeAction->setVisible(false);

        m_pFilterRibbonPanel->addSmallAction(typeAction);

        // Добавляем подсказку для фильтра
        QString description = tr("Показывает или скрывает ресурсы типа %1 в списке").arg(name);
        toolAddActionWithTooltip(typeAction, description);

        connect(typeAction, &QAction::toggled, this, &MainWindow::UpdateFilterResTypes);
    }
    m_pFilterRibbonPanel->resetToolButtonSize();

    pWindowsModel = new SubWindowsModel(this);
    pWindowsComboBox->setModel(pWindowsModel);

    SARibbonPannel* windowsPannel = new SARibbonPannel(tr("Окно"));
    category->addPannel(windowsPannel);

    QAction* optAct = new QAction(this);
    windowsPannel->setOptionAction(optAct);

    // Добавляем подсказку для опции
    toolAddActionWithTooltip(optAct,
                             tr("Открывает диалог со списком всех открытых окон"));

    connect(optAct, &QAction::triggered, this, [this](bool on)
            {
                Q_UNUSED(on);
                showWindowList();
            });

    // Действие: Активация следующего окна редактирования
    QAction *nextWindow = createAction(tr("Следующее"), "NextDocument");
    // Действие: Активация предыдущего окна редактирования
    QAction *prevWindow = createAction(tr("Предыдущее"), "PreviousDocument");
    windowsPannel->addMediumAction(nextWindow);
    windowsPannel->addMediumAction(prevWindow);
    windowsPannel->addSeparator();

    // Действие: Закрытие активного окна редактирования
    QAction *closeWindow = createAction(tr("Закрыть"), "CloseDocument");
    // Действие: Закрытие всех окон редактирования
    QAction *closeAllWindow = createAction(tr("Закрыть все"), "CloseDocumentGroup");
    windowsPannel->addMediumAction(closeWindow);
    windowsPannel->addMediumAction(closeAllWindow);

    // Добавляем подсказки для управления окнами
    toolAddActionWithTooltip(nextWindow,
                             tr("Активирует следующее окно в списке открытых окон"),
                             QKeySequence("Ctrl+Tab"));

    toolAddActionWithTooltip(prevWindow,
                             tr("Активирует предыдущее окно в списке открытых окон"),
                             QKeySequence("Ctrl+Shift+Tab"));

    toolAddActionWithTooltip(closeWindow,
                             tr("Закрывает текущее активное окно редактирования"),
                             QKeySequence("Ctrl+F4"));

    toolAddActionWithTooltip(closeAllWindow,
                             tr("Закрывает все открытые окна редактирования"));

    connect(nextWindow, SIGNAL(triggered(bool)), m_Mdi, SLOT(activateNextSubWindow()));
    connect(prevWindow, SIGNAL(triggered(bool)), m_Mdi, SLOT(activatePreviousSubWindow()));
    connect(closeWindow, SIGNAL(triggered(bool)), m_Mdi, SLOT(closeActiveSubWindow()));
    connect(closeAllWindow, SIGNAL(triggered(bool)), this, SLOT(closeAllSubWindows()));
}

void MainWindow::setupAction(QAction* act, const QString& text, const QString& iconname)
{
    act->setText(text);
    act->setIcon(QIcon::fromTheme(iconname));
    act->setObjectName(text);
}

void MainWindow::UpdateActions()
{
    bool EnableIfOpenLbr = (m_pLbrObj != nullptr);
    bool EnableIfOpenLbrSelRes = (m_pLbrObj != nullptr && m_ResListDock->hasSelection());
    bool EnableIfResModifyed = false;
    bool EnableIfResOpened = false;

    QMdiSubWindow *mdiwnd = m_Mdi->currentSubWindow();
    if (mdiwnd)
    {
        BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(mdiwnd->widget());
        EnableIfResModifyed = wnd && wnd->isChanged();
        EnableIfResOpened = wnd != nullptr;
    }

    m_pImportXmlFolder->setEnabled(EnableIfOpenLbr);
    m_ImportXml->setEnabled(EnableIfOpenLbr);
    m_pExportXmlFolder->setEnabled(EnableIfOpenLbr);
    m_pExportXmlFile->setEnabled(EnableIfOpenLbr);

    m_pActionNewPanel->setEnabled(EnableIfOpenLbr);
    m_pActionNewBScrol->setEnabled(EnableIfOpenLbr);
    m_pActionDeleteRes->setEnabled(EnableIfOpenLbrSelRes);
    m_pActionEditRes->setEnabled(EnableIfOpenLbrSelRes);

    m_pUndoRedoMenu->setEnabled(EnableIfResOpened);
    m_pActionSave->setEnabled(EnableIfResModifyed);
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
    editor->setRibbonBar(ribbonBar());

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

    connect(editor, &BaseEditorWindow::modifyChanged, this, &MainWindow::UpdateActions);
}

void MainWindow::onSave()
{
    QMdiSubWindow *wnd = m_Mdi->currentSubWindow();

    if (!wnd)
        return;

    BaseEditorWindow *editor = qobject_cast<BaseEditorWindow*>(wnd->widget());

    if (!editor)
        return;

    readySave(editor);
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

            for (const QString &dir : std::as_const(lst))
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
        if (m_LastActiveWindow)
        {
            BaseEditorWindow *lastwnd = dynamic_cast<BaseEditorWindow*>(m_LastActiveWindow->widget());

            if (lastwnd)
            {
                QList<QWidget*> status = lastwnd->statusBarSections();
                lastwnd->clearRibbonTabs();

                for (auto widget : qAsConst(status))
                    ui->statusbar->removeWidget(widget);
            }
        }

        QList<SARibbonContextCategory*> allCategoryes = ribbonBar()->contextCategoryList();
        for (auto all : qAsConst(allCategoryes))
        {
            if (all->categoryCount())
                ribbonBar()->showContextCategory(all);
            else
                ribbonBar()->hideContextCategory(all);
        }

        m_ToolBoxDock->setModel(nullptr);
        m_PropertyDock->setPropertyModel(nullptr);
        m_PropertyDock->setStructModel(nullptr);
        m_LastActiveWindow = nullptr;

        return;
    }

    if (m_LastActiveWindow == window)
    {
        return;
    }

    BaseEditorWindow *lastwnd = nullptr;
    BaseEditorWindow *wnd = dynamic_cast<BaseEditorWindow*>(window->widget());

    if (m_LastActiveWindow)
        lastwnd = dynamic_cast<BaseEditorWindow*>(m_LastActiveWindow->widget());

    if (!wnd)
    {
        m_ToolBoxDock->setModel(nullptr);
        m_PropertyDock->setPropertyModel(nullptr);
        m_PropertyDock->setStructModel(nullptr);

        m_pActionUndo->setSource(nullptr);
        m_pActionRedo->setSource(nullptr);
        m_pUndoActionWidget->setUndoStack(nullptr);

        if (lastwnd)
        {
            QList<QWidget*> status = lastwnd->statusBarSections();
            lastwnd->clearRibbonTabs();

            for (auto widget : qAsConst(status))
                ui->statusbar->removeWidget(widget);
        }

        m_LastActiveWindow = nullptr;
    }
    else
    {
        m_ToolBoxDock->setModel(wnd->toolBox());
        m_PropertyDock->setPropertyModel(wnd->propertyModel());
        m_PropertyDock->setStructModel(wnd->structModel());

        m_pActionUndo->setSource(wnd->undoAction());
        m_pActionRedo->setSource(wnd->redoAction());
        m_pUndoActionWidget->setUndoStack(wnd->undoStack());

        QModelIndex index = pWindowsModel->findWindow(window);
        pWindowsComboBox->setCurrentIndex(index.row());

        m_LastRibbonTabName.lock();

        ribbonBar()->setUpdatesEnabled(false);
        if (lastwnd)
            lastwnd->clearRibbonTabs();

        wnd->updateRibbonTabs();

        if (!m_LastRibbonTabName.get().isEmpty())
            ribbonBar()->raiseCategory(ribbonBar()->categoryByName(m_LastRibbonTabName));

        QList<SARibbonContextCategory*> allCategoryes = ribbonBar()->contextCategoryList();
        for (auto all : qAsConst(allCategoryes))
        {
            if (all->categoryCount())
                ribbonBar()->showContextCategory(all);
            else
                ribbonBar()->hideContextCategory(all);
        }
        ribbonBar()->setUpdatesEnabled(true);

        QList<QWidget*> status = wnd->statusBarSections();
        for (auto widget : qAsConst(status))
            ui->statusbar->addPermanentWidget(widget);

        m_LastActiveWindow = window;

        m_LastRibbonTabName.unlock();
    }
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
            ResourceEditorResult resulst = interface->newItemsAction(guid, name, path);

            if (resulst.wnd)
                AddEditorWindow(resulst.wnd);
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

            setWindowTitle(QString("%1 - %2").arg(RecentLbrList::formatName(filename), WORKLBR_TITLE));
        }
        else
            QMessageBox::critical(this, tr("Ошибка!"), tr("Ошибка открытия файла: ") + m_pLbrObj->lastError());
    }
}

void MainWindow::onOpenRecent()
{
    QAction *action = qobject_cast<QAction*>(sender());

    if (!action)
        return;

    open(action->data().toString());
}

void MainWindow::onNewLbr()
{
    static const QString guid = "{c7e4dbe9-cd8e-4eaf-bcd3-975f9fb6ba1e}";
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
    dlg.setWindowTitle(tr("Создание библиотеки"));
    dlg.setWindowIcon(QIcon::fromTheme("NewLibrary"));
    dlg.setFileMode(QFileDialog::AnyFile);
    dlg.setAcceptMode(QFileDialog::AcceptSave);
    dlg.setNameFilter(tr("Библиотека ресурсов (*.lbr)"));
    dlg.setOption(QFileDialog::DontUseNativeDialog, true);
    dlg.setSidebarUrls(urls);

    if (dlg.exec() == QDialog::Accepted)
    {
        QString filename = dlg.selectedFiles().at(0);

        QFileInfo fi(filename);
        if (fi.completeSuffix().isEmpty())
            filename = fi.fileName() + ".lbr";
        else
            filename = fi.fileName();

        ResourceEditorInterface *interface = RsResCore::inst()->pluginForNewAction(guid);

        if (!interface)
            return;

        ResourceEditorResult result = interface->newItemsAction(guid, filename, fi.path());

        if (result.succeed)
        {
            QDir d(fi.path());
            open(d.absoluteFilePath(filename));
        }
    }
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
                    readySave(editor);
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
                        readySave(editor);
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

void MainWindow::showEvent(QShowEvent *event)
{
    SARibbonBar* ribbon = ribbonBar();
    SARibbonMainWindow::showEvent(event);
    setRibbonTheme(SARibbonTheme::RibbonThemeOffice2013);

    QFile file;
    file.setFileName("://res/theme-office2013-blue.qss");
    file.open(QIODevice::ReadOnly | QIODevice::Text);

    QString qss = QString::fromUtf8(file.readAll());
    setStyleSheet(qss);
    ribbon->setTabBarBaseLineColor(QColor(0, 114, 198));
    ribbon->setWindowTitleTextColor(QColor(0, 114, 198));

    setContentsMargins(2,2,2,2);

    update();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    bool canceled = false;
    closeAllSubWindows(&canceled);

    if (canceled)
    {
        event->ignore();
        return;
    }

    ResApplication *app = (ResApplication*)qApp;
    QSettings *s = app->settings();

    s->setValue("Geometry", saveGeometry());
    s->setValue("State", saveState());

    pUpdateChecker->requestInterruption();
    pUpdateChecker->deleteLater();
}

void MainWindow::subWindowIndexChanged(const int &index)
{
    QMdiSubWindow *wnd = pWindowsModel->window(pWindowsModel->index(index, 0));

    if (wnd)
        SetActiveWindow(wnd);
}

void MainWindow::SetActiveWindow(QMdiSubWindow *wnd)
{
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

void MainWindow::closeAllSubWindows(bool *canceled)
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
            else
            {
                if (canceled)
                    *canceled = true;
            }
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

void MainWindow::OnCurrentRibbonTabChanged(int index)
{
    SARibbonCategory *categ = ribbonBar()->categoryByIndex(index);

    if (categ)
        m_LastRibbonTabName = categ->categoryName();
}

void MainWindow::OnNewResAction()
{
    OnNewResActionEx(qobject_cast<QAction*>(sender()));
}

void MainWindow::OnNewResActionEx(QAction *action)
{
    QString guid = action->data().toString();
    QUuid uuid = QUuid::fromString(guid);

    if (m_pActionNewPanel == action || m_pActionNewBScrol == action || !uuid.isNull())
    {
        NewItemsDlg dlg(m_pLbrObj, this);
        dlg.buildStandartNewItems();
        dlg.filterByAction(guid);

        if (dlg.exec() == QDialog::Accepted)
        {
            QString guid = dlg.action();
            QString name = dlg.name();
            QString path = dlg.path();
            ResourceEditorInterface *interface = RsResCore::inst()->pluginForNewAction(guid);

            if (interface)
            {
                ResourceEditorResult resulst = interface->newItemsAction(guid, name, path);

                if (resulst.wnd)
                    AddEditorWindow(resulst.wnd);
            }
        }
    }
}

bool MainWindow::processSingleImportXmlFile(const QString& filePath, ErrorsModel* errorsModel)
{
    QFileInfo fileInfo(filePath);

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly))
    {
        if (errorsModel)
            errorsModel->appendError(tr("Не удалось открыть файл <b>%1</b>").arg(filePath), ErrorsModel::TypeError);
        return false;
    }

    ResXmlLoader loader(m_pLbrObj, errorsModel);
    loader.readXml(&file);

    file.close();

    return true;
}

void MainWindow::processImportXmlWithProgress(const QStringList& filePaths, ErrorsModel* errorsModel,
                                              QWidget* parent, const QString& dialogTitle,
                                              const QString& dialogLabel)
{
    QProgressDialog* progressDialog = new QProgressDialog(dialogLabel, tr("Отмена"), 0, filePaths.size(), parent);
    progressDialog->setWindowTitle(dialogTitle);
    progressDialog->setWindowModality(Qt::WindowModal);
    progressDialog->setMinimumDuration(500);
    progressDialog->setValue(0);
    progressDialog->setMinimumWidth(400);

    for (int i = 0; i < filePaths.size(); ++i)
    {
        if (progressDialog->wasCanceled())
            break;

        QFileInfo fi(filePaths[i]);
        progressDialog->setValue(i);
        progressDialog->setLabelText(tr("Обработка файла %1 из %2: %3")
                                         .arg(i + 1)
                                         .arg(filePaths.size())
                                         .arg(fi.fileName()));

        processSingleImportXmlFile(filePaths[i], errorsModel);
        QApplication::processEvents();
    }

    progressDialog->setValue(filePaths.size());
}

void MainWindow::OnImportXmlFile()
{
    QStringList filePaths = QFileDialog::getOpenFileNames(this,
                                                          tr("Выберите XML файлы для загрузки"), QString(),
                                                          tr("XML файлы (*.xml);"));

    if (filePaths.isEmpty())
        return;

    ErrorsModel errors;
    processImportXmlWithProgress(filePaths, &errors, this,
                                 tr("Загрузка файлов"), tr("Загрузка XML файлов..."));

    ErrorDlg dlg(ErrorDlg::ModeInformation, this);
    dlg.setErrors(&errors);
    dlg.exec();
}

void MainWindow::OnImportXmlDir()
{
    QFileDialog dlg(this);
    dlg.setWindowTitle(tr("Выберите каталог с XML файлами"));
    dlg.setOption(QFileDialog::DontUseNativeDialog, true);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);

    QCheckBox* recursiveCheckbox = new QCheckBox(tr("Искать XML файлы в подкаталогах"), &dlg);
    recursiveCheckbox->setChecked(true);

    QGridLayout* layout = qobject_cast<QGridLayout*>(dlg.layout());
    if (layout)
    {
        int rowCount = layout->rowCount();
        layout->addWidget(recursiveCheckbox, rowCount, 0, 1, -1);
    }

    QString directoryPath;
    if (dlg.exec() == QDialog::Accepted)
    {
        QStringList selectedDirs = dlg.selectedFiles();
        if (!selectedDirs.isEmpty())
            directoryPath = selectedDirs.first();
    }

    if (!directoryPath.isEmpty())
    {
        QDirIterator::IteratorFlags iteratorFlags =
            recursiveCheckbox->isChecked() ? QDirIterator::Subdirectories : QDirIterator::NoIteratorFlags;

        QStringList xmlFiles;
        QDirIterator it(directoryPath, QStringList() << "*.xml", QDir::Files | QDir::Readable, iteratorFlags);

        while (it.hasNext())
            xmlFiles.append(it.next());

        if (xmlFiles.isEmpty())
        {
            QMessageBox::information(this, tr("Информация"), tr("В каталоге не найдено XML файлов"));
            return;
        }
        else
        {
            ErrorsModel errors;
            processImportXmlWithProgress(xmlFiles, &errors, this,
                                         tr("Загрузка файлов"), tr("Загрузка XML файлов..."));

            ErrorDlg dlg(ErrorDlg::ModeInformation, this);
            dlg.setErrors(&errors);
            dlg.exec();
        }
    }
}

void MainWindow::OnExportXml()
{
    QFileInfo fi(m_pLbrObj->fileName());
    QString filename = QFileDialog::getSaveFileName(this, tr("Сохранение библиотеки в файл"), fi.baseName() + ".xml", tr("XML файлы (*.xml);"));

    if (filename.isEmpty())
        return;

    ErrorsModel errors;
    ResLibWriter writer;

    if (!writer.begin(filename))
    {
        errors.addError(tr("Ошибка выгрузки библиотеки в файл %1. %2").arg(filename, writer.errorString()));

        ErrorDlg dlg(ErrorDlg::ModeInformation, this);
        dlg.setErrors(&errors);
        dlg.exec();
        return;
    }

    QAbstractItemModel *ResModel = m_pLbrObj->list();

    QProgressDialog progress(this);
    progress.setWindowTitle(tr("Обработка ресурсов"));
    progress.setLabelText(tr("Обработка ресурсов"));
    progress.setMaximum(ResModel->rowCount());
    progress.show();

    for (int i = 0; i < ResModel->rowCount(); i++ )
    {
        if (progress.wasCanceled())
            break;

        QString name = ResModel->data(ResModel->index(i, 0)).toString();
        qint16 type = ResModel->data(ResModel->index(i, 1)).toInt();

        progress.setLabelText(tr("Обработка ресурса %1 из %2")
                                  .arg(i + 1, 4)
                                  .arg(ResModel->rowCount(), 4));

        QApplication::processEvents();
        ResBuffer *buffer = nullptr;

        m_pLbrObj->getResource(name, type, &buffer);

        ResPanel panel;
        if (panel.load(buffer))
            errors.addError(tr("Ошибка загрузка ресурса <b>%1 [%2]</b>").arg(RsResCore::inst()->typeNameFromResType(type), name));
        else
        {
            if (panel.saveToXml(writer.writer()))
                errors.addMessage(tr("Ресурс <b>%1 [%2]</b> успешно сохранен").arg(RsResCore::inst()->typeNameFromResType(type), name));
            else
                errors.addError(tr("Не удалось сохранить ресурс <b>%1 [%2]</b>").arg(RsResCore::inst()->typeNameFromResType(type), name));
        }

        progress.setValue(i + 1);
        QApplication::processEvents();

        delete buffer;
    }

    writer.end();

    if (QMessageBox::question(this, tr("Проверка файла"), tr("Проверить файл по xsd схеме?")))
    {
        progress.show();
        progress.setLabelText(tr("Проверка файла по xsd схеме"));
        progress.setRange(0, 0);
        progress.setCancelButton(nullptr);

        QFuture<bool> future = QtConcurrent::run([&errors,filename]()
                                                 {
                                                     QFile libxml(filename);
                                                     if (libxml.open(QIODevice::ReadOnly))
                                                     {
                                                         bool res = RsResCore::inst()->validateResXmlWithXsd(&libxml, &errors);
                                                         libxml.close();
                                                         return res;
                                                     }

                                                     return false;
                                                 });

        while (!future.isFinished())
        {
            QApplication::processEvents(QEventLoop::AllEvents, 100);
            QThread::msleep(25);
        }

        if (future.result() && !future.isCanceled())
            errors.addMessage(tr("Валидация по xsd прошла успешно"));
    }

    ErrorDlg dlg(ErrorDlg::ModeInformation, this);
    dlg.setErrors(&errors);
    dlg.exec();
}

void MainWindow::OnExportXmlDir()
{
    QFileInfo fi(m_pLbrObj->fileName());

    QFileDialog dlg(this);
    dlg.setWindowTitle(tr("Сохранение библиотеки в каталог"));
    dlg.setOption(QFileDialog::DontUseNativeDialog, true);
    dlg.setFileMode(QFileDialog::Directory);
    dlg.setOption(QFileDialog::ShowDirsOnly, true);

    QCheckBox* xsdCheckbox = new QCheckBox(tr("Проверять XML файлы по XSD схеме"), &dlg);
    xsdCheckbox->setChecked(true);

    QGridLayout* layout = qobject_cast<QGridLayout*>(dlg.layout());
    if (layout)
    {
        int rowCount = layout->rowCount();
        layout->addWidget(xsdCheckbox, rowCount, 0, 1, -1);
    }

    QString directoryPath;
    if (dlg.exec() == QDialog::Accepted)
    {
        QStringList selectedDirs = dlg.selectedFiles();
        if (!selectedDirs.isEmpty())
            directoryPath = selectedDirs.first();
    }

    QDir dir(directoryPath);
    if (directoryPath.isEmpty())
        return;

    ErrorsModel errors;
    QAbstractItemModel *ResModel = m_pLbrObj->list();

    QProgressDialog progress(this);
    progress.setWindowTitle(tr("Обработка ресурсов"));
    progress.setLabelText(tr("Обработка ресурсов"));
    progress.setMaximum(ResModel->rowCount());
    progress.show();

    XmlValidator validator;
    validator.setSchemaFileName(":/res/reslib.xsd");
    for (int i = 0; i < ResModel->rowCount(); i++ )
    {
        if (progress.wasCanceled())
            break;

        QString name = ResModel->data(ResModel->index(i, 0)).toString();
        qint16 type = ResModel->data(ResModel->index(i, 1)).toInt();

        progress.setLabelText(tr("Обработка ресурса %1 из %2")
                                  .arg(i + 1, 4)
                                  .arg(ResModel->rowCount(), 4));

        QApplication::processEvents();
        ResBuffer *buffer = nullptr;

        m_pLbrObj->getResource(name, type, &buffer);

        ResPanel panel;
        if (panel.load(buffer))
            errors.addError(tr("Ошибка загрузка ресурса <b>%1 [%2]</b>").arg(RsResCore::inst()->typeNameFromResType(type), name));
        else
        {
            const char *preffix = RsResCore::inst()->resTypePrefix(type);
            QString filename = QString("%1_%2.xml").arg(name).arg(preffix);
            QFile f(dir.absoluteFilePath(filename));
            if (f.open(QIODevice::ReadWrite))
            {
                QTextStream stream(&f);
                stream.setCodec("UTF-8");

                QString result = panel.saveXml("UTF-8");
                stream << result;

                errors.addMessage(tr("Ресурс <b>%1 [%2]</b> успешно сохранен в файл <b>%3</b>")
                                      .arg(RsResCore::inst()->typeNameFromResType(type), name, filename));

                f.seek(0);
                if (xsdCheckbox->isChecked())
                    validator.validateXmlWithXsd(&f, &errors);

                f.close();
            }
            else
                errors.addError(tr("Не удалось сохранить ресурс <b>%1 [%2]</b>").arg(RsResCore::inst()->typeNameFromResType(type), name));
        }

        delete buffer;

        progress.setValue(i + 1);
        QApplication::processEvents();
    }
}
