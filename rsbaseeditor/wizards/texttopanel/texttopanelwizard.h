#ifndef TEXTTOPANELWIZARD_H
#define TEXTTOPANELWIZARD_H

#include <QWizard>

class TextPage;
class PanelItem;
class ResPanel;
class TextToPanelWizard : public QWizard
{
    Q_OBJECT
public:
    TextToPanelWizard(QWidget *parent = nullptr);
    virtual ~TextToPanelWizard();

    ResPanel *makePanel();

private:
    TextPage *m_pMaketText;
};

#endif // TEXTTOPANELWIZARD_H
