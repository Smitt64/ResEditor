#include "stdpaneleditor.h"
#include "baseeditorview.h"
#include "controlpropertysdlg.h"
#include "panelitem.h"
#include "resapplication.h"
#include "respanel.h"
#include "rsrescore.h"
#include "scrolitem.h"
#include "panelstructmodel.h"
#include "statusbarelement.h"
#include "basescene.h"
#include "baseeditorview.h"
#include "undoredo/undoitemadd.h"
#include "undoredo/undoitemdelete.h"
#include "bankdistribselect.h"
#include "lbrobject.h"
#include "textitem.h"
#include "controlitem.h"
#include "containeritem.h"
#include "toolsruntime.h"
#include "codeeditor/codeeditor.h"
#include "codeeditor/codehighlighter.h"
#include "widgets/resinfodlg.h"
#include "spelling/resspellstringsdlg.h"
#include "spelling/spellchecker.h"
#include <errorsmodel.h>
#include <errordlg.h>
#include <QStatusBar>
#include <QHBoxLayout>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QToolBar>
#include <QLineEdit>
#include <QFontMetrics>
#include <QPalette>
#include <QPainter>
#include <QGraphicsItem>
#include <QUndoStack>
#include <QMenuBar>
#include <QApplication>
#include <QJsonArray>
#include <QClipboard>
#include <QJsonDocument>
#include <QTextLayout>
#include <QTimer>
#include <QMimeData>
#include <QFileDialog>
#include <QComboBox>
#include <QSpacerItem>
#include <QGraphicsSceneMouseEvent>
#include <QPlainTextEdit>
#include <QMessageBox>
#include <QSettings>
#include <QVersionNumber>
#include <QTemporaryDir>
#include <QFile>
#include <QProcess>
#include <QInputDialog>
#include "propertymodel.h"
#include <QTextCodec>
#include <QProgressDialog>

#define SHADOW_CODE 9617

// ----------------------------------------------------

class StdEditorScene : public BaseScene
{
public:
    StdEditorScene(QObject *parent = nullptr) :
        BaseScene(parent),
        m_fShowCursor(false)
    {
        m_pCursorTimer = new QTimer(this);
        m_pCursorTimer->setInterval(500);
        m_pCursorTimer->setSingleShot(false);

        connect(m_pCursorTimer, &QTimer::timeout, [&]()
        {
            m_fShowCursor = !m_fShowCursor;
        });

        m_pCursorTimer->start();
    }

    const QPointF &cursorPos() const
    {
        return m_CursorPos;
    }

    virtual ~StdEditorScene()
    {
    }

    virtual void sceneItemPosChanged() Q_DECL_OVERRIDE
    {
        PanelItem *panel = findFirst<PanelItem>();

        if (panel)
            panel->updateChildControlsOrder();
    }

protected:
    virtual void drawBackground (QPainter* painter, const QRectF &rect) Q_DECL_OVERRIDE
    {
        BaseScene::drawBackground(painter, rect);

        QList<QGraphicsItem*> totalItems = items(Qt::AscendingOrder);
        for (QGraphicsItem *item : qAsConst(totalItems))
        {
            PanelItem *panel = dynamic_cast<PanelItem*>(item);

            if (panel)
            {
                QSize grSize = getGridSize();
                QRectF panelRect = panel->mapRectToScene(panel->boundingRect());
                panelRect.translate(QPointF(grSize.width() * 2, grSize.height()));
                painter->save();
                painter->setPen(Qt::NoPen);
                painter->setBrush(QBrush(Qt::Dense3Pattern));
                painter->drawRect(panelRect);
                painter->restore();
                break;
            }
        }
    }

    virtual void drawForeground(QPainter *painter, const QRectF &rect) Q_DECL_OVERRIDE
    {
        if (m_fShowCursor && !m_CursorPos.isNull())
        {
            painter->save();
            painter->setCompositionMode(QPainter::RasterOp_NotDestination);

            QPointF CursorPos = m_CursorPos;
            painter->fillRect(QRectF(CursorPos, QSizeF(getGridSize().width(), getGridSize().height()/* / 2*/)), Qt::black);
            painter->restore();
        }

        update(rect);
    }

