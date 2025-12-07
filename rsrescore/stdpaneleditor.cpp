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
#include "stdeditorscene.h"
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
#include <QGroupBox>
#include <QButtonGroup>
#include <QRadioButton>

#define SHADOW_CODE 9617

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

#define PanStyleIcon(_style) panelItem->style()->renderStyleIcon(panelItem->borderStyle(), _style, panelItem)
#define CtrlStyleIcon(_ctrlstyle) panelItem->style()->renderControlStyleIcon(_ctrlstyle, panelItem->panelStyle(), panelItem)
#define BorderIcon(border) panelItem->style()->renderBorderIcon(border, panelItem->panelStyle(), panelItem)

StdPanelEditor::StdPanelEditor(const qint16 &Type, QWidget *parent) :
    BaseEditorWindow(parent),
    m_pPanel(nullptr),
    panelItem(nullptr),
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
    setWindowTitle(tr("Редактирование панели"));

    m_SizeText = new StatusBarElement(this);
    m_SizeText->setPixmap(QIcon::fromTheme("MoveGlyphBox").pixmap(16));
    m_SizeText->setText(QString("0 x 0"));
    m_SizeText->setMaximumWidth(200);

    m_CursorText = new StatusBarElement(this);
    m_CursorText->setPixmap(QIcon::fromTheme("DirectSelection").pixmap(16));
    m_CursorText->setText(QString("0 : 0"));
    m_CursorText->setMaximumWidth(200);
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

QList<QWidget*> StdPanelEditor::statusBarSections()
{
    return { m_SizeText, m_CursorText };
}

void StdPanelEditor::setupEditor()
{
    m_pView = new StdEditorView(this);
    m_pView->setupScene();
    setCentralWidget(m_pView);

    panelItem->setBrush(QColor(128, 128, 0));

    m_pView->scene()->addItem(panelItem);

    panelItem->setCoord(QPoint(1, 1));
    panelItem->setSize(QSize(25, 20));

    m_pView->scene()->installEventFilter(this);
    m_pView->setMouseTracking(true);

    setupNameLine();
    initUndoRedo();

    StdEditorScene *baseScene = dynamic_cast<StdEditorScene*>(m_pView->scene());
    if (baseScene)
        initpropertyModelSignals(baseScene);

    loadToolBox();

    connect(panelItem, &PanelItem::titleChanged, [=]()
    {
        emit titleChanged(panelItem->title());
    });

    connect(panelItem, &PanelItem::panelStyleChanged, [=]()
    {
        m_pPanelStyleGallery->blockSignals(true);
        ApplyPanelStyleToGallary();
        UpdateGallarysIcons();
        m_pPanelStyleGallery->blockSignals(false);
    });

    connect(panelItem, &PanelItem::borderStyleChanged, [=]()
    {
        m_pBorderStyleGallery->blockSignals(true);
        ApplyBorderStyleToGallary();
        UpdateGallarysIcons();
        m_pBorderStyleGallery->blockSignals(false);
    });

    ControlItemsWrapper *wrp = baseScene->controlItemsWrapper();
    connect(wrp, &ControlItemsWrapper::controlStyleChanged, [=]()
    {
        m_pControlStyleGallery->blockSignals(true);
        ApplyControlStyleToGallary();
        m_pControlStyleGallery->blockSignals(false);
    });
}

