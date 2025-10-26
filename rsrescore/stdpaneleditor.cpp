#include "stdpaneleditor.h"
#include "baseeditorview.h"
#include "controlitemswrapper.h"
#include "controlpropertysdlg.h"
#include "panelitem.h"
#include "qmetaobject.h"
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
#include "SARibbon.h"
#include "propertywidgetmapper.h"
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
#include <QActionGroup>

#define SHADOW_CODE 9617

// ----------------------------------------------------

class StdEditorScene : public BaseScene
{
public:
    StdEditorScene(QObject *parent = nullptr) :
        BaseScene(parent),
        m_fShowCursor(false)
    {
        QSize grSize = getGridSize();

        m_pCursorTimer = new QTimer(this);
        m_pCursorTimer->setInterval(500);
        m_pCursorTimer->setSingleShot(false);
        //m_CursorPos = QPointF(grSize.width(), grSize.height());

        connect(m_pCursorTimer, &QTimer::timeout, [&]()
        {
            m_fShowCursor = !m_fShowCursor;
        });

        m_controlItemsWrapper = new ControlItemsWrapper(this);
        m_pCursorTimer->start();
    }

    ControlItemsWrapper *controlItemsWrapper()
    {
        return m_controlItemsWrapper;
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

    void setCursorPosition(const QPointF &scenePos)
    {
        PanelItem* panel = findFirst<PanelItem>();
        if (!panel)
            return;

        QPointF localPos = panel->mapFromScene(scenePos);
        QSize gridSize = getGridSize();

        qreal xV = floor(localPos.x() / gridSize.width()) * gridSize.width();
        qreal yV = floor(localPos.y() / gridSize.height()) * gridSize.height();

        m_CursorPos = QPointF(xV, yV);
        update();
    }

protected:
    virtual void handleSelectionChanged(const QList<QGraphicsItem*> &selectedItems) Q_DECL_OVERRIDE
    {
        QVector<ControlItem*> controlItems;

        for (QGraphicsItem* item : selectedItems)
        {
            ControlItem* controlItem = dynamic_cast<ControlItem*>(item);
            if (controlItem)
                controlItems.append(controlItem);
        }

        if (!controlItems.isEmpty())
        {
            // Если выделено несколько ControlItem, создаем или обновляем обертку
            if (!m_controlItemsWrapper->undoStack())
                m_controlItemsWrapper->setUndoStack(controlItems.first()->undoStack());

            m_controlItemsWrapper->clearControlItems();
            m_controlItemsWrapper->addControlItems(controlItems);
            m_controlItemsWrapper->emitAll();
            emit propertyModelChanged(m_controlItemsWrapper->propertyModel());
        }
        else
            BaseScene::handleSelectionChanged(selectedItems);
    }

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

            PanelItem* panel = findFirst<PanelItem>();
            QPointF sceneCursorPos = m_CursorPos;
            if (panel)
                sceneCursorPos = panel->mapToScene(m_CursorPos);

            painter->fillRect(QRectF(sceneCursorPos, QSizeF(getGridSize().width(), getGridSize().height())), Qt::black);
            painter->restore();
        }