    virtual void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE
    {
        QSize grSize = getGridSize();
        auto CheckNewPos = [this, &grSize](const QPointF &point)
        {
            const CustomRectItem *panel = findTopLevelItem();
            QRectF bound = panel->mapRectToScene(panel->boundingRect());
            QRectF caret(point, QSizeF(grSize.width(), grSize.height()));

            return bound.contains(caret);
        };

        QPointF savepos = m_CursorPos;
        if (keyEvent->key() == Qt::Key_Down)
            m_CursorPos.setY(m_CursorPos.y() + grSize.height());

        if (keyEvent->key() == Qt::Key_Up)
            m_CursorPos.setY(m_CursorPos.y() - grSize.height());

        if (keyEvent->key() == Qt::Key_Right)
            m_CursorPos.setX(m_CursorPos.x() + grSize.width());

        if (keyEvent->key() == Qt::Key_Left)
            m_CursorPos.setX(m_CursorPos.x() - grSize.width());

        if (!CheckNewPos(m_CursorPos))
            m_CursorPos = savepos;

        BaseScene::keyPressEvent(keyEvent);
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE
    {
        BaseScene::mousePressEvent(mouseEvent);
        if (mouseEvent->button() != Qt::LeftButton)
            return;

        CustomRectItem *panelItem = findTopLevelItem();
        if (!panelItem)
            return;

        QSize gridSize = getGridSize();
        QPointF scenePos = mouseEvent->scenePos();
        qreal xV = round(scenePos.x() / gridSize.width()) * gridSize.width();
        qreal yV = round(scenePos.y() / gridSize.height())* gridSize.height();

        QList<QRectF> mouseRects =
        {
            QRectF(QPoint(xV, yV - gridSize.height()), gridSize), // Up rect
            QRectF(QPoint(xV, yV + gridSize.height()), gridSize), // Bottom rect
            QRectF(QPoint(xV - gridSize.width(), yV), gridSize),  // Left rect
            QRectF(QPoint(xV + gridSize.width(), yV), gridSize)   // Right rect
        };

        QPointF nPos = QPointF(xV, yV);
        for (QRectF rect : mouseRects)
        {
            rect.adjusted(-1, -1, -1, -1);
            if (rect.contains(scenePos))
                nPos = rect.topLeft();
        }

        QRectF panelSceneBound = panelItem->mapRectToScene(panelItem->boundingRect());
        if (panelSceneBound.contains(mouseEvent->scenePos()))
        {
            bool hasBounds = false;
            QList<QGraphicsItem*> panelItems = items();

            for (QGraphicsItem *tmpItem : qAsConst(panelItems))
            {
                ScrolAreaRectItem *area = dynamic_cast<ScrolAreaRectItem*>(tmpItem);

                if (tmpItem == panelItem || area || !tmpItem->isVisible())
                    continue;

                QRectF itemSceneBound = tmpItem->mapRectToScene(tmpItem->boundingRect());
                if (itemSceneBound.contains(mouseEvent->scenePos()))
                {
                    hasBounds = true;
                    break;
                }
            }

            if (!hasBounds)
                m_CursorPos = nPos;
        }
    }

private:
    bool m_fShowCursor;
    QTimer *m_pCursorTimer;
    QPointF m_CursorPos;
};

// ----------------------------------------------------
class StdEditorView : public BaseEditorView
{
public:
    StdEditorView(QWidget *parent = nullptr) :
        BaseEditorView(parent)
    {

    }

    virtual ~StdEditorView()
    {

    }

