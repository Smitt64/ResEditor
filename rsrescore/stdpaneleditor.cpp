#include "stdpaneleditor.h"
#include "baseeditorview.h"
#include "panelitem.h"
#include "respanel.h"
#include "rsrescore.h"
#include "scrolitem.h"
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
    m_EwViewAction = addAction(m_pResMenu, QIcon(":/img/Panel2.png"), tr("Просмотр в EW"), QKeySequence("Ctrl+F3"));
    m_ViewAction = addAction(m_pResMenu, QIcon(":/img/PanelCmd.png"), tr("Просмотр"), QKeySequence("Alt+F3"));
    m_Statistic = addAction(m_pResMenu, QIcon(":/img/Statistic.png"), tr("Информация"));

    connect(m_SaveToXml, &QAction::triggered, this, &StdPanelEditor::saveToXml);
    connect(m_pCheckAction, &QAction::triggered, this, &StdPanelEditor::onCheckRes);
    connect(m_EwViewAction, &QAction::triggered, this, &StdPanelEditor::onViewEasyWin);
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
    setCentralWidget(m_TabContainer);

    panelItem->setBrush(QColor(128, 128, 0));

    m_pView->scene()->addItem(panelItem);

    panelItem->setCoord(QPoint(1, 1));
    panelItem->setSize(QSize(25, 20));

    m_pView->scene()->installEventFilter(this);
    m_pView->setMouseTracking(true);

    setupNameLine();
    m_pToolBar->addSeparator();
    m_pSave = addAction(QIcon(":/img/saveHS.png"), tr("Сохранить"), QKeySequence::Save);
    m_pToolBar->addSeparator();
    initUndoRedo(m_pToolBar);
    setupCopyPaste();

    m_pDelete = addAction(QIcon(":/img/Delete.png"), tr("Удалить"), QKeySequence::Delete);

    m_pEditMenu = m_pMenuBar->addMenu(tr("&Правка"));
    m_pViewMenu = m_pMenuBar->addMenu(tr("&Вид"));
    m_pResMenu = m_pMenuBar->addMenu(tr("&Ресурс"));
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
}

void StdPanelEditor::setupContrastAction()
{
    m_pToolBar->addSeparator();

    m_pContrst = addAction(QIcon(":/img/EditBrightContrastHS.png"), tr("Контраст"), QKeySequence("Alt+F9"));
    m_pContrst->setCheckable(true);

    connect(m_pContrst, &QAction::toggled, [&](bool toogled)
    {
        panelItem->setProperty(CONTRAST_PROPERTY, toogled);
    });

    m_pContrst->setChecked(true);
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
                                  item->fdm());
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
    QPlainTextEdit *pEdit = new QPlainTextEdit(this);
    pEdit->setReadOnly(true);
    pEdit->setPlainText(text);

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
    /*QDir dir(filename);

    ResPanel resPanel;
    ResBuffer *buffer = nullptr;
    lbr()->getResource(name(), type(), &buffer);
    resPanel.load(buffer);
    //fillResPanel(&resPanel);

    const char *preffix = resTypeStr(type());
    QFile f(dir.absoluteFilePath(QString("%1_%2.xml").arg(name()).arg(preffix)));
    if (f.open(QIODevice::WriteOnly))
    {
        QTextStream stream(&f);
        stream.setCodec(encode->currentText().toLocal8Bit().data());

        QString result = resPanel.saveXml(encode->currentText());
        stream << result;

        addCodeWindow(tr("XML"), result);
        f.close();
    }*/
}

void StdPanelEditor::showCheckError(int stat)
{
    if (stat)
    {
        QMessageBox::critical(this, tr("Ошибка проверки ресурса"),
                              ResPanel::GetCheckError(stat));
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

    int stat = panel.checkResource();
    showCheckError(stat);
}

void StdPanelEditor::onSave()
{
    ResPanel panel;
    fillResPanel(&panel);

    int stat = panel.checkResource();

    if (!stat)
        emit readySave();
    else
        showCheckError(stat);
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
