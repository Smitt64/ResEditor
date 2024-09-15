#ifndef RESAPPLICATION_H
#define RESAPPLICATION_H

#include "rsrescore_global.h"
#include <QApplication>
#include <QTranslator>

class QSettings;
class RSRESCORE_EXPORT ResApplication : public QApplication
{
    Q_OBJECT
public:
    ResApplication(int &argc, char **argv);
    virtual ~ResApplication();

    QSettings *settings();
    void init();

    void applyStyle();

private:
    QSettings *m_pSettings;
    QTranslator qt_translator;
};

#define ResApp ((ResApplication*)qApp)

#endif // RESAPPLICATION_H
