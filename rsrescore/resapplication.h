#ifndef RESAPPLICATION_H
#define RESAPPLICATION_H

#include <QApplication>

class QSettings;
class ResApplication : public QApplication
{
    Q_OBJECT
public:
    ResApplication(int argc, char *argv[]);
    virtual ~ResApplication();

private:
    QSettings *m_pSettings;
};

#endif // RESAPPLICATION_H
