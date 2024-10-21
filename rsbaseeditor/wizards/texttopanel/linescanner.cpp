#include "linescanner.h"
#include "respanel.h"
#include <QTextCursor>
#include <QTextBlock>
#include <controlitem.h>
#include <QRegularExpression>

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
        if ((elem.type & TypeLabel) && !elem.text.trimmed().isEmpty())
        {
            pPanel->addText(elem.text.trimmed(),
                              elem.pos,
                              cursor.block().blockNumber(),
                              0);
        }
        else if (elem.type & TypeControl)
        {
            quint8 DataType = ControlItem::STRING;
            quint8 FieldType = ControlItem::FET;
            quint16 DataLength = elem.text.size() + 1;

            QRegularExpression date("(^0[1-9]|[12][0-9]|3[01])\\.(0[1-9]|1[0-2])\\.(\\d{4}$)");
            QRegularExpressionMatch date_match = date.match(elem.text.trimmed());

            if (date_match.hasMatch())
            {
                DataLength = 0;
                DataType = ControlItem::DATE;
            }

            if (DataType == ControlItem::STRING)
            {
                QRegularExpression time("([01][0-9]|2[0-3]):([0-5][0-9]):([0-5][0-9])");
                QRegularExpressionMatch time_match = time.match(elem.text.trimmed());

                if (time_match.hasMatch())
                {
                    DataLength = 0;
                    DataType = ControlItem::TIME;
                }
            }

            if (DataType == ControlItem::STRING)
            {
                QRegularExpression real("((\\+|-)?([0-9]+)(\\.[0-9]+)?)|((\\+|-)?\\.?[0-9]+)");
                QRegularExpressionMatch real_match = real.match(elem.text.trimmed());

                if (real_match.hasMatch())
                {
                    if (elem.text.trimmed().contains("."))
                    {
                        DataLength = 2;
                        DataType = ControlItem::NUMERIC;
                    }
                    else
                    {
                        DataLength = 0;
                        DataType = ControlItem::INT16;
                    }
                }
            }

            if (DataType == ControlItem::STRING)
            {
                if ((DataLength - 1) == 1)
                {
                    DataLength = 0;
                    FieldType = ControlItem::FBT;
                    DataType = ControlItem::CHAR;
                }
            }

            pPanel->beginAddField(QString());
            pPanel->setFieldDataType(FieldType, DataType, DataLength, false, false);
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
                    {
                        if (!(m_Elements.top().type & TypeLeftRadio) &&
                            !(m_Elements.top().type & TypeLeftCheck))
                        {
                            QString addScan = scanForRightBracers(cursor);

                            if (!addScan.trimmed().isEmpty())
                            {
                                if (addScan.contains("("))
                                    m_Elements.top().type |= TypeRightRadio;
                                else if (addScan.contains("["))
                                    m_Elements.top().type |= TypeRightCheck;

                                m_Elements.top().addScan = addScan;
                            }
                        }
                        finishLabel(cursor);
                    }
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

QString LineScanner::scanForRightBracers(QTextCursor &cursor)
{
    QString line;

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
                cursor.movePosition(QTextCursor::Left);
            }
            else
            {
                if (ch != QChar::Space && ch != QChar::Nbsp)
                {
                    if (ch == '(' || ch == '[')
                    {
                        stop = true;
                        line += ch;
                    }
                }
                else
                    line += ch;
            }
        }
    }
    return line;
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
                if (ch != QChar::Space && ch != QChar::Nbsp && ch != ']' && ch != ')')
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
    QString label = m_Elements.top().text.trimmed();
    m_Elements.top().pos = line.indexOf(label);
    label = label.replace("(X)", "( )").replace("[X]", "[ ]");
    label = label.replace("(Х)", "( )").replace("[Х]", "[ ]");
    m_Elements.top().text = label;

    if (m_Elements.top().type & TypeRightRadio)
        m_Elements.top().text += m_Elements.top().addScan + " )";
    else if (m_Elements.top().type & TypeRightCheck)
        m_Elements.top().text += m_Elements.top().addScan + " ]";

    if (!(m_Elements.top().type & TypeRightRadio) &&
        !(m_Elements.top().type & TypeRightCheck)&&
        !(m_Elements.top().type & TypeLeftRadio)&&
        !(m_Elements.top().type & TypeLeftCheck))
    {
        if (label.endsWith("( )"))
            m_Elements.top().type |= TypeRightRadio;
        else if (label.endsWith("[ ]"))
            m_Elements.top().type |= TypeRightCheck;
    }

    if (label.endsWith("(") || label.endsWith("["))
    {
        if (label.endsWith("("))
        {
            label += " )";
            m_Elements.top().text = label;
        }

        if (label.endsWith("["))
        {
            label += " ]";
            m_Elements.top().text = label;
        }
    }

    if (m_Elements.top().pos == -1)
    {
        if ((m_Elements.top().type & TypeRightRadio) ||
            (m_Elements.top().type & TypeRightCheck))
        {
            QString label2 = label;
            label2 = label2.remove("[ ]").remove("( )").trimmed();
            m_Elements.top().pos = line.indexOf(label2);
        }
    }

    skipSpaces(cursor);
}

void LineScanner::finishControl(QTextCursor &cursor)
{
    skipSpaces(cursor);
}
