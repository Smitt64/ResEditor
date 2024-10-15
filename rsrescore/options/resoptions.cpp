#include "options/resoptions.h"
#include "options/generaloptionspage.h"
#include <QIcon>

ResOptions::ResOptions(QSettings *settings, QWidget *parent) :
    OptionsDlg(settings, parent)
{
    setDefaultStyle("windowsvista");

    m_pGeneral = new GeneralOptionsPage();
    addPage(tr("Общие"), QIcon(":/img/explorer.exe_14_103-6.png"), m_pGeneral);
    addStylePage(QString(), "style");
    addCodeEditorPage("CodeEditor", "theme");
    //addRslPage();

    //addCommandsPage();
    addLogPage("WorkRes");
    addUpdatePage();
}

ResOptions::~ResOptions()
{

}

void ResOptions::setAutoUnloadDir(const QString &filename)
{
    if (filename.isEmpty())
        return;

    m_pGeneral->setAutoUnloadDir(filename);
}