    virtual void setupScene() Q_DECL_OVERRIDE
    {
        BaseScene *Scene = new StdEditorScene(this);
        setScene(Scene);
        Scene->setSceneRect(QRectF(0, 0, width(), height()));
    }
};

// ----------------------------------------------------

StdPanelEditor::StdPanelEditor(const qint16 &Type, QWidget *parent) :
    BaseEditorWindow(parent),
    m_pPanel(nullptr),
    panelItem(nullptr),
    m_StatusBar(nullptr)
{
    m_Type = Type;
    if (Type == LbrObject::RES_PANEL)
        panelItem = new PanelItem();
    else
    {
        panelItem = new ScrolItem();
        connect((ScrolItem*)panelItem, &ScrolItem::scrolTypeChanged, [=]() -> void
        {
            m_Type = ((ScrolItem*)panelItem)->scrolType();
            setWindowIcon(RsResCore::inst()->iconFromResType(m_Type));
        });
    }

    m_pStructModel = new PanelStructModel(panelItem);

    m_StatusBar = new QStatusBar(this);
    m_pStatusContainer = new QWidget(this);
    setWindowTitle(tr("Редактирование панели"));

    m_SizeText = new StatusBarElement(this);
    m_SizeText->setPixmap(QPixmap(":/img/Size.png"));
    m_SizeText->setText(QString("0 x 0"));

    m_CursorText = new StatusBarElement(this);
    m_CursorText->setPixmap(QPixmap(":/img/CursorPos.png"));
    m_CursorText->setText(QString("0 : 0"));

    m_pStatusContainerLayout = new QHBoxLayout();
    m_pStatusContainerLayout->setMargin(0);
    m_pStatusContainerLayout->addWidget(m_SizeText);
    m_pStatusContainerLayout->addWidget(m_CursorText);
    m_pStatusContainer->setLayout(m_pStatusContainerLayout);
    m_StatusBar->addPermanentWidget(m_pStatusContainer);
    setStatusBar(m_StatusBar);

    setMouseTracking(true);

    m_pClipboard = QApplication::clipboard();
    connect(m_pClipboard, &QClipboard::dataChanged, this, &StdPanelEditor::clipboardChanged);
}

void StdPanelEditor::setupMenus()
{
    m_pEditMenu->addAction(undoAction());
    m_pEditMenu->addAction(redoAction());
    m_pEditMenu->addSeparator();
    m_pEditMenu->addAction(m_pCutAction);
    m_pEditMenu->addAction(m_pCopyAction);
    m_pEditMenu->addAction(m_pPasteAction);
    m_pEditMenu->addSeparator();
    m_pEditMenu->addAction(m_pDelete);

    m_pResMenu->addAction(m_pSave);
    m_SaveToXml = addAction(m_pResMenu, QIcon(":/img/XMLFileHS.png"), tr("Сохранить в XML"), QKeySequence("Ctrl+ALT+S"));
    m_pCheckAction = addAction(m_pResMenu, QIcon(":/img/CheckRes.png"), tr("Проверить"), QKeySequence("Ctrl+H"));
    m_pSpellCheckAction = addAction(m_pResMenu, QIcon(":/img/CheckSpellingHS.png"), tr("Проверить орфографию"), QKeySequence("Alt+H"));
    m_EwViewAction = addAction(m_pResMenu, QIcon(":/img/Panel2.png"), tr("Просмотр в EW"), QKeySequence("Ctrl+F3"));
    m_ViewAction = addAction(m_pResMenu, QIcon(":/img/PanelCmd.png"), tr("Просмотр"), QKeySequence("Alt+F3"));
    m_Statistic = addAction(m_pResMenu, QIcon(":/img/Statistic.png"), tr("Информация"));

    m_pCreateControl = addAction(m_pElements, QIcon(":/img/insctrl.png"), tr("Создать поле"), QKeySequence(Qt::Key_Insert));

    connect(m_SaveToXml, &QAction::triggered, this, &StdPanelEditor::saveToXml);
    connect(m_pCheckAction, &QAction::triggered, this, &StdPanelEditor::onCheckRes);
    connect(m_EwViewAction, &QAction::triggered, this, &StdPanelEditor::onViewEasyWin);
    connect(m_pCreateControl, &QAction::triggered, this, &StdPanelEditor::onInsertControl);
    connect(m_pSpellCheckAction, &QAction::triggered, this, &StdPanelEditor::CheckSpelling);

    connect(m_Statistic, &QAction::triggered, [=]()
    {
        ResInfoDlg dlg(this);
        dlg.setTitle(m_pNameLineEdit->text());
        dlg.setType(QString("%1 (%2)")
                        .arg(type())
                        .arg(RsResCore::typeNameFromResType(type())));

        int ctrl = 0, txt = 0;
        QList<QGraphicsItem*> lst = panelItem->childItems();
        for (QGraphicsItem *item : qAsConst(lst))
        {
            ControlItem *control = dynamic_cast<ControlItem*>(item);
            TextItem *text = dynamic_cast<TextItem*>(item);

            if (control)
                ctrl ++;

            if (text)
                txt ++;
        }

        dlg.setFields(ctrl);
        dlg.setLabels(txt);
        dlg.exec();
    });
}

void StdPanelEditor::setupEditor()
{
    m_pMenuBar = new QMenuBar(this);
    setMenuBar(m_pMenuBar);

    m_pToolBar = addToolBar(tr("Основная"));
    m_pToolBar->setIconSize(QSize(16, 16));

    m_pView = new StdEditorView(this);
    m_pView->setupScene();

    m_TabContainer = new QTabWidget(this);
    m_TabContainer->addTab(m_pView, tr("Редактор"));
    m_TabContainer->setTabPosition(QTabWidget::South);
    m_TabContainer->setTabShape(QTabWidget::Triangular);
    m_TabContainer->setTabsClosable(true);
    m_TabContainer->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);

    setCentralWidget(m_TabContainer);

    panelItem->setBrush(QColor(128, 128, 0));

    m_pView->scene()->addItem(panelItem);

    panelItem->setCoord(QPoint(1, 1));
    panelItem->setSize(QSize(25, 20));

    m_pView->scene()->installEventFilter(this);
    m_pView->setMouseTracking(true);

    setupNameLine();
    m_pSave = addAction(QIcon(":/img/saveHS.png"), tr("Сохранить"), QKeySequence::Save);
    m_pToolBar->addSeparator();
    initUndoRedo(m_pToolBar);
    setupCopyPaste();

    m_pDelete = addAction(QIcon(":/img/Delete.png"), tr("Удалить"), QKeySequence::Delete);

    m_pEditMenu = m_pMenuBar->addMenu(tr("&Правка"));
    m_pViewMenu = m_pMenuBar->addMenu(tr("&Вид"));
    m_pResMenu = m_pMenuBar->addMenu(tr("&Ресурс"));
    m_pElements = m_pMenuBar->addMenu(tr("&Элементы"));
    setupContrastAction();
    setupScrolAreaAction();
    setupPropertyAction();

    BaseScene *baseScene = dynamic_cast<BaseScene*>(m_pView->scene());
    if (baseScene)
        initpropertyModelSignals(baseScene);

    setupMenus();
    loadToolBox();

    connect(m_pDelete, &QAction::triggered, this, &StdPanelEditor::sceneDeleteItems);
    connect(m_pSave, &QAction::triggered, this, &StdPanelEditor::onSave);
    connect(panelItem, &PanelItem::titleChanged, [=]()
    {
        emit titleChanged(panelItem->title());
    });

    connect(m_TabContainer->tabBar(), &QTabBar::tabCloseRequested, [=](int index)
    {
        QWidget *w = m_TabContainer->widget(index);
        m_TabContainer->removeTab(index);
        w ->deleteLater();
    });
}

