#ifndef TEXTTOPANELWIZARD_H
#define TEXTTOPANELWIZARD_H

#include <QWizard>

class TextPage;
class PanelItem;
class TextToPanelWizard : public QWizard
{
    Q_OBJECT
public:
    TextToPanelWizard(QWidget *parent = nullptr);
    virtual ~TextToPanelWizard();

    PanelItem *makePanel();

private:
    TextPage *m_pMaketText;
};

#endif // TEXTTOPANELWIZARD_H
