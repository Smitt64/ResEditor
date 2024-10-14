#ifndef LINESCANNER_H
#define LINESCANNER_H

#include <QTextCharFormat>
#include <QStack>
#include <QTextDocument>
#include "styles/resstyle.h"

class ResPanel;
class LineScanner
{
    typedef struct Element
    {
        QString text;
        int type;
        int pos;
    }Element;
public:
    enum
    {
        TypeLabel = 1,
        TypeControl,
    };

    LineScanner(QTextDocument *doc);
    void setMainFormat(const QTextCharFormat &format);
    void setBorders(const ResStyle::BorderChars &borders);
    void processCursor(QTextCursor &cursor);

    void clear();
    void makeControls(ResPanel *pPanel, QTextCursor &cursor);

private:
    void skipSpaces(QTextCursor &cursor);
    void finishLabel(QTextCursor &cursor);
    void finishControl(QTextCursor &cursor);
    QTextDocument *m_Doc;
    QTextCharFormat main_format;
    QStack<Element> m_Elements;
    ResStyle::BorderChars m_Borders;
};

#endif // LINESCANNER_H
