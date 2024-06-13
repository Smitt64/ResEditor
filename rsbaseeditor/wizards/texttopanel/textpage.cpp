#include "textpage.h"
#include "ui_textpage.h"
#include "styles/resstyle.h"
#include "respanel.h"
#include <QTextBlock>
#include <QTextCursor>
#include <QDebug>
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
        Scan_StartLine = 1 << 3
    };
    QScopedPointer<ResStyle> rst(new ResStyle());

    QTextDocument *doc = ui->textEdit->document();
    QTextBlock block = doc->firstBlock();

    QTextCharFormat main_format;
    ResStyle::BorderChars borders;
    quint32 Opration = 0;

    QString Title, Label;
    QRect PanelRect(3, 3, 0, 0);

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

                    if (borders.chtopleft == ch)
                    {
                        Opration |= Scan_TopLeftCorn | Scan_Header;
                        main_format = cursor.charFormat();

                        m_pPanel->setPanelStyle((ResStyle::BorderStyle)i, ResStyle::SCOM);
                        PanelRect.setWidth(cursor.block().length() + 1);
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
                        Opration |= Scan_StartLine;
                }
                else
                {
                    if (ch == borders.verline)
                        Opration &= ~Scan_StartLine;
                    else
                    {
                        if (ch != ':' && ch != '[')
                            Label += ch;
                        else
                        {
                            int pos = -1;
                            QString line = cursor.block().text();

                            Label = Label.remove(QRegExp("[<\\[]+(\\D|\\d)+[>\\]]+")).simplified();
                            pos = line.indexOf(Label);

                            if (ch == '[')
                                Label += "[ ]";
                            else if (ch == ':')
                               Label += ":";

                            if (!Label.isEmpty() && pos >= 0)
                            {

                                m_pPanel->addText(Label, pos + 1, cursor.block().blockNumber(), 0);
                            }

                            Label = QString();
                        }
                    }
                }
            }

            op = QTextCursor::Right;
        }

        block = cursor.block();


        //qDebug() << "block.next()" << block.text();
        block = block.next();
    }

    PanelRect.setHeight(doc->lineCount() + 1);
    m_pPanel->setPanelRect(PanelRect);
    m_pPanel->setPanelStrings(Title, QString(), QString());

    //qDebug() << Title;

    return true;
}
