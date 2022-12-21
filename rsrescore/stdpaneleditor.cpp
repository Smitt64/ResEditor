#include "stdpaneleditor.h"
#include "baseeditorview.h"
#include "panelitem.h"
#include "respanel.h"
#include "statusbarelement.h"
#include "basescene.h"
#include "baseeditorview.h"
#include "undoredo/undoitemdelete.h"
#include "toolbox/toolboxmodel.h"
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

#define SHADOW_CODE 9617

// ----------------------------------------------------

class StdEditorScene : public BaseScene
{
public:
    StdEditorScene(QObject *parent = nullptr) :
        BaseScene(parent)
    {

    }

    virtual ~StdEditorScene() {}

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

StdPanelEditor::StdPanelEditor(QWidget *parent) :
    BaseEditorWindow(parent),
    panelItem(nullptr),
    m_StatusBar(nullptr)
{
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
}

void StdPanelEditor::setupMenus()
{
    m_pEditMenu = m_pMenuBar->addMenu(tr("Правка"));
    m_pEditMenu->addAction(undoAction());
    m_pEditMenu->addAction(redoAction());
    m_pEditMenu->addSeparator();
    m_pEditMenu->addAction(m_pCutAction);
    m_pEditMenu->addAction(m_pCopyAction);
    m_pEditMenu->addAction(m_pPasteAction);
}

void StdPanelEditor::setupEditor()
{
    m_pMenuBar = new QMenuBar(this);
    setMenuBar(m_pMenuBar);

    m_pToolBar = addToolBar(tr("Основная"));
    m_pToolBar->setIconSize(QSize(16, 16));

    m_pView = new StdEditorView(this);
    m_pView->setupScene();
    setCentralWidget(m_pView);

    panelItem = new PanelItem();
    panelItem->setBrush(QColor(128, 128, 0));

    m_pView->scene()->addItem(panelItem);

    panelItem->setCoord(QPoint(1, 1));
    panelItem->setSize(QSize(25, 20));

    m_pView->scene()->installEventFilter(this);
    m_pView->setMouseTracking(true);

    setupNameLine();
    m_pToolBar->addSeparator();
    initUndoRedo(m_pToolBar);
    setupCopyPaste();
    setupContrastAction();

    m_pDelete = m_pToolBar->addAction(QIcon(":/img/Delete.png"), tr("Удалить"));
    m_pDelete->setShortcut(QKeySequence::Delete);

    BaseScene *baseScene = dynamic_cast<BaseScene*>(m_pView->scene());
    if (baseScene)
        initpropertyModelSignals(baseScene);

    setupMenus();
    loadToolBox();

    connect(m_pDelete, &QAction::triggered, this, &StdPanelEditor::sceneDeleteItems);
}

void StdPanelEditor::setupContrastAction()
{
    m_pToolBar->addSeparator();

    m_pContrst = m_pToolBar->addAction(QIcon(":/img/EditBrightContrastHS.png"), tr("Контраст"));
    m_pContrst->setCheckable(true);

    connect(m_pContrst, &QAction::toggled, [this](bool toogled)
    {
        panelItem->setProperty(CONTRAST_PROPERTY, toogled);
    });
    m_pContrst->setChecked(true);
}

void StdPanelEditor::setupNameLine()
{
    QFont font("TerminalVector", 10);
    font.setFixedPitch(true);

    QFontMetrics metrics(font);
    int namewidth = metrics.width(QChar::Space) * 9;

    m_pNameLineEdit = new QLineEdit(this);
    m_pNameLineEdit->setReadOnly(true);
    m_pNameLineEdit->setFont(font);
    m_pNameLineEdit->setMaximumWidth(namewidth);
    m_pToolBar->addWidget(m_pNameLineEdit);
}

void StdPanelEditor::setupCopyPaste()
{
    m_pToolBar->addSeparator();
    m_pCutAction = m_pToolBar->addAction(QIcon(":/img/CutHS.png"), tr("Вырезать"));
    m_pCutAction->setShortcut(QKeySequence::Cut);

    m_pCopyAction = m_pToolBar->addAction(QIcon(":/img/CopyHS.png"), tr("Копировать"));
    m_pCopyAction->setShortcut(QKeySequence::Copy);

    m_pPasteAction = m_pToolBar->addAction(QIcon(":/img/PasteHS.png"), tr("Вставить"));
    m_pPasteAction->setShortcut(QKeySequence::Paste);

    connect(m_pCopyAction, &QAction::triggered, this, &StdPanelEditor::sceneCopyItems);
}

void StdPanelEditor::setPanel(ResPanel *panel)
{
    m_pPanel = panel;
    panelItem->setUndoStack(undoStack());
    panelItem->setPanel(m_pPanel);

    m_pNameLineEdit->setText(m_pPanel->name());
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

    pClipboard->setText(doc.toJson());
}
