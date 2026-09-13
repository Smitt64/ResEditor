#include "resapplicationwidget.h"
#include "lbrobjectinterface.h"
#include "options/generaloptionspage.h"
#include "resapplication.h"
#include "reslistcategorycontentwidget.h"
#include "ribbon/appoptionscontentwidget.h"
#include "ribbon/CategoryContentWidgetStyle.h"

ResApplicationWidget::ResApplicationWidget(LbrObjectInterface *pLbrObj, SARibbonMainWindow *parent) :
    ApplicationWidgetBase(parent),
    m_pLbrObj(pLbrObj)
{
    ResApplication *app = (ResApplication*)qApp;
    setMenuPanelColor(QColor(0x0072C6));

    m_pOptionsWidget = optionsWidget();
    m_pOptionsWidget->setSettings(app->settings());

    getCategoryContentStyle()->setBaseColor(QColor(0x0072C6));

    m_pGeneralOptionsPage = new GeneralOptionsPage();
    m_pOptionsWidget->addPage(tr("Общие"), QIcon::fromTheme("Dialog"), m_pGeneralOptionsPage);

    m_pOptionsWidget->addRslPage(QIcon::fromTheme("RunTestDialog"));
    m_pOptionsWidget->addLogPage(QIcon::fromTheme("Log"), "WorkFmt");

    if (m_pLbrObj)
    {
        m_pResListWidget = new ResListCategoryContentWidget(pLbrObj, this);
        addTab(tr("Русурсы"), m_pResListWidget);

        connect(m_pResListWidget, &ResListCategoryContentWidget::openResource, this, &ResApplicationWidget::openResource);
    }

    addTab(tr("Параметры"), m_pOptionsWidget);

    m_pOptionsWidget->configureStringListEditors(m_pGeneralOptionsPage);
}

ResApplicationWidget::~ResApplicationWidget()
{

}