void StdPanelEditor::setupNameLine()
{
    QFont font("TerminalVector", 10);
    font.setFixedPitch(true);

    m_pNameLineEdit = new SARibbonLineEdit(this);
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
    //m_TabContainer->addTab(pEdit, title);
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
    QSettings *pSettings = RsResCore::inst()->settings();

    QStringList recentDirs;
    int size = pSettings->beginReadArray("RecentXmlDirs");
    for (int i = 0; i < size; i++)
    {
        pSettings->setArrayIndex(i);
        QString dir = pSettings->value("path").toString();
        if (QDir(dir).exists() && !recentDirs.contains(dir))
            recentDirs.append(dir);
    }
    pSettings->endArray();

    fileDlg.setOption(QFileDialog::DontUseNativeDialog);
    fileDlg.setWindowTitle(tr("Сохранение в xml"));
    fileDlg.setAcceptMode(QFileDialog::AcceptOpen);
    fileDlg.setDirectory("/home/jana");
    fileDlg.setFileMode(QFileDialog::DirectoryOnly);
    fileDlg.setViewMode(QFileDialog::List);

    QList<QUrl> sidebarUrls;
    sidebarUrls << QUrl::fromLocalFile(QDir::homePath())
                << QUrl::fromLocalFile(QDir::currentPath());

    for (const QString &dir : recentDirs)
    {
        if (QDir(dir).exists())
            sidebarUrls.append(QUrl::fromLocalFile(dir));
    }
    fileDlg.setSidebarUrls(sidebarUrls);

    if (!recentDirs.isEmpty())
        fileDlg.setDirectory(recentDirs.first());
    else
        fileDlg.setDirectory(QDir::homePath());

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

    if (QDir(filename).exists())
    {
        recentDirs.removeAll(filename);
        recentDirs.prepend(filename);

        pSettings->beginWriteArray("RecentXmlDirs");
        for (int i = 0; i < recentDirs.size(); i++)
        {
            pSettings->setArrayIndex(i);
            pSettings->setValue("path", recentDirs[i]);
        }
        pSettings->endArray();
        pSettings->sync();
    }

    /*addCodeWindow(tr("XML"), result);*/
    bool showMessage = pSettings->value("StdPanelEditor/ShowXmlSuccessMessage", true).toBool();
    if (showMessage)
    {
        QMessageBox msgBox(QMessageBox::Information,
                           tr("Сохранение XML"),
                           tr("<b>Ресурс '%1' успешно экспортирован в XML</b><br><br>"
                              "Каталог: %2<br>"
                              "Кодировка: %3")
                               .arg(name())
                               .arg(QDir::toNativeSeparators(filename))
                               .arg(encodetxt),
                           QMessageBox::Ok,
                           this);

        msgBox.setCheckBox(new QCheckBox(tr("Больше не показывать")));
        msgBox.exec();

        if (msgBox.checkBox()->isChecked())
        {
            pSettings->setValue("StdPanelEditor/ShowXmlSuccessMessage", false);
            pSettings->sync();
        }
    }
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

    bool found = false;
    QSize grid = pScene->getGridSize();
    QRectF cursor(CursorPos, QSizeF(grid.width(), grid.height()));
    QList<CustomRectItem*> lst = pScene->findItems<CustomRectItem>();
    for (CustomRectItem *item : qAsConst(lst))
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
    dialog.setWindowIcon(QIcon::fromTheme("SpellingCheck"));
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
    StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());

    SARibbonPannel *fieldpanel = category->addPannel(tr("Параметры поля"));
    SARibbonPannel *fieldtypepanel = category->addPannel(tr("Тип поля"));
    SARibbonPannel *datatypepanel = category->addPannel(tr("Тип значения"));

    m_pFieldProperty = createAction(tr("Параметры поля"), "FieldProperties");
    toolAddActionWithTooltip(m_pFieldProperty,
                         tr("Открывает диалог параметров поля"),
                         QKeySequence("Enter"));
    fieldpanel->addLargeAction(m_pFieldProperty);

    m_pFdmAction = createAction(tr("Признак FDM"), "TimeLineLock");
    m_pFdmAction->setCheckable(true);
    toolAddActionWithTooltip(m_pFdmAction, tr("Включает/выключает режим FDM"));
    fieldpanel->addMediumAction(m_pFdmAction);

    m_pAsTextAction = createAction(tr("Признак текста"), "TextBlock");
    m_pAsTextAction->setCheckable(true);
    toolAddActionWithTooltip(m_pAsTextAction,
                         tr("Включает/выключает текстовый режим поля"));
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

        QString status;
        QString description = getFieldTypeDescription(value, status);
        ftype->setToolTip(description);
        ftype->setStatusTip(status);

        m_pFieledTypeGroup->addAction(ftype);
        fieldtypepanel->addMediumAction(ftype);
    }

    QMetaEnum DataTypeEnum = QMetaEnum::fromType<ControlItem::DataType>();
    for (int i = 0; i < DataTypeEnum.keyCount(); i++)
    {
        int value = DataTypeEnum.value(i);
        QAction *ftype = createAction(DataTypeEnum.valueToKey(value));
        ftype->setCheckable(true);
        ftype->setData(value);

        QString tooltip = getDataTypeDescription(static_cast<ControlItem::DataType>(value));
        ftype->setToolTip(tooltip);

        m_pDataTypeGroup->addAction(ftype);
        datatypepanel->addMediumAction(ftype);
    }

    SARibbonPannel *stylepanel = category->addPannel(tr("Стиль"));
    m_pControlStyleGallery = stylepanel->addGallery();
    MakeStyleRaibbonGallary(m_pControlStyleGallery, SLOT(OnControlStyleSelected(QAction*)), &m_pControlStyleGroup, true);

    m_pNoTabStop = createAction(tr("Исключить из обхода"), "ExcludePath");
    toolAddActionWithTooltip(m_pNoTabStop, tr("Исключает поле из перехода по Tab"));
    m_pNoTabStop->setCheckable(true);
    fieldpanel->addMediumAction(m_pNoTabStop);

    m_pListSelect = createAction(tr("Выбор из списка"), "ListBoxSearch");
    toolAddActionWithTooltip(m_pListSelect, tr("Позволяет пользователю выбирать значения из предопределенного списка"));
    m_pListSelect->setCheckable(true);
    fieldpanel->addMediumAction(m_pListSelect);

    ControlItemsWrapper *wrp = pScene->controlItemsWrapper();
    m_RibbonControlMapper.reset(new PropertyWidgetMapper());
    m_RibbonMapper->bind(wrp, "fieldType", m_pFieledTypeGroup);
    m_RibbonMapper->bind(wrp, "dataType", m_pDataTypeGroup);

    m_RibbonMapper->bind(wrp, "fdm", m_pFdmAction);
    m_RibbonMapper->bind(wrp, "isText", m_pAsTextAction);
    m_RibbonMapper->bind(wrp, "noTabStop", m_pNoTabStop);
    m_RibbonMapper->bind(wrp, "listSelect", m_pListSelect);

    connect(m_pFieldProperty, &QAction::triggered, [&]()
    {
        StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());
        ControlItemsWrapper *controlItemsWrapper = pScene->controlItemsWrapper();
        controlItemsWrapper->userAction(CustomRectItem::ActionKeyEnter);
    });
}

