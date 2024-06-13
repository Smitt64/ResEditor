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

ResPanel *TextToPanelWizard::makePanel()
{
    return m_pMaketText->m_pPanel;
}