void StdPanelEditor::setupContrastAction()
{
    ResApplication *app = (ResApplication*)qApp;

    m_pToolBar->addSeparator();

    m_pContrst = addAction(QIcon(":/img/EditBrightContrastHS.png"), tr("Контраст"), QKeySequence("Alt+F9"));
    m_pContrst->setCheckable(true);

    connect(m_pContrst, &QAction::toggled, [&](bool toogled)
    {
        panelItem->setProperty(CONTRAST_PROPERTY, toogled);
    });

    app->settings()->beginGroup("StdEditor");
    m_pContrst->setChecked(app->settings()->value("AutoContrast", true).toBool());
    app->settings()->endGroup();
    m_pViewMenu->addAction(m_pContrst);
}

void StdPanelEditor::setupScrolAreaAction()
{
    m_pContrst = addAction(QIcon(":/img/EditTableHS.png"), tr("Скролинг"), QKeySequence("Ctrl+F5"));
    m_pContrst->setCheckable(true);

    connect(m_pContrst, &QAction::toggled, [&](bool toogled)
    {
        panelItem->setProperty(SCROLAREA_PROPERTY, toogled);
        QMetaObject::invokeMethod(panelItem, "showScrolArea", Q_ARG(bool, toogled));
        //panelItem->metaObject()->invokeMethod()
    });

    m_pContrst->setChecked(false);
    m_pViewMenu->addAction(m_pContrst);
}

void StdPanelEditor::setupPropertyAction()
{
    m_pProperty = addAction(QIcon(":/img/Properties.png"), tr("Характеристики элемента"), QKeySequence::InsertParagraphSeparator);

    connect(m_pProperty, &QAction::triggered, [&]()
    {
        QKeyEvent event(QEvent::KeyPress, Qt::Key_Return, Qt::NoModifier);
        QApplication::sendEvent(m_pView, &event);
    });
}

void StdPanelEditor::setupNameLine()
{
    QFont font("TerminalVector", 10);
    font.setFixedPitch(true);

    m_pNameLineEdit = new QLineEdit(this);
    m_pNameLineEdit->setReadOnly(true);
    m_pNameLineEdit->setFont(font);
    m_pNameLineEdit->setAlignment(Qt::AlignRight);

    m_pMenuBar->setCornerWidget(m_pNameLineEdit);
}

void StdPanelEditor::setupCopyPaste()
{
    m_pToolBar->addSeparator();
    m_pCutAction = addAction(QIcon(":/img/CutHS.png"), tr("Вырезать"), QKeySequence::Cut);
    m_pCopyAction = addAction(QIcon(":/img/CopyHS.png"), tr("Копировать"), QKeySequence::Copy);
    m_pPasteAction = addAction(QIcon(":/img/PasteHS.png"), tr("Вставить"), QKeySequence::Paste);

    clipboardChanged();

    connect(m_pCopyAction, &QAction::triggered, this, &StdPanelEditor::sceneCopyItems);
    connect(m_pPasteAction, &QAction::triggered, this, &StdPanelEditor::scenePasteItems);
    connect(m_pCutAction, &QAction::triggered, this, &StdPanelEditor::sceneCutItems);
}

QAction *StdPanelEditor::addAction(const QIcon &icon, const QString &text, const QKeySequence &key)
{
    QAction *action = m_pToolBar->addAction(icon, text);
    action->setToolTip(text);

    if (!key.isEmpty())
    {
        action->setShortcut(key);
        AddShortcutToToolTip(action);
    }

    return action;
}

QAction *StdPanelEditor::addAction(QMenu *menu, const QIcon &icon, const QString &text, const QKeySequence &key)
{
    QAction *action = menu->addAction(icon, text);
    action->setToolTip(text);

    if (!key.isEmpty())
    {
        action->setShortcut(key);
        AddShortcutToToolTip(action);
    }

    return action;
}

void StdPanelEditor::setPanel(ResPanel *panel, const QString &comment)
{
    m_pPanel = panel;
    m_Type = panel->type();
    panelItem->setUndoStack(undoStack());
    panelItem->setPanel(m_pPanel, comment);

    m_pNameLineEdit->setText(m_pPanel->name());
    m_pStructModel->structChanged();

    emit titleChanged(m_pPanel->title());
}

QString StdPanelEditor::name() const
{
    return m_pNameLineEdit->text();
}

QString StdPanelEditor::title() const
{
    if (panelItem)
        return panelItem->title();

    return BaseEditorWindow::title();
}

qint16 StdPanelEditor::type() const
{
    return m_Type;
}

bool StdPanelEditor::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::GraphicsSceneMouseMove)
    {
        QSize gsz = m_pView->gridSize();
        QGraphicsSceneMouseEvent *mouse = dynamic_cast<QGraphicsSceneMouseEvent*>(event);
        QPointF local = mouse->scenePos();
        QPoint gridPos = QPoint(round(local.x() / gsz.width()), round(local.y() / gsz.height()));

        QString posText = QString("%3 : %4 (%1 : %2)")
                .arg(local.x())
                .arg(local.y())
                .arg(gridPos.x())
                .arg(gridPos.y());
        m_CursorText->setText(posText);

        updateSizeStatus();

        return false;
    }
    return QObject::eventFilter(obj, event);
}

