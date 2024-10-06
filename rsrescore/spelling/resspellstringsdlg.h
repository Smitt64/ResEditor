#ifndef RESSPELLSTRINGSDLG_H
#define RESSPELLSTRINGSDLG_H

#include <spelling/spellstringsdlg.h>

enum
{
    CheckModePanel = 1 << 1,
    CheckModeLabel = 1 << 2,
    CheckModeControl = 1 << 3,
    CheckModeTitle = 1 << 4,
    CheckModeStatus = 1 << 5,
    CheckModeStatus2 = 1 << 6,
    CheckModeControlName = 1 << 7,
    CheckModeToolTip = 1 << 7,
};

class CustomRectItem;
class ResSpellStringsDlg : public SpellStringsDlg
{
    Q_OBJECT
public:
    ResSpellStringsDlg(SpellChecker *spell, QWidget *parent = nullptr);
    virtual ~ResSpellStringsDlg();

protected:
    virtual bool skipWord(const QString &word) Q_DECL_OVERRIDE;
};

typedef std::tuple<int, CustomRectItem*> CheckUserDataTuple;

#endif // RESSPELLSTRINGSDLG_H
