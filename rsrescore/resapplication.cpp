#include "resapplication.h"
#include <QSettings>
#include <QDir>
#include <QStyleFactory>

ResApplication::ResApplication(int &argc, char **argv) :
    QApplication(argc, argv)
{
    QDir dir = QDir(applicationDirPath());
    m_pSettings = new QSettings(dir.absoluteFilePath("lbropt.ini"), QSettings::IniFormat);

    /*QDir current(QDir::current());
    addLibraryPath(QFileInfo(QCoreApplication::applicationFilePath()).path());
    addLibraryPath(current.absolutePath());
    addLibraryPath(current.absoluteFilePath("platforms"));
    addLibraryPath(current.absoluteFilePath("iconengines"));
    addLibraryPath(current.absoluteFilePath("imageformats"));
    addLibraryPath(current.absoluteFilePath("platforms"));
    addLibraryPath(current.absoluteFilePath("sqldrivers"));
    addLibraryPath(current.absoluteFilePath("styles"));*/
}

ResApplication::~ResApplication()
{

}

QSettings *ResApplication::settings()
{
    return m_pSettings;
}

void ResApplication::applyStyle()
{
    QString styleName = m_pSettings->value("style").toString();
    QStyle *style = QStyleFactory::create(styleName);
    QApplication::setStyle(style);
}

void ResApplication::init()
{
    QDir trDir(applicationDirPath());

    if (trDir.cd("translations"))
    {
        QString translatorFile = QString("qt_%1").arg("ru");
        if (qt_translator.load(translatorFile, trDir.absolutePath()))
        {
            //qCInfo(logCore()) << "Translator installed: " << translatorFile;
            installTranslator(&qt_translator);
        }
        /*else
            qCWarning(logCore()) << "Error loading translator " << translatorFile;
   */ }
    /*else
        qCWarning(logCore()) << "Can't find translations folder";*/
}
