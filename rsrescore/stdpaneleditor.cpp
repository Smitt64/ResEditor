#include "stdpaneleditor.h"
#include "baseeditorview.h"
#include "panelitem.h"
#include "respanel.h"
#include "statusbarelement.h"
#include <QStatusBar>
#include <QHBoxLayout>
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QToolBar>
#include <QLineEdit>
#include <QFontMetrics>
#include <QPalette>

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

void StdPanelEditor::setupEditor()
{
    m_pToolBar = addToolBar(tr("Основная"));
    m_pToolBar->setIconSize(QSize(16, 16));

    m_pView = new BaseEditorView(this);
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
