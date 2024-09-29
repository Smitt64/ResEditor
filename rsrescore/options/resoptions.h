#ifndef RESOPTIONS_H
#define RESOPTIONS_H

#include <QObject>
#include "rsrescore_global.h"
#include <optionsdlg/optionsdlg.h>

class QSettings;
class GeneralOptionsPage;
class RSRESCORE_EXPORT ResOptions : public OptionsDlg
{
    Q_OBJECT
public:
    ResOptions(QSettings *settings, QWidget *parent = nullptr);
    virtual ~ResOptions();

    void setAutoUnloadDir(const QString &filename);

private:
    GeneralOptionsPage *m_pGeneral;
};

#endif // RESOPTIONS_H