        update(rect);
    }

    virtual void keyPressEvent(QKeyEvent *keyEvent) Q_DECL_OVERRIDE
    {
        PanelItem* panel = findFirst<PanelItem>();
        if (!panel)
        {
            BaseScene::keyPressEvent(keyEvent);
            return;
        }

        QSize grSize = getGridSize();
        QPointF localCursorPos = m_CursorPos;
        QPointF savepos = localCursorPos;

        if (keyEvent->key() == Qt::Key_Down)
            localCursorPos.setY(localCursorPos.y() + grSize.height());

        if (keyEvent->key() == Qt::Key_Up)
            localCursorPos.setY(localCursorPos.y() - grSize.height());

        if (keyEvent->key() == Qt::Key_Right)
            localCursorPos.setX(localCursorPos.x() + grSize.width());

        if (keyEvent->key() == Qt::Key_Left)
            localCursorPos.setX(localCursorPos.x() - grSize.width());

        QRectF panelRect = panel->boundingRect();
        QRectF availableRect = panelRect;

        if (panel->borderStyle() != ResStyle::Border_NoLine)
        {
            availableRect = panelRect.adjusted(grSize.width(), grSize.height(),
                                               -grSize.width(), -grSize.height());
        }

        QRectF cursorRect(localCursorPos, QSizeF(grSize.width(), grSize.height()));

        if (availableRect.contains(cursorRect))
            m_CursorPos = localCursorPos;
        else
            m_CursorPos = savepos;

        BaseScene::keyPressEvent(keyEvent);
    }

    virtual void mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent) Q_DECL_OVERRIDE
    {
        BaseScene::mousePressEvent(mouseEvent);

        if (mouseEvent->button() != Qt::LeftButton)
            return;

        PanelItem* panelItem = findFirst<PanelItem>();
        if (!panelItem)
            return;

        QSize gridSize = getGridSize();
        QPointF scenePos = mouseEvent->scenePos();
        QPointF localPos = panelItem->mapFromScene(scenePos);

        qreal xV = floor(localPos.x() / gridSize.width()) * gridSize.width();
        qreal yV = floor(localPos.y() / gridSize.height()) * gridSize.height();
        QPointF gridPos(xV, yV);

        QRectF cellRect(gridPos, gridSize);
        if (!cellRect.contains(localPos))
            return;

        QRectF panelBound = panelItem->boundingRect();
        QRectF availableBound = panelBound;

        if (panelItem->borderStyle() != ResStyle::Border_NoLine)
        {
            availableBound = panelBound.adjusted(gridSize.width(), gridSize.height(),
                                                 -gridSize.width(), -gridSize.height());
        }

        if (!availableBound.contains(cellRect))
            return;

        bool positionBlocked = false;
        QList<QGraphicsItem*> itemsAtPos = items(scenePos);

        for (QGraphicsItem* item : qAsConst(itemsAtPos))
        {
            if (item == panelItem || dynamic_cast<ScrolAreaRectItem*>(item) || !item->isVisible())
                continue;

            QRectF itemRect = item->mapRectToScene(item->boundingRect());
            QRectF sceneCellRect = panelItem->mapToScene(cellRect).boundingRect();
            if (itemRect.intersects(sceneCellRect))
            {
                positionBlocked = true;
                break;
            }
        }

        if (!positionBlocked)
        {
            m_CursorPos = gridPos; // Сохраняем в локальных координатах panelItem
            update();
        }
    }

private:
    bool m_fShowCursor;
    QTimer *m_pCursorTimer;
    QPointF m_CursorPos;
    ControlItemsWrapper *m_controlItemsWrapper;
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
    m_StatusBar(nullptr),
    m_pPanelCategory(nullptr)
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

StdPanelEditor::~StdPanelEditor()
{
    delete m_pPanelCategory;
    delete m_pControlCategory;

    m_pPanelCategory = nullptr;
    m_pControlCategory = nullptr;
}

