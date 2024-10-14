#include "linescanner.h"
#include "respanel.h"
#include <QTextCursor>
#include <QTextBlock>
#include <controlitem.h>

LineScanner::LineScanner(QTextDocument *doc)
{
    m_Doc = doc;
}

void LineScanner::setMainFormat(const QTextCharFormat &format)
{
    main_format = format;
}

void LineScanner::setBorders(const ResStyle::BorderChars &borders)
{
    m_Borders = borders;
}

void LineScanner::clear()
{
    m_Elements.clear();
}

void LineScanner::makeControls(ResPanel *pPanel, QTextCursor &cursor)
{
    for (const Element &elem : m_Elements)
    {
        if (elem.type == TypeLabel && !elem.text.trimmed().isEmpty())
        {
            pPanel->addText(elem.text.trimmed(),
                              elem.pos,
                              cursor.block().blockNumber(),
                              0);
        }
        else if (elem.type == TypeControl)
        {
            pPanel->beginAddField(QString());
            pPanel->setFieldDataType(ControlItem::FET, ControlItem::STRING, elem.text.size(), false, false);
            pPanel->setLenHeight(elem.text.size(), 1);
            pPanel->setFieldPos(elem.pos, cursor.block().blockNumber());
            pPanel->endAddField();
        }
    }
}

void LineScanner::processCursor(QTextCursor &cursor)
{
    if (m_Elements.isEmpty())
    {
        // пропускаем первые пробелы
        skipSpaces(cursor);
    }

    while(cursor.movePosition(QTextCursor::Right))
    {
        QChar ch = cursor.document()->characterAt(cursor.position());

        if (ch == m_Borders.verline)
        {
            cursor.movePosition(QTextCursor::Left);

            if (!m_Elements.isEmpty())
            {
                if (m_Elements.top().type == TypeLabel)
                    finishLabel(cursor);
                else
                    finishControl(cursor);
            }
            break;
        }
        else
        {
            QTextCursor c2(cursor);
            c2.movePosition(QTextCursor::Right);

            QTextCharFormat format = c2.charFormat();
            if (m_Elements.top().type == TypeLabel)
            {
                if (format.background() == main_format.background())
                {
                    m_Elements.top().text += ch;

                    if (ch == ":")
                        finishLabel(cursor);
                }
                else
                    finishLabel(cursor);
            }
            else
            {
                if (format.background() != main_format.background())
                    m_Elements.top().text += ch;
                else
                    finishControl(cursor);
            }
        }
    }
}

void LineScanner::skipSpaces(QTextCursor &cursor)
{
    bool stop = false;
    while(!stop)
    {
        cursor.movePosition(QTextCursor::Right);
        QChar ch = cursor.document()->characterAt(cursor.position());

        if (ch == m_Borders.verline)
        {
            cursor.movePosition(QTextCursor::Left);
            break;
        }
        else
        {
            QTextCursor c2(cursor);
            c2.movePosition(QTextCursor::Right);
            QTextCharFormat format = c2.charFormat();

            if (format.background() != main_format.background())
            {
                stop = true;
                Element elem;
                elem.type = TypeControl;
                elem.pos = cursor.positionInBlock();
                m_Elements.push(elem);
                cursor.movePosition(QTextCursor::Left);
            }
            else
            {
                if (ch != QChar::Space && ch != QChar::Nbsp)
                {
                    stop = true;
                    Element elem;
                    elem.type = TypeLabel;
                    elem.pos = cursor.positionInBlock();
                    m_Elements.push(elem);
                    cursor.movePosition(QTextCursor::Left);
                }
            }
        }
    }
}

void LineScanner::finishLabel(QTextCursor &cursor)
{
    QString line = cursor.block().text();
    QString label = m_Elements.top().text.simplified().trimmed();
    m_Elements.top().pos = line.indexOf(label);

    skipSpaces(cursor);
}

void LineScanner::finishControl(QTextCursor &cursor)
{
    skipSpaces(cursor);
}
