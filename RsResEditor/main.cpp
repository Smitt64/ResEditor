#include "mainwindow.h"
#include "rsrescore.h"
#include "resapplication.h"
#include "IconThemeManager.h"
#include "styles/mdiofficestyle.h"
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
    IconThemeManager::initialize("vs_theme");

/*#ifdef QT_DEBUG
    IconThemeManager::addCustomPath("d:\\Work\\ResEditor\\RsResEditor\\res\\icons");
#endif*/
}

int main(int argc, char *argv[])
{
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

    // Позиционный аргумент - файл, переданный без ключа
    // (так вызывает проводник по ассоциации *.lbr: WorkRes.exe "%1")
    parser.addPositionalArgument("file",
        QCoreApplication::translate("main", "Lbr file to open (e.g. via shell association)"));

    ResApplication a(argc, argv);

    // Только после создания QApplication: без экземпляра applicationDirPath()
    // пуст, и тема иконок ищется относительно рабочего каталога (ломается
    // при запуске через ассоциацию файлов - CWD становится папкой файла)
    InitIconTheme();

    a.init();

    parser.process(a.arguments());
    RsResCore::inst()->init();

    MainWindow w;

    // Офисный стиль на всё приложение (как FmtRibbonMainWindow::ApplyRibbonProxy
    // в FmtLib): выпадающие меню ленты - top-level окна, stylesheet главного
    // окна их не достаёт, а палитру standardPalette() они получают отсюда
    MdiOfficeStyle::applyToApplication();

    w.show();

    if (parser.isSet(lbrFileOption))
        w.open(parser.value(lbrFileOption));
    else
    {
        // Файл из позиционного аргумента (ассоциация *.lbr).
        // Каждый файл открывается в отдельном экземпляре приложения.
        const QStringList posArgs = parser.positionalArguments();
        if (!posArgs.isEmpty())
            w.open(posArgs.first());
    }

    if (parser.isSet(resUnloadDirOption))
        w.setAutoUnloadDir(parser.value(resUnloadDirOption));

    int stat = a.exec();

    return stat;
}
