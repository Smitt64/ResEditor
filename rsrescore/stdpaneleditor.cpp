#include "stdpaneleditor.h"
#include "baseeditorview.h"
#include "panelitem.h"
#include "respanel.h"
#include "rsrescore.h"
#include "statusbarelement.h"
#include "basescene.h"
#include "baseeditorview.h"
#include "undoredo/undoitemadd.h"
#include "undoredo/undoitemdelete.h"
#include "textitem.h"
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
#include <QGraphicsSceneMouseEvent>

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
        m_pCursorTimer->setInterval(1000);
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

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE
    {
        BaseScene::mousePressEvent(mouseEvent);
        if (mouseEvent->button() != Qt::LeftButton)
            return;

        CustomRectItem *panelItem = findTopLevelItem();
        if (!panelItem)
            return;

        QSize gridSize = getGridSize();
        qreal xV = round(mouseEvent->scenePos().x() / gridSize.width()) * gridSize.width();
        qreal yV = round(mouseEvent->scenePos().y() / gridSize.height())* gridSize.height();

        QPointF nPos = QPointF(xV, yV);
        QRectF panelSceneBound = panelItem->mapRectToScene(panelItem->boundingRect());

        if (panelSceneBound.contains(mouseEvent->scenePos()))
        {
            bool hasBounds = false;
            QList<QGraphicsItem*> panelItems = items();

            for (QGraphicsItem *tmpItem : qAsConst(panelItems))
            {
                if (tmpItem == panelItem)
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

    m_pClipboard = QApplication::clipboard();
    connect(m_pClipboard, &QClipboard::dataChanged, this, &StdPanelEditor::clipboardChanged);
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

    m_pDelete = addAction(QIcon(":/img/Delete.png"), tr("Удалить"), QKeySequence::Delete);

    setupContrastAction();
    setupPropertyAction();

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

    m_pContrst = addAction(QIcon(":/img/EditBrightContrastHS.png"), tr("Контраст"));
    m_pContrst->setCheckable(true);

    connect(m_pContrst, &QAction::toggled, [&](bool toogled)
    {
        panelItem->setProperty(CONTRAST_PROPERTY, toogled);
    });

    m_pContrst->setChecked(true);
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

void StdPanelEditor::setPanel(ResPanel *panel, const QString &comment)
{
    m_pPanel = panel;
    panelItem->setUndoStack(undoStack());
    panelItem->setPanel(m_pPanel, comment);

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

    QMimeData *pMimeData = new QMimeData();
    pMimeData->setData(MIMETYPE_TOOLBOX, doc.toJson());
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

    if (mimeData->hasText())
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
}

void StdPanelEditor::clipboardChanged()
{
    const QMimeData *mimeData = m_pClipboard->mimeData();
    if (mimeData->hasText() || mimeData->hasFormat(MIMETYPE_TOOLBOX))
        m_pPasteAction->setEnabled(true);
    else
        m_pPasteAction->setEnabled(false);
}
