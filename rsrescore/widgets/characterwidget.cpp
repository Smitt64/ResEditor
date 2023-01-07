#include "characterwidget.h"
#include <QPainter>
#include <QTextCodec>
#include <QPaintEvent>
#include <QMouseEvent>
#include <QFontDatabase>
#include <QToolTip>
#include <windows.h>
#include <array>

typedef int(WINAPI* GetUNameFunc)(WORD wCharCode, LPWSTR lpBuf);

QString GetUnicodeCharacterName(wchar_t character)
{
    // https://github.com/reactos/reactos/tree/master/dll/win32/getuname
    typedef int(WINAPI* GetUNameFunc)(WORD wCharCode, LPWSTR lpBuf);
    static GetUNameFunc pfnGetUName = reinterpret_cast<GetUNameFunc>(::GetProcAddress(::LoadLibraryA("getuname.dll"), "GetUName"));

    if (!pfnGetUName)
        return {};

    std::array<WCHAR, 256> buffer;
    int length = pfnGetUName(character, buffer.data());

    return QString::fromWCharArray(buffer.data(), length);
}

CharacterWidget::CharacterWidget(QWidget *parent)
    : QWidget{parent},
      m_Columns(10)
{
    m_DisplayFont = QFont("TerminalVector");
    m_DisplayFont.setFixedPitch(true);
    fillGraphicsChar();
    updateSize("20");
    calculateSquareSize();
    setMouseTracking(true);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
}

CharacterWidget::~CharacterWidget()
{

}

const int &CharacterWidget::columns() const
{
    return m_Columns;
}

void CharacterWidget::setColumns(const int &value)
{
    m_Columns = value;
}

void CharacterWidget::updateFont(const QFont &font)
{
    m_DisplayFont.setFamily(font.family());
    calculateSquareSize();
    adjustSize();
    update();
}

void CharacterWidget::updateSize(const QString &fontSize)
{
    m_DisplayFont.setPointSize(fontSize.toInt());
    calculateSquareSize();
    adjustSize();
    update();
}

void CharacterWidget::updateStyle(const QString &fontStyle)
{
    QFontDatabase db;
    const QFont::StyleStrategy oldStrategy = m_DisplayFont.styleStrategy();
    m_DisplayFont = db.font(m_DisplayFont.family(), fontStyle, m_DisplayFont.pointSize());
    m_DisplayFont.setStyleStrategy(oldStrategy);
    calculateSquareSize();
    adjustSize();
    update();
}

void CharacterWidget::updateFontMerging(bool enable)
{
    if (enable)
        m_DisplayFont.setStyleStrategy(QFont::PreferDefault);
    else
        m_DisplayFont.setStyleStrategy(QFont::NoFontMerging);
    adjustSize();
    update();
}

const QFont &CharacterWidget::displayFont() const
{
    return m_DisplayFont;
}

void CharacterWidget::fillGraphicsChar()
{
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    for (int i = 176; i <= 223; i++)
    {
        QString str = codec->toUnicode(QByteArray(1, (char)i));
        m_Chars.append(str.front());
    }
}

void CharacterWidget::calculateSquareSize()
{
    m_SquareSize = qMax(16, 4 + QFontMetrics(m_DisplayFont, this).height());
}

QSize CharacterWidget::sizeHint() const
{
    QSize size = QSize(m_Columns * m_SquareSize, (m_Chars.size() / m_Columns) * m_SquareSize);
    return size;
}

void CharacterWidget::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.fillRect(event->rect(), QBrush(Qt::white));
    painter.setFont(m_DisplayFont);

    QRect redrawRect = event->rect();
    int beginRow = redrawRect.top() / m_SquareSize;
    int endRow = redrawRect.bottom() / m_SquareSize;
    int beginColumn = redrawRect.left() / m_SquareSize;
    int endColumn = redrawRect.right() / m_SquareSize;

    painter.setPen(QPen(Qt::gray));
    for (int row = beginRow; row <= endRow; ++row)
    {
        for (int column = beginColumn; column <= endColumn; ++column)
            painter.drawRect(column * m_SquareSize, row * m_SquareSize, m_SquareSize, m_SquareSize);
    }

    QFontMetrics fontMetrics(m_DisplayFont);
    painter.setPen(QPen(Qt::black));

    bool fIsNoRects = m_CharsRects.empty();
    for (int row = beginRow; row <= endRow; ++row)
    {
        for (int column = beginColumn; column <= m_Columns; ++column)
        {
            int key = row * m_Columns + column;
            QRect clipRect = QRect(column * m_SquareSize, row * m_SquareSize, m_SquareSize, m_SquareSize);
            painter.setClipRect(clipRect);

            if (key == lastKey)
                painter.fillRect(column * m_SquareSize + 1, row * m_SquareSize + 1,
                                 m_SquareSize, m_SquareSize, QBrush(Qt::red));

            if (key >= m_Chars.size())
                continue;

            if (fIsNoRects)
                m_CharsRects.insert(key, clipRect);

            painter.drawText(column * m_SquareSize + (m_SquareSize / 2) -
                             fontMetrics.horizontalAdvance(m_Chars[key]) / 2,
                             row * m_SquareSize + 4 + fontMetrics.ascent(),
                             QString(m_Chars[key]));
        }
    }
}

void CharacterWidget::mouseMoveEvent(QMouseEvent *event)
{
    QPoint widgetPosition = event->pos();

    for (QMap<int,QRect>::iterator i = m_CharsRects.begin(); i != m_CharsRects.end(); ++i)
    {
        if (i->contains(widgetPosition))
        {
            int charId = m_CharsRects.key(*i);

            QString charName = GetUnicodeCharacterName(m_Chars[charId].unicode());
            QString text = QString::fromLatin1("<p>Character: <span style=\"font-size: 24pt; font-family: %1\">").arg(m_DisplayFont.family())
                    + QString(m_Chars[charId])
                    + QString::fromLatin1("</span><p>Value: 0x")
                    + QString::number(m_Chars[charId].unicode(), 16)
                    + QString("<p>Name: %1</p>").arg(charName);

            QToolTip::showText(mapToGlobal(event->pos()), text, this);
            break;
        }
    }
}

void CharacterWidget::mouseDoubleClickEvent(QMouseEvent *event)
{
    QPoint widgetPosition = event->pos();

    for (QMap<int,QRect>::iterator i = m_CharsRects.begin(); i != m_CharsRects.end(); ++i)
    {
        if (i->contains(widgetPosition))
        {
            int charId = m_CharsRects.key(*i);
            emit characterSelected(m_Chars[charId]);
            break;
        }
    }
}
