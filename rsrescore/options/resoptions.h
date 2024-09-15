#ifndef RESOPTIONS_H
#define RESOPTIONS_H

#include <QObject>
#include "rsrescore_global.h"
#include <optionsdlg/optionsdlg.h>

class QSettings;
class RSRESCORE_EXPORT ResOptions : public OptionsDlg
{
    Q_OBJECT
public:
    ResOptions(QSettings *settings, QWidget *parent = nullptr);
    virtual ~ResOptions();
};

#endif // RESOPTIONS_H
