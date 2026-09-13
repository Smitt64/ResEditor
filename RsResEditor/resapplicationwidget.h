#ifndef RESAPPLICATIONWIDGET_H
#define RESAPPLICATIONWIDGET_H

#include <ribbon/applicationwidgetbase.h>

class AppOptionsContentWidget;
class LbrObjectInterface;
class ResListCategoryContentWidget;
class GeneralOptionsPage;
class ResApplicationWidget : public ApplicationWidgetBase
{
    Q_OBJECT
public:
    ResApplicationWidget(LbrObjectInterface *pLbrObj, SARibbonMainWindow *parent = nullptr);
    virtual ~ResApplicationWidget();

signals:
    void openResource(const QString &name, const int &type);

private:
    void InitGeneralOptions();

    GeneralOptionsPage *m_pGeneralOptionsPage;
    AppOptionsContentWidget *m_pOptionsWidget;
    ResListCategoryContentWidget *m_pResListWidget;

    LbrObjectInterface *m_pLbrObj;
};

#endif // RESAPPLICATIONWIDGET_H
