#include "textpage.h"
#include "ui_textpage.h"
#include "styles/resstyle.h"
#include "respanel.h"
#include "wizards/texttopanel/linescanner.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>
#include <QStack>
#include <QRegExp>

TextPage::TextPage(QWidget *parent)
    : QWizardPage(parent)
    , ui(new Ui::TextPage)
{
    ui->setupUi(this);

    m_pPanel = new ResPanel();
    QTextDocument *doc = ui->textEdit->document();
    QFont font = doc->defaultFont();
    font.setFamily("Courier New");
    doc->setDefaultFont(font);
}

TextPage::~TextPage()
{
    delete ui;

    delete m_pPanel;
}

QString ScanHeader(QTextCursor &cursor, const ResStyle::BorderChars &borders)
{
    QString Title;
    while(cursor.movePosition(QTextCursor::Right))
    {
        QChar ch = cursor.document()->characterAt(cursor.position());

        if (ch == borders.chtopright)
            break;
        else if (ch != borders.horline)
            Title += ch;
    }

    return Title.simplified();
}

bool TextPage::validatePage()
{
    enum ScanOperation
    {
        Scan_TopLeftCorn = 1 << 1,
        Scan_Header = 1 << 2,
        Scan_StartLine = 1 << 3,
        Scan_EndPanel = 1 << 4,
    };

    QScopedPointer<ResStyle> rst(new ResStyle());

    QTextDocument *doc = ui->textEdit->document();
    QTextBlock block = doc->firstBlock();

    ResStyle::BorderChars borders;
    quint32 Opration = 0;

    QString Title, Status;
    QRect PanelRect(3, 3, 0, 1);

    LineScanner scanner(doc);
    while (block.isValid())
    {
        QTextCursor::MoveOperation op = QTextCursor::NoMove;
        QTextCursor cursor = QTextCursor(block);

        while(cursor.movePosition(op))
        {
            QChar ch = cursor.document()->characterAt(cursor.position());

            if (!(Opration & Scan_TopLeftCorn))
            {
                for (int i = ResStyle::Border_DoubleLine; i < ResStyle::Border_Count; i ++)
                {
                    borders = rst->borderChars((ResStyle::BorderStyle)i);
                    scanner.setBorders(borders);

                    if (borders.chtopleft == ch)
                    {
                        Opration |= Scan_TopLeftCorn | Scan_Header;
                        scanner.setMainFormat(cursor.charFormat());

                        m_pPanel->setPanelStyle((ResStyle::BorderStyle)i, ResStyle::SCOM);
                        PanelRect.setWidth(cursor.block().length() - 2);
                        Title = ScanHeader(cursor, borders);
                        break;
                    }
                }
            }
            else
            {

                if (!(Opration & Scan_StartLine))
                {
                    if (ch == borders.verline)
                    {
                        Opration |= Scan_StartLine;

                        if (borders.chbotleft != ch && !(Opration & Scan_EndPanel))
                            PanelRect.setHeight(PanelRect.height() + 1);
                    }
                    else
                    {
                        if (ch == borders.chbotright)
                        {
                            cursor.movePosition(QTextCursor::Right);
                            cursor.movePosition(QTextCursor::End, QTextCursor::KeepAnchor);
                            Status = cursor.selectedText().simplified().trimmed().mid(0, 254);
                        }
                    }
                }
                else
                {
                    if (ch == borders.verline)
                    {
                        Opration &= ~Scan_StartLine;
                        scanner.makeControls(m_pPanel, cursor);
                        scanner.clear();
                        break;
                    }
                    else
                        scanner.processCursor(cursor);
                }
            }

            op = QTextCursor::Right;
        }
        block = block.next();
    }

    //PanelRect.setHeight(doc->lineCount() + 1);
    m_pPanel->setPanelRect(PanelRect);
    m_pPanel->setPanelStrings(Title, Status, QString());

    //qDebug() << Title;

    return true;
}