void StdPanelEditor::sceneSelectionChanged()
{
    updateSizeStatus();
}

void StdPanelEditor::updateSizeStatus()
{
    QList<QGraphicsItem*> items = m_pView->scene()->selectedItems();

    if (items.empty() || items.size() == 1)
    {
        QRectF bound = panelItem->boundingRect();
        QPointF pos = panelItem->pos();
        QSize gsz = m_pView->gridSize();

        QString posText = QString("%1 : %2 (%3 x %4)")
                .arg(round(pos.x() / gsz.width()))
                .arg(round(pos.y() / gsz.height()))
                .arg(round(bound.width() / gsz.width()))
                .arg(round(bound.height() / gsz.height()));

        m_SizeText->setText(posText);
    }
}

void StdPanelEditor::fillItems(const QList<QGraphicsItem*> &selectedItems, QSet<CustomRectItem*> &result, const FillItemsChildMode &mode)
{
    for(auto item : qAsConst(selectedItems))
    {
        CustomRectItem *rectItem = dynamic_cast<CustomRectItem*>(item);
        if (!rectItem || !rectItem->parentItem())
            continue;

        QList<QGraphicsItem*> childItems = rectItem->childItems();

        if (mode == FICMode_ChildBeforeParent)
            fillItems(childItems, result, mode);

        result.insert(rectItem);

        if (mode == FICMode_ParentBeforeChild)
            fillItems(childItems, result, mode);
    }
}

void StdPanelEditor::sceneCutItems()
{
    sceneCopyItems();
    sceneDeleteItems();
}

void StdPanelEditor::sceneDeleteItems()
{
    BaseScene *pScene = dynamic_cast<BaseScene*>(m_pView->scene());

    if (!pScene)
        return;

    QList<QGraphicsItem*> selectedItems = pScene->selectedItems();
    QSet<CustomRectItem*> realDelete;
    fillItems(selectedItems, realDelete, FICMode_ChildBeforeParent);

    if (realDelete.empty())
        return;

    if (realDelete.size() > 1)
        undoStack()->beginMacro(tr("Удаление %1 элементов").arg(realDelete.size()));

    for (auto delItem : qAsConst(realDelete))
    {
        UndoItemDelete *cmd = new UndoItemDelete(pScene, delItem->uuid());
        undoStack()->push(cmd);
    }

    if (realDelete.size() > 1)
        undoStack()->endMacro();

    m_pStructModel->structChanged();
}

void StdPanelEditor::sceneCopyItems()
{
    QClipboard *pClipboard = QApplication::clipboard();
    BaseScene *pScene = dynamic_cast<BaseScene*>(m_pView->scene());

    if (!pScene)
        return;

    QList<QGraphicsItem*> selectedItems = pScene->selectedItems();
    QSet<CustomRectItem*> realCopy;
    fillItems(selectedItems, realCopy, FICMode_ParentBeforeChild);

    if (realCopy.empty())
        return;

    QJsonObject rootObj;
    QJsonArray items;

    for (auto item : qAsConst(realCopy))
    {
        QJsonObject itemData;
        item->serialize(itemData);
        items.append(itemData);
    }

    rootObj.insert("items", items);

    QJsonDocument doc;
    doc.setObject(rootObj);

    QByteArray json = doc.toJson();
    QMimeData *pMimeData = new QMimeData();
    pMimeData->setData(MIMETYPE_TOOLBOX, doc.toJson());

#ifdef _DEBUG
    pMimeData->setText(json);
#endif

    pClipboard->setMimeData(pMimeData);
}

void StdPanelEditor::scenePasteItems()
{
    QClipboard *pClipboard = QApplication::clipboard();
    const QMimeData *mimeData = pClipboard->mimeData();
    StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());
    CustomRectItem *topItem = pScene->findTopLevelItem();

    if (!pScene || pScene->cursorPos().isNull())
        return;

    if (mimeData->hasText() && !mimeData->hasFormat(MIMETYPE_TOOLBOX))
    {
        QTextStream stream(mimeData->text().toLocal8Bit());

        QJsonObject rootObj;
        QJsonArray items;
        QPointF offset = topItem->mapFromScene(pScene->cursorPos());
        int yOffset = 0;
        while (!stream.atEnd())
        {
            QString text = stream.readLine();

            TextItem *pTextItem = new TextItem();
            pScene->addItem(pTextItem);
            pTextItem->setText(text);
            pTextItem->setCoord(QPoint(0, yOffset));
            QJsonObject itemData;

            pTextItem->serialize(itemData);
            delete pTextItem;

            items.append(itemData);

            yOffset ++;
        }
        rootObj.insert("items", items);

        QJsonDocument doc;
        doc.setObject(rootObj);
        UndoItemAdd *pUndo = new UndoItemAdd(pScene);
        pUndo->setData(doc.toJson());
        pUndo->setOffset(topItem->realCoordToEw(offset));
        undoStack()->push(pUndo);

        pScene->update();
    }
    else if (mimeData->hasFormat(MIMETYPE_TOOLBOX))
    {
        QPointF offset = topItem->mapFromScene(pScene->cursorPos());
        UndoItemAdd *pUndo = new UndoItemAdd(pScene);
        pUndo->setData(mimeData->data(MIMETYPE_TOOLBOX));
        pUndo->setOffset(topItem->realCoordToEw(offset));
        undoStack()->push(pUndo);

        pScene->update();
    }

    m_pStructModel->structChanged();
}

