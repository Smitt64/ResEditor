#include "texttopanelwizard.h"
#include "textpage.h"
#include "panelitem.h"

TextToPanelWizard::TextToPanelWizard(QWidget *parent) :
    QWizard(parent)
{
    m_pMaketText = new TextPage();
    addPage(m_pMaketText);
}

TextToPanelWizard::~TextToPanelWizard()
{

}

PanelItem *TextToPanelWizard::makePanel()
{
    PanelItem *item = nullptr;

    return item;
}
