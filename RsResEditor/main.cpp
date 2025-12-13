#include "mainwindow.h"
#include "rsrescore.h"
#include "resapplication.h"
#include <QtPlugin>
#include <QScopedPointer>
#include <QCommandLineParser>
#include <QElapsedTimer>
#include <QDir>
#include <QIcon>
#include <SARibbon.h>

// boxy-svg.com

static void InitIconTheme()
{
    QStringList themes = QIcon::themeSearchPaths();
    themes.append("d:\\Work\\ResEditor\\RsResEditor\\res\\icons");
    QIcon::setThemeSearchPaths(themes);

    QIcon::setThemeName("vs_theme");
}

int main(int argc, char *argv[])
{
    InitIconTheme();
    SARibbonBar::initHighDpi();

    QCommandLineParser parser;
    parser.setApplicationDescription("Work Lbr");
    parser.addHelpOption();
    parser.addVersionOption();

    QCommandLineOption lbrFileOption(QStringList() << "l" << "lbr",
                                     QCoreApplication::translate("main", "Source lbr file to open"),
                                     QCoreApplication::translate("main", "file"));

    QCommandLineOption resUnloadDirOption(QStringList() << "s" << "sd",
                                     QCoreApplication::translate("main", "Unload resource to xml file into <directory>"),
                                     QCoreApplication::translate("main", "directory"));

    parser.addOption(lbrFileOption);
    parser.addOption(resUnloadDirOption);

    ResApplication a(argc, argv);
    a.init();

    parser.process(a.arguments());
    RsResCore::inst()->init();

    MainWindow w;
    w.show();

    if (parser.isSet(lbrFileOption))
        w.open(parser.value(lbrFileOption));

    if (parser.isSet(resUnloadDirOption))
        w.setAutoUnloadDir(parser.value(resUnloadDirOption));

    int stat = a.exec();

    return stat;
}
