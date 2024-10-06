#include "resspellstringsdlg.h"
#include <QRegExp>

ResSpellStringsDlg::ResSpellStringsDlg(SpellChecker *spell, QWidget *parent) :
    SpellStringsDlg(spell, parent)
{

}

ResSpellStringsDlg::~ResSpellStringsDlg()
{

}

bool ResSpellStringsDlg::skipWord(const QString &word)
{
    static QStringList skiplist =
    {
        "ESC",
        "CTRL",
        "ALT",
        "AD",
        "ENTER",
        "DEL",
        "DELETE",
        "HOME",
        "END",
        "BREAK",
        "INS",
        "INSERT",
        "SPACE",
        "PGUP",
        "PGUP",
        "GRAY"
    };

    if (skiplist.contains(word.toUpper()))
        return true;

    QRegExp rx("\\F\\d+");
    rx.setCaseSensitivity(Qt::CaseInsensitive);

    if (rx.indexIn(word) != -1)
        return true;

    return false;
}