void StdPanelEditor::clipboardChanged()
{
    const QMimeData *mimeData = m_pClipboard->mimeData();
    if (mimeData->hasText() || mimeData->hasFormat(MIMETYPE_TOOLBOX))
        m_pPasteAction->setEnabled(true);
    else
        m_pPasteAction->setEnabled(false);
}

void StdPanelEditor::fillResPanel(ResPanel *resPanel)
{
    BaseScene *pScene = dynamic_cast<BaseScene*>(m_pView->scene());

    QList<ControlItem*> controls = pScene->findItems<ControlItem>();
    QList<BorderItem*> borders = pScene->findItems<BorderItem>();
    QList<TextItem*> texts = pScene->findItems<TextItem>();
    PanelItem *pPanel = pScene->findFirst<PanelItem>();

    for (ControlItem *item : qAsConst(controls))
    {
        resPanel->beginAddField(item->controlName(), item->controlName2());
        resPanel->setFieldDataType(item->fieldType(),
                                   item->dataType(),
                                   item->dataLength(),
                                   item->fdm(),
                                   item->isText());
        resPanel->setFieldFlags(item->controlFlags());
        resPanel->setLenHeight(item->length(), item->lines());
        resPanel->setFieldPos(item->getPoint().x(), item->getPoint().y());
        resPanel->setFormatTooltip(item->valueTemplate(), item->toolTip());
        resPanel->setFieldStyle((quint16)item->controlStyle());
        resPanel->setFieldGroup(item->controlGroup());
        resPanel->setFieldHelp(item->helpPage());
        resPanel->endAddField();
    }

    for (BorderItem *item : qAsConst(borders))
        resPanel->addBorder(item->geometry(), item->borderStyle());

    for (TextItem *item : qAsConst(texts))
    {
        QRect rc = item->geometry();
        resPanel->addText(item->text(), rc.x(), rc.y(), item->textStyle().style());
    }

    resPanel->setType(m_Type);
    resPanel->setName(m_pNameLineEdit->text());
    resPanel->setComment(pPanel->comment());
    resPanel->setPanelStrings(pPanel->title(), pPanel->status(), pPanel->status2());
    resPanel->setPanelStyle(pPanel->borderStyle(), pPanel->panelStyle());
    resPanel->setPanelHelp(pPanel->helpPage());
    resPanel->setPanelExcludeFlags(pPanel->panelExclude());
    resPanel->setPanelCentered(pPanel->isCentered());
    resPanel->setPanelCentered(pPanel->isCentered());
    resPanel->setPanelRightText(pPanel->isRightText());
    resPanel->setPanelRect(pPanel->geometry());

    if (m_Type != LbrObject::RES_PANEL)
    {
        resPanel->setScrolFlags(pPanel->property("scrolFlags").toInt());

        quint16 rowNum = pPanel->property("rowNum").value<quint16>();
        quint16 rowLength = pPanel->property("rowLength").value<quint16>();
        QRect scrol(QPoint(pPanel->property("scrolPos").toPoint()),
                    QSize(rowLength, rowNum));

        resPanel->setScrol(scrol);
        resPanel->setRowHeight(pPanel->property("rowHeight").value<quint16>());
    }
}

bool StdPanelEditor::save(ResBuffer *res, QString *error)
{
    ResPanel resPanel;
    fillResPanel(&resPanel);

    bool hr = !resPanel.save(res);
    if (hr)
        hr = BaseEditorWindow::save(res, error);

    return hr;
}

void StdPanelEditor::addCodeWindow(const QString &title, const QString &text)
{
    CodeEditor *pEdit = new CodeEditor(this);
    pEdit->setReadOnly(true);
    pEdit->setPlainText(text);

    ToolApplyHighlighter(pEdit, HighlighterXml);
    m_TabContainer->addTab(pEdit, title);
}

const char *StdPanelEditor::resTypeStr(int tp)
{
    switch(tp)
    {
    case LbrObject::RES_PANEL:   return "P";
    case LbrObject::RES_SCROL:   return "S";
    case LbrObject::RES_LS:      return "L";
    case LbrObject::RES_BS:      return "B";
    case LbrObject::RES_REP:     return "R";
    case LbrObject::RES_MENU2:   return "M";
    }

    return "N";
}

