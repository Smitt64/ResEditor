#include "generaloptionspage.h"
#include "ui_generaloptionspage.h"
#include "basescene.h"
#include "panelitem.h"
#include "respanel.h"
#include "styles/resstyle.h"
#include <QSettings>
#include <QDomDocument>
#include <QResizeEvent>

class OptionsScene : public BaseScene
{
public:
    OptionsScene(QObject *parent = nullptr) :
        BaseScene(parent)
    {
        m_GridSize = ResStyle::gridSizes()[0];
    }

    virtual ~OptionsScene()
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

private:
    QSize m_GridSize;
};

GeneralOptionsPage::GeneralOptionsPage(QWidget *parent)
    : OptionsPage(parent)
    , ui(new Ui::GeneralOptionsPage)
{
    ui->setupUi(this);

    pScene = new OptionsScene(this);
    ui->graphicsView->installEventFilter(this);
    ui->graphicsView->setScene(pScene);
    pScene->setSceneRect(QRectF(0, 0, ui->graphicsView->width(),
                                ui->graphicsView->height()));

    ResPanel panel;

    QDomDocument doc;
    QFile file(":/res/options_preview.xml");
    if (file.open(QIODevice::ReadOnly))
    {
        doc.setContent(&file);
        file.close();

        panel.loadXmlNode(doc.documentElement().firstChild().toElement());
    }

    panelItem = new PanelItem();
    ui->graphicsView->scene()->addItem(panelItem);
    panelItem->setPanel(&panel);

    m_StdFolderListHandler = new StdFolderListHandler();
    ui->pathListWidget->setHandler(m_StdFolderListHandler);

    UpdateGridSizesView(0);
    connect(ui->gridSizesComboBox, qOverload<int>(&QComboBox::currentIndexChanged), [=](const int &index)
    {
        UpdateGridSizesView(index);
    });
}

GeneralOptionsPage::~GeneralOptionsPage()
{
    delete ui;
}

void GeneralOptionsPage::UpdateGridSizesView(const int &index)
{
    pScene->style()->setGridSize(index);
    panelItem->recalcByGridChanges();
    panelItem->update();
    pScene->update();
}

bool GeneralOptionsPage::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type() == QEvent::Resize && obj == ui->graphicsView)
    {
        QResizeEvent *ResizeEvent = (QResizeEvent*)event;
        pScene->setSceneRect(QRectF(0, 0, ResizeEvent->size().width(),
                                    ResizeEvent->size().height()));
    }
    return QObject::eventFilter(obj, event);
}

void GeneralOptionsPage::setAutoUnloadDir(const QString &filename)
{
    ui->pathListWidget->addStatic(filename);
}

int GeneralOptionsPage::save()
{
    QSettings *_sett = settings();
    QStringList lst = ui->pathListWidget->stringList();

    _sett->setValue("AutoUnload", ui->autoUnloadGroup->isChecked());
    _sett->setValue("AutoUnloadDirs", lst);

    QStringList gridSizes = ResStyle::gridSizesName();
    _sett->beginGroup("StdEditor");
    _sett->setValue("GridSizeMode", gridSizes[ui->gridSizesComboBox->currentIndex()]);
    _sett->setValue("AutoContrast", ui->autoContrastCheck->isChecked());
    _sett->endGroup();
    _sett->sync();

    return 0;
}

void GeneralOptionsPage::restore()
{
    QSettings *_sett = settings();
    ui->autoUnloadGroup->setChecked(_sett->value("AutoUnload", false).toBool());
    QStringList lst = _sett->value("AutoUnloadDirs").toStringList();
    ui->pathListWidget->addList(lst);

    QStringList gridSizes = ResStyle::gridSizesName();
    _sett->beginGroup("StdEditor");
    
    if (_sett->contains("GridSizeMode"))
    {
        int index = gridSizes.indexOf(_sett->value("GridSizeMode").toString());

        if (index != -1)
            ui->gridSizesComboBox->setCurrentIndex(index);
    }
    
    ui->autoContrastCheck->setChecked(_sett->value("AutoContrast", true).toBool());
    _sett->endGroup();
}