void StdPanelEditor::setupEditor()
{
    /*m_pToolBar = addToolBar(tr("Основная"));
    m_pToolBar->setIconSize(QSize(16, 16));*/

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
    initUndoRedo();

    BaseScene *baseScene = dynamic_cast<BaseScene*>(m_pView->scene());
    if (baseScene)
        initpropertyModelSignals(baseScene);

    loadToolBox();

    //connect(m_pDelete, &QAction::triggered, this, &StdPanelEditor::sceneDeleteItems);
    //connect(m_pSave, &QAction::triggered, this, &StdPanelEditor::onSave);
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

void StdPanelEditor::setupNameLine()
{
    QFont font("TerminalVector", 10);
    font.setFixedPitch(true);

    m_pNameLineEdit = new QLineEdit(this);
    m_pNameLineEdit->setReadOnly(true);
    m_pNameLineEdit->setFont(font);
    m_pNameLineEdit->setObjectName("pNameLineEdit");

    QAction *copyAction = m_pNameLineEdit->addAction(QIcon::fromTheme("Copy"), QLineEdit::TrailingPosition);
    connect(copyAction, &QAction::triggered, [=]()
    {
        QClipboard *pClipboard = QApplication::clipboard();
        pClipboard->setText(m_pNameLineEdit->text(), QClipboard::Clipboard);
    });
}

/*QAction *StdPanelEditor::addAction(const QIcon &icon, const QString &text, const QKeySequence &key)
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
}*/

void StdPanelEditor::setCursorToFirstFreeCell()
{
    StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());
    if (!pScene || !panelItem)
        return;

    QSize gridSize = pScene->getGridSize();
    QRectF panelRect = panelItem->boundingRect();

    // Определяем доступную область в зависимости от наличия границы
    QRectF availableRect = panelRect;
    if (panelItem->borderStyle() != ResStyle::Border_NoLine)
    {
        availableRect = panelRect.adjusted(gridSize.width(), gridSize.height(),
                                           -gridSize.width(), -gridSize.height());
    }

    // Получаем все дочерние элементы панели
    QList<QGraphicsItem*> childItems = panelItem->childItems();

    // Вместо QSet<QRectF> используем QList для хранения занятых ячеек
    QList<QRectF> occupiedCells;

    // Собираем занятые ячейки
    for (QGraphicsItem* item : childItems)
    {
        if (dynamic_cast<ScrolAreaRectItem*>(item) || !item->isVisible())
            continue;

        CustomRectItem* rectItem = dynamic_cast<CustomRectItem*>(item);
        if (rectItem)
        {
            QRectF itemRect = rectItem->boundingRect();
            itemRect.moveTo(rectItem->pos());

            // Преобразуем в координаты сетки
            qreal startX = floor(itemRect.x() / gridSize.width()) * gridSize.width();
            qreal startY = floor(itemRect.y() / gridSize.height()) * gridSize.height();
            qreal endX = ceil((itemRect.x() + itemRect.width()) / gridSize.width()) * gridSize.width();
            qreal endY = ceil((itemRect.y() + itemRect.height()) / gridSize.height()) * gridSize.height();

            // Добавляем все занятые ячейки
            for (qreal y = startY; y < endY; y += gridSize.height())
            {
                for (qreal x = startX; x < endX; x += gridSize.width())
                    occupiedCells.append(QRectF(x, y, gridSize.width(), gridSize.height()));
            }
        }
    }

    // Ищем первую свободную ячейку
    for (qreal y = availableRect.y(); y < availableRect.y() + availableRect.height(); y += gridSize.height())
    {
        for (qreal x = availableRect.x(); x < availableRect.x() + availableRect.width(); x += gridSize.width())
        {
            QRectF cellRect(x, y, gridSize.width(), gridSize.height());

            // Проверяем, что ячейка полностью внутри доступной области
            if (!availableRect.contains(cellRect))
                continue;

            // Проверяем, что ячейка не занята
            bool isOccupied = false;
            for (const QRectF& occupiedCell : occupiedCells)
            {
                if (occupiedCell.intersects(cellRect))
                {
                    isOccupied = true;
                    break;
                }
            }

            if (!isOccupied)
            {
                // Нашли свободную ячейку, устанавливаем курсор
                pScene->setCursorPosition(panelItem->mapToScene(QPointF(x, y)));
                return;
            }
        }
    }

    // Если не нашли свободную ячейку, устанавливаем курсор в начало доступной области
    pScene->setCursorPosition(panelItem->mapToScene(availableRect.topLeft()));
}