void StdPanelEditor::saveToXml()
{
    QFileDialog fileDlg(this);
    fileDlg.setOption(QFileDialog::DontUseNativeDialog);
    fileDlg.setWindowTitle(tr("Сохранение в xml"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setDirectory("/home/jana");
    fileDlg.setFileMode(QFileDialog::DirectoryOnly);
    fileDlg.setViewMode(QFileDialog::List);
    //fileDlg.setNameFilter(tr("Image Files (*.png *.jpg *.bmp)"));

    QScopedPointer<QHBoxLayout> hbl(new QHBoxLayout(0));
    QScopedPointer<QComboBox> encode(new QComboBox());
    QGridLayout* mainLayout = dynamic_cast <QGridLayout*>(fileDlg.layout());
    hbl->addWidget(encode.data());
    mainLayout->addLayout(hbl.data(), mainLayout->rowCount(), 1, 1, -1);

    encode->addItems({"CP866", "WINDOWS-1251", "UTF-8", "UTF-16LE", "UTF-16BE"});
    encode->setCurrentText("UTF-8");

    if (fileDlg.exec() != QDialog::Accepted)
        return;

    QString filename = fileDlg.selectedFiles().first();

    if (filename.isEmpty())
        return;

    QString encodetxt = encode->currentText();
    QString result =
        RsResCore::inst()->saveResToXml(type(), name(), lbr(), filename, encodetxt);

    addCodeWindow(tr("XML"), result);
}

void StdPanelEditor::showCheckError(int stat, ErrorsModel *model)
{
    if (stat)
    {
        if (!model)
        {
            QMessageBox::critical(this, tr("Ошибка проверки ресурса"),
                                  ResPanel::GetCheckError(stat));
        }
        else
        {
            ErrorDlg dlg(ErrorDlg::ModeInformation, this);
            dlg.setMessage(tr("Ошибка проверки ресурса"));
            dlg.setErrors(model);
            dlg.exec();
        }
    }
    else
    {
        QMessageBox::information(this, tr("Результат проверки ресурса"),
                                 ResPanel::GetCheckError(stat));
    }
}

void StdPanelEditor::onCheckRes()
{
    ResPanel panel;
    fillResPanel(&panel);

    ErrorsModel model;
    int stat = panel.checkResource(&model);
    showCheckError(stat, &model);
}

void StdPanelEditor::onSave()
{
    ResPanel panel;
    fillResPanel(&panel);

    ErrorsModel model;
    int stat = panel.checkResource(&model);

    if (!stat)
        emit readySave();
    else
        showCheckError(stat, &model);
}

QAbstractItemModel *StdPanelEditor::propertyModel()
{
    BaseScene *pScene = qobject_cast<BaseScene*>(m_pView->scene());

    if (!pScene)
        return nullptr;

    CustomRectItem *rectItem = pScene->firstSelected<CustomRectItem>();

    if (!rectItem)
        return nullptr;

    return rectItem->propertyModel();
}

QAbstractItemModel *StdPanelEditor::structModel()
{
    return m_pStructModel;
}

void StdPanelEditor::ViewResource(bool EwFlag)
{
    BankDistribSelect dlg(this);
    if (dlg.exec() == QDialog::Accepted)
    {
        QString selected = dlg.path();
        m_ViewerDir.reset(new QTemporaryDir());

        QDir dir(m_ViewerDir->path());
        QString fileName = dir.absoluteFilePath("viewresi.exe");
        QFile viewresi(fileName);
        QFile source(":/tools/viewresi.exe");

        if (viewresi.open(QIODevice::WriteOnly) && source.open(QIODevice::ReadOnly))
            viewresi.write(source.readAll());

        viewresi.close();
        source.close();

        QDir BankDir(selected);

        QString typeparam;
        switch(type())
        {
        case LbrObject::RES_PANEL:
            typeparam = "panel";
            break;

        case LbrObject::RES_SCROL:
            typeparam = "scrol";
            break;

        case LbrObject::RES_BS:
            typeparam = "bscrol";
            break;

        case LbrObject::RES_LS:
            typeparam = "lscrol";
            break;
        }

        QStringList params;

        if (EwFlag)
            params.append("/w");

        params << QDir::toNativeSeparators(lbr()->fileName())
               << name()
               << typeparam;

        QProcess *process = new QProcess();
        connect(process, qOverload<int, QProcess::ExitStatus>(&QProcess::finished), this, &StdPanelEditor::ViewerFinished);
        process->setWorkingDirectory(selected);
        process->start(fileName, params);
        process->waitForStarted();
    }
}

void StdPanelEditor::onViewEasyWin()
{
    ViewResource(true);
}

void StdPanelEditor::onViewCmd()
{
    ViewResource(false);
}

void StdPanelEditor::ViewerFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    m_ViewerDir.reset();
    sender()->deleteLater();
}

void StdPanelEditor::onInsertControl()
{
    StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());

    if (!pScene)
        return;

    QPointF CursorPos = pScene->cursorPos();
    if (CursorPos.isNull())
        return;

    CustomRectItem *pPanel = pScene->findTopLevelItem();
    if (!pPanel)
        return;

    CursorPos = pPanel->mapFromScene(CursorPos);

    bool found = false;
    QSize grid = pScene->getGridSize();
    QRectF cursor(CursorPos, QSizeF(grid.width(), grid.height()));
    QList<CustomRectItem*> lst = pScene->findItems<CustomRectItem>();
    for (CustomRectItem *item : lst)
    {
        if (pPanel == item)
            continue;

        QRectF rect = item->boundingRect();
        rect.moveTo(item->pos());

        if (rect.contains(cursor))
        {
            found = true;
            break;
        }
    }

    if (!found)
    {
        undoStack()->beginMacro(tr("Изменение параметров элемента"));
        QList<QGraphicsItem*> Created;
        UndoItemAdd *pUndo = new UndoItemAdd(pScene);
        pUndo->setData(toolReadTextFileContent(":/json/InsertControlData.json").toLocal8Bit());
        pUndo->setOffset(pPanel->realCoordToEw(CursorPos));
        pUndo->setItemsListPtr(&Created);
        undoStack()->push(pUndo);

        ControlPropertysDlg dlg(this);
        dlg.setControlItem(dynamic_cast<ControlItem*>(Created[0]));

        if (dlg.exec() == QDialog::Accepted)
        {
            pUndo->setItemsListPtr(nullptr);
            dynamic_cast<ControlItem*>(Created[0])->FillItemControl(dlg);
            undoStack()->endMacro();
        }
        else
        {
            auto* cmd = const_cast<QUndoCommand*>(undoStack()->command(undoStack()->count() - 1));
            cmd->undo();
            cmd->setObsolete(true);
            undoStack()->endMacro();
            undoStack()->undo();
        }
        pScene->update();
    }
}

