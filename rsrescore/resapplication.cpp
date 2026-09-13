#include "resapplication.h"
#include "codeeditor/highlighterstyle.h"
#include <toolsruntime.h>
#include <QSettings>
#include <QDir>
#include <QStyleFactory>

ResApplication::ResApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    QApplication::setApplicationName("WorkLbr");
    //QApplication::setApplicationVersion(GetVersionNumberString());

    QDir dir = QDir(applicationDirPath());
    m_pSettings = new QSettings(dir.absoluteFilePath("lbropt.ini"), QSettings::IniFormat);

    QDir current(QDir::current());
    addLibraryPath(QFileInfo(QCoreApplication::applicationFilePath()).path());
    addLibraryPath(current.absolutePath());
    addLibraryPath(current.absoluteFilePath("platforms"));
    addLibraryPath(current.absoluteFilePath("iconengines"));
    addLibraryPath(current.absoluteFilePath("imageformats"));
    addLibraryPath(current.absoluteFilePath("platforms"));
    addLibraryPath(current.absoluteFilePath("sqldrivers"));
    addLibraryPath(current.absoluteFilePath("styles"));

    // Тема подсветки жёстко задана под офисный стиль приложения,
    // настройка CodeEditor/theme из lbropt.ini игнорируется
    HighlighterStyle::inst()->loadStyle(":/res/Office 2013 (Blue).json");
    HighlighterStyle::inst()->setDefaultTheme("Office 2013 (Blue)");
}

ResApplication::~ResApplication()
{

}

QSettings *ResApplication::settings()
{
    return m_pSettings;
}

void ResApplication::init()
{
    QDir trDir(applicationDirPath());

    if (trDir.cd("translations"))
    {
        QString translatorFile = QString("qt_%1").arg("ru");
        if (qt_translator.load(translatorFile, trDir.absolutePath()))
        {
            qCInfo(logSettings()) << "Translator installed: " << translatorFile;
            installTranslator(&qt_translator);
        }
        else
            qCWarning(logSettings()) << "Error loading translator " << translatorFile;
    }
    else
        qCWarning(logSettings()) << "Can't find translations folder";
}
