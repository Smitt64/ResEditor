#ifndef RESAPPLICATION_H
#define RESAPPLICATION_H

#include "rsrescore_global.h"
#include <QApplication>

class QSettings;
class RSRESCORE_EXPORT ResApplication : public QApplication
{
    Q_OBJECT
public:
    ResApplication(int argc, char *argv[]);
    virtual ~ResApplication();

    QSettings *settings();

private:
    QSettings *m_pSettings;
};

#define ResApp ((ResApplication*)qApp)

#endif // RESAPPLICATION_H