void StdPanelEditor::CheckSpelling()
{
    SpellChecker *spell = nullptr;

    QProgressDialog dialog(tr("Загрузка словаря"), "", 0, 0, this);
    dialog.setWindowTitle("Орфография");
    dialog.setWindowIcon(QIcon(":/img/CheckSpellingHS.png"));
    dialog.setCancelButton(nullptr);
    dialog.setWindowModality(Qt::WindowModal);
    dialog.setAutoClose(true);

    dialog.open();
    QCoreApplication::processEvents();
    spellGetCheckerForLanguage("ru", &spell);
    dialog.close();

    if (!spell)
    {
        QMessageBox::warning(this, tr("Проверика орфографии"),
                             tr("Не удалось загрузить словарь"));
        return;
    }

    QList<CheckUserDataTuple> userData;

    ResSpellStringsDlg dlg(spell, this);
    if (!panelItem->title().isEmpty())
    {
        userData.append({CheckModePanel | CheckModeTitle, panelItem});
        dlg.appendString(panelItem->title(), &userData.back());
    }

    if (!panelItem->status().isEmpty())
    {
        userData.append({CheckModePanel | CheckModeStatus, panelItem});
        dlg.appendString(panelItem->status(), &userData.back());
    }

    if (!panelItem->status2().isEmpty())
    {
        userData.append({CheckModePanel | CheckModeStatus2, panelItem});
        dlg.appendString(panelItem->status2(), &userData.back());
    }

    QList<QGraphicsItem*> elements = panelItem->childItems();
    for (QGraphicsItem *item : qAsConst(elements))
    {
        TextItem *label = dynamic_cast<TextItem*>(item);
        ControlItem *control = dynamic_cast<ControlItem*>(item);

        if (label && !label->text().isEmpty())
        {
            userData.append({CheckModeLabel, label});
            dlg.appendString(label->text(), &userData.back());
        }

        if (control)
        {
            if (!control->controlName().isEmpty())
            {
                userData.append({CheckModeLabel | CheckModeControlName, control});
                dlg.appendString(control->controlName(), &userData.back());
            }

            if (!control->toolTip().isEmpty())
            {
                userData.append({CheckModeLabel | CheckModeToolTip, control});
                dlg.appendString(control->toolTip(), &userData.back());
            }
        }
    }

    if (dlg.count())
    {
        if (dlg.exec() == QDialog::Accepted)
            CheckSpellingUpdateTexts(&dlg);
    }
    else
    {
        QMessageBox::information(&dialog, tr("Проверика орфографии"),
                             tr("Ошибок не обнаружено"));
    }
}

void StdPanelEditor::CheckSpellingUpdateTexts(ResSpellStringsDlg *dlg)
{
    int count = dlg->count();
    undoStack()->beginMacro(tr("Проверика орфографии"));
    for (int i = 0; i < count; i++)
    {
        QString str = dlg->value(i);
        CheckUserDataTuple *userData = (CheckUserDataTuple*)dlg->userData(i);

        int mode = std::get<0>(*userData);
        CustomRectItem *rectItem = std::get<1>(*userData);

        if (mode & CheckModePanel)
        {
            PanelItem *item = qobject_cast<PanelItem*>(rectItem);

            if (mode & CheckModeTitle)
                item->setTitle(str);

            if (mode & CheckModeStatus)
                item->setStatus(str);

            if (mode & CheckModeStatus2)
                item->setStatus2(str);
        }

        if (mode & CheckModeLabel)
        {
            TextItem *item = qobject_cast<TextItem*>(rectItem);
            item->setText(str);
        }

        if (mode & CheckModeControl)
        {
            ControlItem *item = qobject_cast<ControlItem*>(rectItem);

            if (mode & CheckModeControlName)
                item->setControlName(str);

            if (mode & CheckModeToolTip)
                item->setToolTip(str);
        }
    }
    undoStack()->endMacro();
}
