#include "resapplication.h"
#include <QSettings>
#include <QDir>

ResApplication::ResApplication(int argc, char *argv[]) :
    QApplication(argc, argv)
{
    QDir dir = QDir(applicationDirPath());
    m_pSettings = new QSettings(dir.absoluteFilePath("lbropt.ini"), QSettings::IniFormat);
}

ResApplication::~ResApplication()
{

}

QSettings *ResApplication::settings()
{
    return m_pSettings;
}
