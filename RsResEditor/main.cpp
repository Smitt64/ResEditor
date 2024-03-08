#include "mainwindow.h"
#include "rsrescore.h"
#include "resapplication.h"
#include <QtPlugin>
#include <QScopedPointer>

int main(int argc, char *argv[])
{
    QScopedPointer<ResApplication> a(new ResApplication(argc, argv));
    RsResCore::inst()->init();
    //staticResCore->init();

    MainWindow w;
    w.showMaximized();
    return a->exec();
}
