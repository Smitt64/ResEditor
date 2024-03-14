#include "mainwindow.h"
#include "rsrescore.h"
#include "resapplication.h"
#include <QtPlugin>
#include <QScopedPointer>

int main(int argc, char *argv[])
{
    QApplication::setAttribute(Qt::AA_EnableHighDpiScaling);

    char **argvnew = (char **)malloc((argc + 2) * sizeof(char*));
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
    strncpy_s(argvnew[argc + 1], 23, "windows:dpiawareness=1", 23);

    QScopedPointer<ResApplication> a(new ResApplication(argc, argv));
    RsResCore::inst()->init();

    MainWindow w;
    w.showMaximized();

    int stat = a->exec();

    // Освобождение памяти
    for (int i = 0; i < argc + 2; i++)
        free(argvnew[i]);
    free(argvnew);

    return stat;
}
