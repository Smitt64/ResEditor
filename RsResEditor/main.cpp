#include "mainwindow.h"
#include "rsrescore.h"
#include <QtPlugin>
#include <QApplication>
#include <QScopedPointer>

int main(int argc, char *argv[])
{
    QScopedPointer<QApplication> a(new QApplication(argc, argv));
    RsResCore::inst()->init();
    //staticResCore->init();

    MainWindow w;
    w.showMaximized();
    return a->exec();
}