void StdPanelEditor::setPanel(ResPanel *panel, const QString &comment)
{
    m_pPanel = panel;
    m_Type = panel->type();
    panelItem->setUndoStack(undoStack());
    panelItem->setPanel(m_pPanel, comment);

    m_pNameLineEdit->setText(m_pPanel->name());
    m_pStructModel->structChanged();

    setCursorToFirstFreeCell();

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
        QPointF offset = pScene->cursorPos();//topItem->mapFromScene(pScene->cursorPos());
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
        QPointF offset = pScene->cursorPos();//topItem->mapFromScene(pScene->cursorPos());
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

    //CursorPos = pPanel->mapFromScene(CursorPos);

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

void StdPanelEditor::MakeControlRibbonCategory(SARibbonCategory* category)
{
    //ResApplication *app = (ResApplication*)qApp;
    StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());

    SARibbonPannel *fieldpanel = category->addPannel(tr("Параметры поля"));
    SARibbonPannel *fieldtypepanel = category->addPannel(tr("Тип поля"));
    SARibbonPannel *datatypepanel = category->addPannel(tr("Тип значения"));

    m_pFieldProperty = createAction(tr("Параметры поля"), "FieldProperties");
    fieldpanel->addLargeAction(m_pFieldProperty);

    m_pFdmAction = createAction(tr("Признак FDM"), "TimeLineLock");
    m_pFdmAction->setCheckable(true);
    fieldpanel->addMediumAction(m_pFdmAction);

    m_pAsTextAction = createAction(tr("Признак текста"), "TextBlock");
    m_pAsTextAction->setCheckable(true);
    fieldpanel->addMediumAction(m_pAsTextAction);

    m_pFieledTypeGroup = new QActionGroup(this);
    m_pFieledTypeGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    m_pDataTypeGroup = new QActionGroup(this);
    m_pDataTypeGroup->setExclusionPolicy(QActionGroup::ExclusionPolicy::Exclusive);

    QMetaEnum FieldTypeEnum = QMetaEnum::fromType<ControlItem::FieldType>();
    for (int i = 0; i < FieldTypeEnum.keyCount(); i++)
    {
        int value = FieldTypeEnum.value(i);
        QAction *ftype = createAction(FieldTypeEnum.valueToKey(value));
        ftype->setCheckable(true);
        ftype->setData(value);

        fieldtypepanel->addSmallAction(ftype);
        m_pFieledTypeGroup->addAction(ftype);
    }

    QMetaEnum DataTypeEnum = QMetaEnum::fromType<ControlItem::DataType>();
    for (int i = 0; i < DataTypeEnum.keyCount(); i++)
    {
        int value = DataTypeEnum.value(i);
        QAction *ftype = createAction(DataTypeEnum.valueToKey(value));
        ftype->setCheckable(true);
        ftype->setData(value);

        datatypepanel->addSmallAction(ftype);
        m_pDataTypeGroup->addAction(ftype);
    }

    SARibbonPannel *stylepanel = category->addPannel(tr("Стиль"));
    m_pControlStyleGallery = stylepanel->addGallery();
    MakeStyleRaibbonGallary(m_pControlStyleGallery, 0, true);

    m_pNoTabStop = createAction(tr("Признак FDM"), "TimeLineLock");
    m_pNoTabStop->setCheckable(true);
    fieldpanel->addMediumAction(m_pFdmAction);

    ControlItemsWrapper *wrp = pScene->controlItemsWrapper();
    m_RibbonControlMapper.reset(new PropertyWidgetMapper());
    m_RibbonMapper->bind(wrp, "fieldType", m_pFieledTypeGroup);
    m_RibbonMapper->bind(wrp, "dataType", m_pDataTypeGroup);

    m_RibbonMapper->bind(wrp, "fdm", m_pFdmAction);
    m_RibbonMapper->bind(wrp, "isText", m_pAsTextAction);
}

