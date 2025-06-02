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

    //QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    /*char **argvnew = (char **)malloc((argc + 2) * sizeof(char*));
    for (int i = 0; i < argc + 2; i++)
    {
        if (i < argc)
        {
            int len = strlen(argv[i]) + 1;
            argvnew[i] = (char*)malloc(len * sizeof(char));
            strncpy_s(argvnew[i], len, argv[i], len);
        }
        else
        {
            int len = _MAX_PATH * sizeof(char);
            argvnew[i] = (char*)malloc(len);
            memset(argvnew[i], 0, len);
        }
    }

    strncpy_s(argvnew[argc], 11, "--platform", 11);
    strncpy_s(argvnew[argc + 1], 23, "windows:dpiawareness=1", 23);*/

    ResApplication a(argc, argv);
    parser.process(a.arguments());

    RsResCore::inst()->init();

    //QElapsedTimer cost;
    //cost.start();
    MainWindow w;
    w.show();
    //a.applyStyle();

    if (parser.isSet(lbrFileOption))
        w.open(parser.value(lbrFileOption));

    if (parser.isSet(resUnloadDirOption))
        w.setAutoUnloadDir(parser.value(resUnloadDirOption));

    int stat = a.exec();

    // Освобождение памяти
    /*for (int i = 0; i < argc + 2; i++)
        free(argvnew[i]);
    free(argvnew);*/

    return stat;
}