void StdPanelEditor::MakeResRibbonCategory(SARibbonCategory* category)
{
    ResApplication *app = (ResApplication*)qApp;
    SARibbonPannel *respanel = category->addPannel(tr("Панель"));

    QAction *panelPropertyAction = createAction(tr("Параметры панели"), "WindowsService");
    toolAddActionWithTooltip(panelPropertyAction,
                         tr("Открывает диалог свойств панели"),
                         QKeySequence("Enter"));
    respanel->addLargeAction(panelPropertyAction);
    respanel->addSeparator();

    m_SaveToXml = createAction(tr("Сохранить в XML"), "XMLFile", QKeySequence("Ctrl+ALT+S"));
    respanel->addLargeAction(m_SaveToXml);
    toolAddActionWithTooltip(m_SaveToXml,
                         tr("Сохраняет панель в XML-формат"),
                         QKeySequence("Ctrl+ALT+S"));

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
    toolAddActionWithTooltip(m_pCreateControl,
                         tr("Создает новое поле в позиции курсора"),
                         QKeySequence(Qt::Key_Insert));
    connect(m_pCreateControl, &QAction::triggered, this, &StdPanelEditor::onInsertControl);
    editpanel->addLargeAction(m_pCreateControl);

    m_pDelete = createAction(tr("Удалить элемент"), "DeleteClause", QKeySequence::Delete);
    toolAddActionWithTooltip(m_pDelete,
                         tr("Удаляет выбранные элементы"),
                         QKeySequence::Delete);
    connect(m_pDelete, &QAction::triggered, this, &StdPanelEditor::sceneDeleteItems);
    editpanel->addLargeAction(m_pDelete);

    QAction *editSeparator = editpanel->addSeparator();
    editpanel->setActionRowProportionProperty(editSeparator, SARibbonPannelItem::Small);

    m_pContrst = createAction(tr("Контраст"), "AcrylicBrush", QKeySequence("Alt+F9"));
    m_pContrst->setCheckable(true);
    toolAddActionWithTooltip(m_pContrst,
                         tr("Включает/выключает режим контраста для лучшей видимости"),
                         QKeySequence("Alt+F9"));
    connect(m_pContrst, &QAction::toggled, [&](bool toogled)
    {
        panelItem->setProperty(CONTRAST_PROPERTY, toogled);
        UpdateGallarysIcons();
    });

    m_pScrolAreaAction = createAction(tr("Область скролинга"), "RowUpdating", QKeySequence("Alt+F9"));
    m_pScrolAreaAction->setCheckable(true);
    toolAddActionWithTooltip(m_pScrolAreaAction,
                         tr("Показывает/скрывает область скролинга"),
                         QKeySequence("Alt+F9"));
    m_pScrolAreaAction->setVisible(m_Type != LbrObject::RES_PANEL);

    if (m_Type != LbrObject::RES_PANEL)
        connect(m_pScrolAreaAction, SIGNAL(toggled(bool)), panelItem, SLOT(showScrolArea(bool)));

    app->settings()->beginGroup("StdEditor");
    m_pContrst->blockSignals(true);
    m_pContrst->setChecked(app->settings()->value("AutoContrast", true).toBool());
    panelItem->setProperty(CONTRAST_PROPERTY, m_pContrst->isChecked());
    m_pContrst->blockSignals(false);
    app->settings()->endGroup();
    editpanel->addLargeAction(m_pContrst);
    editpanel->addLargeAction(m_pScrolAreaAction);
    editpanel->addSeparator();

    m_pCutAction = createAction(tr("Вырезать"), "Cut", QKeySequence::Cut);
    toolAddActionWithTooltip(m_pCutAction,
                         tr("Вырезает выбранные элементы в буфер обмена"),
                         QKeySequence::Cut);
    connect(m_pCutAction, &QAction::triggered, this, &StdPanelEditor::sceneCutItems);
    editpanel->addSmallAction(m_pCutAction);

    m_pCopyAction = createAction(tr("Копировать"), "Copy", QKeySequence::Copy);
    toolAddActionWithTooltip(m_pCopyAction,
                         tr("Копирует выбранные элементы в буфер обмена"),
                         QKeySequence::Copy);
    connect(m_pCopyAction, &QAction::triggered, this, &StdPanelEditor::sceneCopyItems);
    editpanel->addSmallAction(m_pCopyAction);

    m_pPasteAction = createAction(tr("Вставить"), "Paste", QKeySequence::Paste);
    toolAddActionWithTooltip(m_pPasteAction,
                         tr("Вставляет элементы из буфера обмена"),
                         QKeySequence::Paste);
    connect(m_pPasteAction, &QAction::triggered, this, &StdPanelEditor::scenePasteItems);
    editpanel->addSmallAction(m_pPasteAction);

    QAction *centerAction = createAction(tr("Выводить панель по центру"), "AlignCenter");
    toolAddActionWithTooltip(centerAction, tr("Выводит панель по центру экрана при отображении"));

    QAction *alignRightAction = createAction(tr("Выравнивание текста справа"), "AlignRight");
    toolAddActionWithTooltip(alignRightAction, tr("Выравнивает текст панели по правому краю"));
    centerAction->setCheckable(true);
    alignRightAction->setCheckable(true);
    respanel->addSmallAction(centerAction);
    respanel->addSmallAction(alignRightAction);

    SARibbonPannel *checkpanel = category->addPannel(tr("Рецензирование"));
    m_pSpellCheckAction = createAction(tr("Проверить орфографию"), "SpellingCheck", QKeySequence("Alt+H"));
    toolAddActionWithTooltip(m_pSpellCheckAction,
                         tr("Запускает проверку орфографии текстов панели"),
                         QKeySequence("Alt+H"));
    connect(m_pSpellCheckAction, &QAction::triggered, this, &StdPanelEditor::CheckSpelling);
    checkpanel->addLargeAction(m_pSpellCheckAction);

    m_pCheckAction = createAction(tr("Проверить на ошибки"), "ValidateDocument", QKeySequence("Ctrl+H"));
    toolAddActionWithTooltip(m_pCheckAction,
                         tr("Проверяет панель на наличие ошибок"),
                         QKeySequence("Ctrl+H"));
    connect(m_pCheckAction, &QAction::triggered, this, &StdPanelEditor::onCheckRes);
    checkpanel->addSmallAction(m_pCheckAction);

    m_EwViewAction = createAction(tr("Просмотр в EW"), "FormInstance", QKeySequence("Ctrl+F3"));
    toolAddActionWithTooltip(m_EwViewAction,
                         tr("Запускает просмотр панели в EasyWin"),
                         QKeySequence("Ctrl+F3"));
    connect(m_EwViewAction, &QAction::triggered, this, &StdPanelEditor::onViewEasyWin);
    checkpanel->addSmallAction(m_EwViewAction);

    m_Statistic = createAction(tr("Информация"), "InformationSymbol");
    toolAddActionWithTooltip(m_Statistic,
                         tr("Показывает статистическую информацию о панели"));
    checkpanel->addSmallAction(m_Statistic);

    SARibbonPannel *borderpanel = category->addPannel(tr("Рамка"));
    m_pBorderStyleGallery = borderpanel->addGallery();
    MakeBorderRaibbonGallary(m_pBorderStyleGallery);

    SARibbonPannel *stylepanel = category->addPannel(tr("Стиль"));
    m_pPanelStyleGallery = stylepanel->addGallery();
    MakeStyleRaibbonGallary(m_pPanelStyleGallery, SLOT(OnPanelStyleSelected(QAction*)), &m_pPanelStyleGroup);
    ApplyPanelStyleToGallary();

    SARibbonPannel *excludepanel = category->addPannel(tr("Исключить"));

    QAction *excludeAutoStep = createAction(tr("Автоматический обход"), "Step");
    toolAddActionWithTooltip(excludeAutoStep, tr("Исключает автоматический обход полей по Tab"));
    excludeAutoStep->setCheckable(true);
    excludepanel->addSmallAction(excludeAutoStep);

    QAction *excludeAutoNum = createAction(tr("Автоматическую нумерацию полей"), "NumericListBox");
    toolAddActionWithTooltip(excludeAutoNum, tr("Исключает автоматическую нумерацию полей панели"));
    excludeAutoNum->setCheckable(true);
    excludepanel->addSmallAction(excludeAutoNum);

    QAction *excludeShadow = createAction(tr("Отображение тени"), "Shader_exp");
    toolAddActionWithTooltip(excludeShadow, tr("Исключает отображение тени у элементов панели"));
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

#define StyleIcon(_style_) (inheritable ? CtrlStyleIcon(_style_) : PanStyleIcon(_style_))
void StdPanelEditor::MakeStyleRaibbonGallary(SARibbonGallery* gallery, const char *slotName, SARibbonGalleryGroup **pGroup, bool inheritable)
{
    QList<QAction*> galleryActions;

    if (inheritable)
    {
        QAction *inheritStyle = createAction("Наследуемый", "");
        inheritStyle->setIcon(StyleIcon(ResStyle::MainStyle));
        inheritStyle->setData(ResStyle::MainStyle);
        galleryActions.append(inheritStyle);
    }

    QAction *scomStyle = createAction("SCOM Основной стиль", "");
    scomStyle->setIcon(StyleIcon(ResStyle::SCOM));
    scomStyle->setData(ResStyle::SCOM);
    galleryActions.append(scomStyle);

    QAction *smesStyle = createAction("SMES Стиль сообщений", "");
    smesStyle->setIcon(StyleIcon(ResStyle::SMES));
    smesStyle->setData(ResStyle::SMES);
    galleryActions.append(smesStyle);

    QAction *rmesStyle = createAction("RMES Стиль аварийных сообщений", "");
    rmesStyle->setIcon(StyleIcon(ResStyle::RMES));
    rmesStyle->setData(ResStyle::RMES);
    galleryActions.append(rmesStyle);

    QAction *shlpStyle = createAction("SHLP Стиль помощи", "");
    shlpStyle->setIcon(StyleIcon(ResStyle::SHLP));
    shlpStyle->setData(ResStyle::SHLP);
    galleryActions.append(shlpStyle);

    QAction *smenStyle = createAction("SMEN Стиль меню", "");
    smenStyle->setIcon(StyleIcon(ResStyle::SMEN));
    smenStyle->setData(ResStyle::SMEN);
    galleryActions.append(smenStyle);

    QAction *sbcmStyle = createAction("SBCM Стиль с яркой рамкой", "");
    sbcmStyle->setIcon(StyleIcon(ResStyle::SBCM));
    sbcmStyle->setData(ResStyle::SBCM);
    galleryActions.append(sbcmStyle);

    QAction *scrlStyle = createAction("SCRL Стиль справочных скролингов", "");
    scrlStyle->setIcon(StyleIcon(ResStyle::SCRL));
    scrlStyle->setData(ResStyle::SCRL);
    galleryActions.append(scrlStyle);

    *pGroup = gallery->addCategoryActions(tr("Стиль"), galleryActions);
    (*pGroup)->setGalleryGroupStyle(SARibbonGalleryGroup::IconWithWordWrapText);
    (*pGroup)->setGridMinimumWidth(80);

    if (slotName)
        connect(*pGroup, SIGNAL(triggered(QAction*)), this, slotName);
}

void StdPanelEditor::MakeBorderRaibbonGallary(SARibbonGallery* gallery)
{
    QList<QAction*> galleryActions;

    QAction *emptyBorder = createAction("Отсутствует", "");
    emptyBorder->setIcon(BorderIcon(ResStyle::Border_NoLine));
    emptyBorder->setData(ResStyle::Border_NoLine);
    galleryActions.append(emptyBorder);

    QAction *singleBorder = createAction("Одинарная", "");
    singleBorder->setIcon(BorderIcon(ResStyle::Border_SingleLine));
    singleBorder->setData(ResStyle::Border_SingleLine);
    galleryActions.append(singleBorder);

    QAction *doubleBorder = createAction("Двойная", "");
    doubleBorder->setIcon(BorderIcon(ResStyle::Border_DoubleLine));
    doubleBorder->setData(ResStyle::Border_DoubleLine);
    galleryActions.append(doubleBorder);

    QAction *combine1Border = createAction("Комбинированная 1", "");
    combine1Border->setIcon(BorderIcon(ResStyle::Border_Combine1));
    combine1Border->setData(ResStyle::Border_Combine1);
    galleryActions.append(combine1Border);

    QAction *combine2Border = createAction("Комбинированная 2", "");
    combine2Border->setIcon(BorderIcon(ResStyle::Border_Combine2));
    combine2Border->setData(ResStyle::Border_Combine2);
    galleryActions.append(combine2Border);

    QAction *solidBorder = createAction("Сплошная", "");
    solidBorder->setIcon(BorderIcon(ResStyle::Border_Solid));
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

    UpdateGallarysIcons();
}

void StdPanelEditor::OnPanelStyleSelected(QAction *pAction)
{
    int panelStyle = pAction->data().toInt();
    panelItem->setPanelStyle((ResStyle::PanelStyle)panelStyle);

    UpdateGallarysIcons();
}

void StdPanelEditor::OnControlStyleSelected(QAction *pAction)
{
    StdEditorScene *pScene = dynamic_cast<StdEditorScene*>(m_pView->scene());
    ControlItemsWrapper *wrp = pScene->controlItemsWrapper();

    int controlStyle = pAction->data().toInt();
    wrp->setControlStyle((ResStyle::PanelStyle)controlStyle);
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
    ResStyle::PanelStyle panelStyle = panelItem->panelStyle();

    SARibbonGalleryGroupModel *model = m_pPanelStyleGroup->groupModel();
    for (int i = 0; i < model->rowCount(QModelIndex()); i++)
    {
        SARibbonGalleryItem *item = model->at(i);
        ResStyle::PanelStyle modelStyle = (ResStyle::PanelStyle)item->action()->data().toInt();

        if (modelStyle == panelStyle)
        {
            select = i;
            break;
        }
    }

    m_pPanelStyleGroup->setCurrentIndex(m_pPanelStyleGroup->model()->index(select, 0));
    m_pPanelStyleGallery->currentViewGroup()->setCurrentIndex(m_pPanelStyleGallery->currentViewGroup()->model()->index(select, 0));
}

void StdPanelEditor::ApplyControlStyleToGallary()
{
    int select = -1;
    StdEditorScene *baseScene = dynamic_cast<StdEditorScene*>(m_pView->scene());
    ControlItemsWrapper *wrp = baseScene->controlItemsWrapper();
    ResStyle::PanelStyle controlStyle = wrp->controlStyle();

    if (wrp->hasUniformValue("controlStyle"))
    {
        SARibbonGalleryGroupModel *model = m_pControlStyleGroup->groupModel();
        for (int i = 0; i < model->rowCount(QModelIndex()); i++)
        {
            SARibbonGalleryItem *item = model->at(i);
            ResStyle::PanelStyle modelStyle = (ResStyle::PanelStyle)item->action()->data().toInt();

            if (modelStyle == controlStyle)
            {
                select = i;
                break;
            }
        }

        m_pControlStyleGroup->setCurrentIndex(m_pPanelStyleGroup->model()->index(select, 0));
        m_pControlStyleGallery->currentViewGroup()->setCurrentIndex(m_pControlStyleGallery->currentViewGroup()->model()->index(select, 0));
    }
    else
    {
        m_pControlStyleGroup->clearSelection();
        m_pControlStyleGallery->currentViewGroup()->clearSelection();
    }
}

void StdPanelEditor::UpdateGallarysIcons()
{
    QList<QAction*> StyleGroup1 = m_pPanelStyleGroup->actionGroup()->actions();
    QList<QAction*> BorderGroup = m_pBorderGroup1->actionGroup()->actions();
    QList<QAction*> ControlStyleGroup = m_pControlStyleGroup->actionGroup()->actions();

    for (auto StyleAction : qAsConst(StyleGroup1))
        StyleAction->setIcon(PanStyleIcon((ResStyle::PanelStyle)StyleAction->data().toInt()));

    for (auto BorderAction : qAsConst(BorderGroup))
        BorderAction->setIcon(BorderIcon((ResStyle::BorderStyle)BorderAction->data().toInt()));

    for (auto ControlStyle : qAsConst(ControlStyleGroup))
        ControlStyle->setIcon(CtrlStyleIcon((ResStyle::PanelStyle)ControlStyle->data().toInt()));

    m_pBorderStyleGallery->update();
    m_pPanelStyleGallery->update();
    m_pControlStyleGroup->update();
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

QString StdPanelEditor::getFieldTypeDescription(const qint16 &fieldType, QString &description) const
{
    QString title;

    switch(fieldType)
    {
    case ControlItem::FET:
        title = tr("FET - Редактируемое поле");
        description = tr("Редактирование данных, clipboard, выбор из списка");
        break;
    case ControlItem::FBT:
        title = tr("FBT - Нередактируемое поле");
        description = tr("Отображение данных, кнопка без редактирования");
        break;
    case ControlItem::FBS:
        title = tr("FBS - Кнопка с тенью");
        description = tr("Визуальная кнопка с 3D-эффектом тени");
        break;
    case ControlItem::FWR:
        title = tr("FWR - Многострочное редактируемое поле");
        description = tr("Многострочный редактор, перенос текста");
        break;
    case ControlItem::FVT:
        title = tr("FVT - Поле просмотра");
        description = tr("Только для отображения, без редактирования");
        break;
    case ControlItem::FSP:
        title = tr("FSP - Subpanel");
        description = tr("Контейнер для группировки элементов");
        break;
    case ControlItem::FCL:
        title = tr("FCL - Cluster");
        description = tr("Check box или Radio button для выбора");
        break;
    case ControlItem::FVW:
        title = tr("FVW - Многострочное нередактируемое поле");
        description = tr("Многострочное отображение без редактирования");
        break;
    default:
        title = tr("Тип поля");
        description = tr("Неизвестный тип поля");
        break;
    }

    return QString("<b>%1</b><br>%2").arg(title, description);
}

QString StdPanelEditor::getDataTypeDescription(const qint16 &dataType) const
{
    QString title;
    QString description;
    QString ftType;
    QString size;
    QString cppType;

    switch(dataType)
    {
    case ControlItem::INT16:
        title = tr("INT16 - 16-битное целое число");
        description = tr("Целое число от -32,768 до 32,767");
        ftType = tr("FT_INT / FT_INT_NATIVE");
        size = tr("2 байта");
        cppType = tr("int16_t, short");
        break;

    case ControlItem::INT32:
        title = tr("INT32 - 32-битное целое число");
        description = tr("Целое число от -2,147,483,648 до 2,147,483,647");
        ftType = tr("FT_LONG / FT_LONG_NATIVE");
        size = tr("4 байта");
        cppType = tr("int32_t, long");
        break;

    case ControlItem::INT64:
        title = tr("INT64 - 64-битное целое число");
        description = tr("Целое число от -9,223,372,036,854,775,808 до 9,223,372,036,854,775,807");
        ftType = tr("FT_BIGINT / FT_BIGINT_NATIVE");
        size = tr("8 байт");
        cppType = tr("int64_t, long long");
        break;

    case ControlItem::FLOAT:
        title = tr("FLOAT - Число с плавающей точкой");
        description = tr("Число одинарной точности, ~7 значащих цифр");
        ftType = tr("FT_FLOAT / FT_FLOAT_NATIVE");
        size = tr("4 байта");
        cppType = tr("float, db_float");
        break;

    case ControlItem::FLOATG:
        title = tr("FLOATG - Число с плавающей точкой");
        description = tr("Аналогично FLOAT с группировкой разрядов");
        ftType = tr("FT_FLOATG / FT_FLOATG_NATIVE");
        size = tr("4 байта");
        cppType = tr("float, db_float");
        break;

    case ControlItem::DOUBLE:
        title = tr("DOUBLE - Число двойной точности");
        description = tr("Число двойной точности, ~15 значащих цифр");
        ftType = tr("FT_DOUBLE / FT_DOUBLE_NATIVE");
        size = tr("8 байт");
        cppType = tr("double, db_double");
        break;

    case ControlItem::DOUBLEG:
        title = tr("DOUBLEG - Число двойной точности");
        description = tr("Аналогично DOUBLE с группировкой разрядов");
        ftType = tr("FT_DOUBLEG / FT_DOUBLEG_NATIVE");
        size = tr("8 байт");
        cppType = tr("double, db_double");
        break;

    case ControlItem::LDOUBLE:
        title = tr("LDOUBLE - Длинное вещественное число");
        description = tr("Число расширенной точности, ~19 значащих цифр");
        ftType = tr("FT_LDOUBLE10 / FT_LDOUBLE10_NATIVE");
        size = tr("10 байт");
        cppType = tr("long double, db_double10");
        break;

    case ControlItem::MONEY:
        title = tr("MONEY - Денежный тип");
        description = tr("Денежная сумма");
        ftType = tr("FT_MONEY / FT_MONEY_NATIVE");
        size = tr("8 байт");
        cppType = tr("dmoney (Numeric), db_dmoney (DBNumeric)");
        break;

    case ControlItem::MONEYR:
        title = tr("MONEYR - Денежный тип");
        description = tr("Денежная сумма");
        ftType = tr("FT_MONEYR / FT_MONEYR_NATIVE");
        size = tr("8 байт");
        cppType = tr("dmoney (Numeric), db_dmoney (DBNumeric)");
        break;

    case ControlItem::LMONEY:
        title = tr("LMONEY - Длинная денежная сумма");
        description = tr("Денежная сумма расширенной точности");
        ftType = tr("FT_LMONEY / FT_LMONEY_NATIVE");
        size = tr("10 байт");
        cppType = tr("lmoney (Numeric), db_lmoney (DBNumeric)");
        break;

    case ControlItem::LMONEYR:
        title = tr("LMONEYR - Длинная денежная сумма ");
        description = tr("Денежная сумма");
        ftType = tr("FT_LMONEYR / FT_LMONEYR_NATIVE");
        size = tr("10 байт");
        cppType = tr("lmoney (Numeric), db_lmoney (DBNumeric)");
        break;

    case ControlItem::DECIMAL:
        title = tr("DECIMAL - Точное десятичное число");
        description = tr("Точное десятичное число с фиксированной точкой");
        ftType = tr("FT_DECIMAL / FT_DECIMAL_NATIVE");
        size = tr("16 байт");
        cppType = tr("decimal (Numeric), db_decimal (DBNumeric)");
        break;

    case ControlItem::NUMERIC:
        title = tr("NUMERIC - Числовой тип SQL");
        description = tr("SQL NUMERIC с заданной точностью и масштабом");
        ftType = tr("FT_NUMERIC / FT_NUMERIC_NATIVE");
        size = tr("20 байт");
        cppType = tr("DBNumeric, Numeric");
        break;

    case ControlItem::STRING:
        title = tr("STRING - Строковый тип");
        description = tr("Строка переменной длины");
        ftType = tr("FT_STRING");
        size = tr("Переменная длина");
        cppType = tr("char*");
        break;

    case ControlItem::SNR:
        title = tr("SNR - Строка");
        description = tr("Аналогично STRING, с форматом отображения");
        ftType = tr("FT_SNR");
        size = tr("Переменная длина");
        cppType = tr("char*");
        break;

    case ControlItem::NUMSTR:
        title = tr("NUMSTR - Числовая строка");
        description = tr("Строка, содержащая только цифры");
        ftType = tr("FT_NUMSTR");
        size = tr("Переменная длина");
        cppType = tr("char*");
        break;

    case ControlItem::DATE:
        title = tr("DATE - Дата");
        description = tr("Дата в формате базы данных");
        ftType = tr("FT_DATE / FT_DATE_NATIVE");
        size = tr("4 байта");
        cppType = tr("bdate, db_bdate");
        break;

    case ControlItem::TIME:
        title = tr("TIME - Время");
        description = tr("Время в формате базы данных");
        ftType = tr("FT_TIME / FT_TIME_NATIVE");
        size = tr("4 байта");
        cppType = tr("btime, db_btime");
        break;

    case ControlItem::SHTM:
        title = tr("SHTM - Короткое время");
        description = tr("Время без секунд в формате базы данных");
        ftType = tr("FT_SHTM / FT_SHTM_NATIVE");
        size = tr("4 байта");
        cppType = tr("btime, db_btime");
        break;

    case ControlItem::CHAR:
        title = tr("CHAR - Символ");
        description = tr("Один символ (байт)");
        ftType = tr("FT_CHR");
        size = tr("1 байт");
        cppType = tr("char, uchar");
        break;

    case ControlItem::UCHAR:
        title = tr("UCHAR - Беззнаковый символ");
        description = tr("Беззнаковый символ (0-255)");
        ftType = tr("FT_UCHR");
        size = tr("1 байт");
        cppType = tr("unsigned char, uint8_t");
        break;

    case ControlItem::PICTURE:
        title = tr("PICTURE - Изображение");
        description = tr("Двоичные данные изображения");
        ftType = tr("FT_PICTURE");
        size = tr("Переменная длина");
        cppType = tr("CRSImageBase, CRSImage");
        break;

    default:
        title = tr("Неизвестный тип данных");
        description = tr("Тип не определен");
        ftType = tr("?");
        size = tr("?");
        cppType = tr("?");
        break;
    }

    return QString("<html><div style='width: 500px;'>"
                   "<b>%1</b>"
                   "<div>%2</div>"
                   "<div>"
                   "<b>Внутренний тип:</b> %3<br>"
                   "<b>Размер:</b> %4<br>"
                   "<b>C++ тип:</b> %5"
                   "</div></div></html>")
        .arg(title)
        .arg(description)
        .arg(ftType)
        .arg(size)
        .arg(cppType);
}