void StdPanelEditor::MakeResRibbonCategory(SARibbonCategory* category)
{
    ResApplication *app = (ResApplication*)qApp;
    SARibbonPannel *respanel = category->addPannel(tr("Панель"));
    //respanel->setObjectName(respanel->titleLabel()->text());

    QAction *panelPropertyAction = createAction(tr("Параметры панели"), "WindowsService");
    respanel->addLargeAction(panelPropertyAction);
    respanel->addSeparator();

    m_SaveToXml = createAction(tr("Сохранить в XML"), "XMLFile", QKeySequence("Ctrl+ALT+S"));
    respanel->addLargeAction(m_SaveToXml);
    connect(m_SaveToXml, &QAction::triggered, this, &StdPanelEditor::saveToXml);
    respanel->addSeparator();

    connect(panelPropertyAction, &QAction::triggered, [&]()
    {
        panelItem->userAction(CustomRectItem::ActionKeyEnter);
    });

    respanel->addSmallWidget(m_pNameLineEdit);
    //respanel->setMaximumWidth(48 * 4);
    SARibbonPannel *editpanel = category->addPannel(tr("Правка"));

    m_pCreateControl = createAction(tr("Создать поле"), "TextBox", QKeySequence(Qt::Key_Insert));
    connect(m_pCreateControl, &QAction::triggered, this, &StdPanelEditor::onInsertControl);
    editpanel->addLargeAction(m_pCreateControl);

    m_pDelete = createAction(tr("Удалить элемент"), "DeleteClause", QKeySequence::Delete);
    connect(m_pDelete, &QAction::triggered, this, &StdPanelEditor::sceneDeleteItems);
    editpanel->addLargeAction(m_pDelete);

    QAction *editSeparator = editpanel->addSeparator();
    editpanel->setActionRowProportionProperty(editSeparator, SARibbonPannelItem::Small);

    m_pContrst = createAction(tr("Контраст"), "AcrylicBrush", QKeySequence("Alt+F9"));
    m_pContrst->setCheckable(true);
    connect(m_pContrst, &QAction::toggled, [&](bool toogled)
    {
        panelItem->setProperty(CONTRAST_PROPERTY, toogled);
    });

    m_pScrolAreaAction = createAction(tr("Область скролинга"), "RowUpdating", QKeySequence("Alt+F9"));
    m_pScrolAreaAction->setCheckable(true);
    m_pScrolAreaAction->setVisible(m_Type != LbrObject::RES_PANEL);

    if (m_Type != LbrObject::RES_PANEL)
        connect(m_pScrolAreaAction, SIGNAL(toggled(bool)), panelItem, SLOT(showScrolArea(bool)));
    /*connect(m_pScrolAreaAction, &QAction::toggled, [&](bool toogled)
    {
        QMetaObject *panelMeta = panelItem->metaObject();
        //panelItem->setProperty(SCROLAREA_PROPERTY, toogled);
    });*/

    app->settings()->beginGroup("StdEditor");
    m_pContrst->setChecked(app->settings()->value("AutoContrast", true).toBool());
    app->settings()->endGroup();
    editpanel->addLargeAction(m_pContrst);
    editpanel->addLargeAction(m_pScrolAreaAction);
    editpanel->addSeparator();

    m_pCutAction = createAction(tr("Вырезать"), "Cut", QKeySequence::Cut);
    connect(m_pCutAction, &QAction::triggered, this, &StdPanelEditor::sceneCutItems);
    editpanel->addSmallAction(m_pCutAction);

    m_pCopyAction = createAction(tr("Копировать"), "Copy", QKeySequence::Copy);
    connect(m_pCopyAction, &QAction::triggered, this, &StdPanelEditor::sceneCopyItems);
    editpanel->addSmallAction(m_pCopyAction);

    m_pPasteAction = createAction(tr("Вставить"), "Paste", QKeySequence::Paste);
    connect(m_pPasteAction, &QAction::triggered, this, &StdPanelEditor::scenePasteItems);
    editpanel->addSmallAction(m_pPasteAction);

    QAction *centerAction = createAction(tr("Выводить панель по центру"), "AlignCenter");
    QAction *alignRightAction = createAction(tr("Выравнивание текста справа"), "AlignRight");
    centerAction->setCheckable(true);
    alignRightAction->setCheckable(true);
    respanel->addSmallAction(centerAction);
    respanel->addSmallAction(alignRightAction);

    SARibbonPannel *checkpanel = category->addPannel(tr("Рецензирование"));
    m_pSpellCheckAction = createAction(tr("Проверить орфографию"), "SpellingCheck", QKeySequence("Alt+H"));
    connect(m_pSpellCheckAction, &QAction::triggered, this, &StdPanelEditor::CheckSpelling);
    checkpanel->addLargeAction(m_pSpellCheckAction);

    m_pCheckAction = createAction(tr("Проверить на ошибки"), "ValidateDocument", QKeySequence("Ctrl+H"));
    connect(m_pCheckAction, &QAction::triggered, this, &StdPanelEditor::onCheckRes);
    checkpanel->addSmallAction(m_pCheckAction);

    m_EwViewAction = createAction(tr("Просмотр в EW"), "FormInstance", QKeySequence("Ctrl+F3"));
    connect(m_EwViewAction, &QAction::triggered, this, &StdPanelEditor::onViewEasyWin);
    checkpanel->addSmallAction(m_EwViewAction);

    m_Statistic = createAction(tr("Информация"), "InformationSymbol");
    checkpanel->addSmallAction(m_Statistic);

    SARibbonPannel *borderpanel = category->addPannel(tr("Рамка"));
    m_pBorderStyleGallery = borderpanel->addGallery();
    MakeBorderRaibbonGallary(m_pBorderStyleGallery);

    SARibbonPannel *stylepanel = category->addPannel(tr("Стиль"));
    m_pPanelStyleGallery = stylepanel->addGallery();
    MakeStyleRaibbonGallary(m_pPanelStyleGallery, SLOT(OnPanelStyleSelected(QAction*)));
    ApplyPanelStyleToGallary();

    SARibbonPannel *excludepanel = category->addPannel(tr("Исключить"));

    QAction *excludeAutoStep = createAction(tr("Автоматический обход"), "Step");
    excludeAutoStep->setCheckable(true);
    excludepanel->addSmallAction(excludeAutoStep);

    QAction *excludeAutoNum = createAction(tr("Автоматическую нумерацию полей"), "NumericListBox");
    excludeAutoNum->setCheckable(true);
    excludepanel->addSmallAction(excludeAutoNum);

    QAction *excludeShadow = createAction(tr("Отображение тени"), "Shader_exp");
    excludeShadow->setCheckable(true);
    excludepanel->addSmallAction(excludeShadow);

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

    m_RibbonMapper.reset(new PropertyWidgetMapper());
    m_RibbonMapper->bind(panelItem, "isCentered", centerAction);
    m_RibbonMapper->bind(panelItem, "isRightText", alignRightAction);

    m_RibbonMapper->bind(panelItem, "isExcludeNavigation", excludeAutoStep);
    m_RibbonMapper->bind(panelItem, "isExcludeAutoNum", excludeAutoNum);
    m_RibbonMapper->bind(panelItem, "isExcludeShadow", excludeShadow);
}

