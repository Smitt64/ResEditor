#ifndef MENUCODEHIGHLIGHTER_H
#define MENUCODEHIGHLIGHTER_H

#include <QObject>
#include "codeeditor/codehighlighter.h"

class MenuCodeHighlighter : public CodeHighlighter
{
    Q_OBJECT
public:
    MenuCodeHighlighter(QObject *parent = Q_NULLPTR);
    virtual ~MenuCodeHighlighter();

protected:
    virtual void reset() Q_DECL_OVERRIDE;
};

#endif // MENUCODEHIGHLIGHTER_H
