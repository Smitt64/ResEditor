#include "menucodehighlighter.h"

MenuCodeHighlighter::MenuCodeHighlighter(QObject *parent) :
    CodeHighlighter(parent)
{

}

MenuCodeHighlighter::~MenuCodeHighlighter()
{

}

void MenuCodeHighlighter::reset()
{
    CodeHighlighter::reset();

    // Ключевые слова текстового формата меню (MENU/POPUP/ITEM/DELIM/END)
    addKeyword("MENU");
    addKeyword("POPUP");
    addKeyword("ITEM");
    addKeyword("DELIM");
    addKeyword("END");
}