void StdPanelEditor::MakeStyleRaibbonGallary(SARibbonGallery* gallery, const char *slotName, bool inheritable)
{
    QList<QAction*> galleryActions;

    if (inheritable)
    {
        QAction *inheritStyle = createAction("Наследуемый", "");
        inheritStyle->setIcon(QIcon::fromTheme("InheritedControl"));
        inheritStyle->setData(ResStyle::MainStyle);
        galleryActions.append(inheritStyle);
    }

    QAction *scomStyle = createAction("SCOM Основной стиль", "");
    scomStyle->setIcon(QIcon(":/img/gallary_style/scom.png"));
    scomStyle->setData(ResStyle::SCOM);
    galleryActions.append(scomStyle);

    QAction *smesStyle = createAction("SMES Стиль сообщений", "");
    smesStyle->setIcon(QIcon(":/img/gallary_style/smes.png"));
    smesStyle->setData(ResStyle::SMES);
    galleryActions.append(smesStyle);

    QAction *rmesStyle = createAction("RMES Стиль аварийных сообщений", "");
    rmesStyle->setIcon(QIcon(":/img/gallary_style/rmes.png"));
    rmesStyle->setData(ResStyle::RMES);
    galleryActions.append(rmesStyle);

    QAction *shlpStyle = createAction("SHLP Стиль помощи", "");
    shlpStyle->setIcon(QIcon(":/img/gallary_style/shlp.png"));
    shlpStyle->setData(ResStyle::SHLP);
    galleryActions.append(shlpStyle);

    QAction *smenStyle = createAction("SMEN Стиль меню", "");
    smenStyle->setIcon(QIcon(":/img/gallary_style/smen.png"));
    smenStyle->setData(ResStyle::SMEN);
    galleryActions.append(smenStyle);

    QAction *sbcmStyle = createAction("SBCM Стиль с яркой рамкой", "");
    sbcmStyle->setIcon(QIcon(":/img/gallary_style/sbcm.png"));
    sbcmStyle->setData(ResStyle::SBCM);
    galleryActions.append(sbcmStyle);

    QAction *scrlStyle = createAction("SCRL Стиль справочных скролингов", "");
    scrlStyle->setIcon(QIcon(":/img/gallary_style/scrl.png"));
    scrlStyle->setData(ResStyle::SCRL);
    galleryActions.append(scrlStyle);

    m_pStyleGroup1 = gallery->addCategoryActions(tr("Стиль"), galleryActions);
    m_pStyleGroup1->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    m_pStyleGroup1->setGridMinimumWidth(80);

    if (slotName)
        connect(m_pStyleGroup1, SIGNAL(triggered(QAction*)), this, slotName);
}

void StdPanelEditor::MakeBorderRaibbonGallary(SARibbonGallery* gallery)
{
    QList<QAction*> galleryActions;

    QAction *emptyBorder = createAction("Отсутствует", "");
    emptyBorder->setIcon(QIcon("://img/gallary_border/empty.png"));
    emptyBorder->setData(ResStyle::Border_NoLine);
    galleryActions.append(emptyBorder);

    QAction *singleBorder = createAction("Одинарная", "");
    singleBorder->setIcon(QIcon("://img/gallary_border/single.png"));
    singleBorder->setData(ResStyle::Border_SingleLine);
    galleryActions.append(singleBorder);

    QAction *doubleBorder = createAction("Двойная", "");
    doubleBorder->setIcon(QIcon("://img/gallary_border/double.png"));
    doubleBorder->setData(ResStyle::Border_DoubleLine);
    galleryActions.append(doubleBorder);

    QAction *combine1Border = createAction("Комбинированная 1", "");
    combine1Border->setIcon(QIcon("://img/gallary_border/combine1.png"));
    combine1Border->setData(ResStyle::Border_Combine1);
    galleryActions.append(combine1Border);

    QAction *combine2Border = createAction("Комбинированная 2", "");
    combine2Border->setIcon(QIcon("://img/gallary_border/combine2.png"));
    combine2Border->setData(ResStyle::Border_Combine2);
    galleryActions.append(combine2Border);

    QAction *solidBorder = createAction("Сплошная", "");
    solidBorder->setIcon(QIcon("://img/gallary_border/solid.png"));
    solidBorder->setData(ResStyle::Border_Solid);
    galleryActions.append(solidBorder);

    m_pBorderGroup1 = gallery->addCategoryActions(tr("Рамки"), galleryActions);
    m_pBorderGroup1->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    m_pBorderGroup1->setGridMinimumWidth(80);

    ApplyBorderStyleToGallary();

    connect(m_pBorderGroup1, &SARibbonGalleryGroup::triggered, this, &StdPanelEditor::OnBorderStyleSelected);
}

void StdPanelEditor::OnBorderStyleSelected(QAction *pAction)
{
    int borderStyle = pAction->data().toInt();
    panelItem->setBorderStyle((ResStyle::BorderStyle)borderStyle);
}

void StdPanelEditor::OnPanelStyleSelected(QAction *pAction)
{
    int panelStyle = pAction->data().toInt();
    panelItem->setPanelStyle((ResStyle::PanelStyle)panelStyle);
}

void StdPanelEditor::initRibbonPanels()
{
    m_pPanelCategory = new SARibbonCategory(tr("Панель"), ribbon());
    m_pPanelCategory->setObjectName(name());
    MakeResRibbonCategory(m_pPanelCategory);

    m_pControlCategory = new SARibbonCategory(tr("Поле"), ribbon());
    m_pControlCategory->setObjectName(name() + "_control");
    MakeControlRibbonCategory(m_pControlCategory);
}

void StdPanelEditor::ApplyBorderStyleToGallary()
{
    int select = -1;
    int borderStyle = panelItem->borderStyle();

    SARibbonGalleryGroupModel *model = m_pBorderGroup1->groupModel();
    for (int i = 0; i < model->rowCount(QModelIndex()); i++)
    {
        SARibbonGalleryItem *item = model->at(i);

        if (item->action()->data().toInt() == borderStyle)
        {
            select = i;
            break;
        }
    }

    m_pBorderGroup1->setCurrentIndex(m_pBorderGroup1->model()->index(select, 0));
    m_pBorderStyleGallery->currentViewGroup()->setCurrentIndex(m_pBorderStyleGallery->currentViewGroup()->model()->index(select, 0));
}

void StdPanelEditor::ApplyPanelStyleToGallary()
{
    int select = -1;
    int panelStyle = panelItem->panelStyle();

    SARibbonGalleryGroupModel *model = m_pStyleGroup1->groupModel();
    for (int i = 0; i < model->rowCount(QModelIndex()); i++)
    {
        SARibbonGalleryItem *item = model->at(i);

        if (item->action()->data().toInt() == panelStyle)
        {
            select = i;
            break;
        }
    }

    m_pStyleGroup1->setCurrentIndex(m_pStyleGroup1->model()->index(select, 0));
    m_pPanelStyleGallery->currentViewGroup()->setCurrentIndex(m_pPanelStyleGallery->currentViewGroup()->model()->index(select, 0));
}

void StdPanelEditor::updateRibbonTabs()
{
    SARibbonContextCategory *context = findCategoryByName(tr("Ресурс"));

    if (!context)
        return;

    QList<SARibbonCategory*> oldCategories = context->categoryList();
    for (SARibbonCategory *cat : qAsConst(oldCategories))
        context->takeCategory(cat);

    if (m_pPanelCategory)
    {
        if (!context->isHaveCategory(m_pPanelCategory))
            context->addCategoryPage(m_pPanelCategory);
    }

    bool HasControls = false;
    QList<QGraphicsItem*> sel = m_pView->scene()->selectedItems();
    for (QGraphicsItem *item : qAsConst(sel))
    {
        ControlItem *control = dynamic_cast<ControlItem*>(item);

        if (control)
            HasControls = true;
    }

    if (HasControls && m_pControlCategory)
    {
        if (!context->isHaveCategory(m_pControlCategory))
            context->addCategoryPage(m_pControlCategory);
    }

    ribbon()->showContextCategory(context);
    ribbon()->showCategory(m_pPanelCategory);

    if (HasControls && m_pControlCategory)
        ribbon()->showCategory(m_pControlCategory);
}

void StdPanelEditor::clearRibbonTabs()
{
    SARibbonContextCategory *context = findCategoryByName(tr("Ресурс"));

    if (!context)
        return;

    if (m_pPanelCategory)
    {
        ribbon()->removeCategory(m_pPanelCategory);
        ribbon()->hideCategory(m_pPanelCategory);
    }

    if (m_pControlCategory)
    {
        ribbon()->removeCategory(m_pControlCategory);
        ribbon()->hideCategory(m_pControlCategory);
    }

    bool hasVisible = false;
    QList<SARibbonCategory*> oldCategories = context->categoryList();
    for (SARibbonCategory *cat : qAsConst(oldCategories))
    {
        if (ribbon()->isCategoryVisible(cat))
        {
            hasVisible = true;
            break;
        }
    }

    if (!hasVisible)
        ribbon()->hideContextCategory(context);
}
