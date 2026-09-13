#include "mdiofficestyle.h"

#include <QApplication>
#include <QFile>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPainter>
#include <QStyleFactory>
#include <QStyleOptionTitleBar>

MdiOfficeStyle::MdiOfficeStyle(QStyle *style)
    : QProxyStyle(style)
{
    loadIcons();
}

MdiOfficeStyle *MdiOfficeStyle::inst()
{
    // Собственный базовый стиль (не qApp->style()), чтобы рисование
    // MDI-окон не зависело от application stylesheet
    static MdiOfficeStyle *style = []()
    {
        MdiOfficeStyle *s = new MdiOfficeStyle();
        s->setBaseStyle(QStyleFactory::create(QStringLiteral("windowsvista")));
        return s;
    }();

    return style;
}

void MdiOfficeStyle::applyToApplication()
{
    // Порядок как в FmtLib (FmtRibbonMainWindow::ApplyRibbonProxy):
    // базовый стиль фиксируем ДО setStyle, иначе QProxyStyle с нулевой
    // базой после установки ссылается сам на себя
    MdiOfficeStyle *style = new MdiOfficeStyle();
    style->setBaseStyle(qApp->style());
    qApp->setStyle(style); // палитра приложения обновится из standardPalette()
}

void MdiOfficeStyle::loadIcons()
{
    // Иконки кнопок заголовка из ресурсов SARibbon
    const struct
    {
        QIcon *target;
        const char *path;
    } icons[] =
    {
        { &closeIcon,    ":/SARibbon/image/resource/Titlebar_Close.svg"  },
        { &minimizeIcon, ":/SARibbon/image/resource/Titlebar_Min.svg"    },
        { &maximizeIcon, ":/SARibbon/image/resource/Titlebar_Max.svg"    },
        { &restoreIcon,  ":/SARibbon/image/resource/Titlebar_Normal.svg" }
    };

    for (const auto &entry : icons)
    {
        if (QFile::exists(QString::fromLatin1(entry.path)))
            *entry.target = QIcon(QString::fromLatin1(entry.path));
    }

    // Fallback'и: рисуем простые иконки программно
    if (closeIcon.isNull())
    {
        QPixmap pm(16, 16);
        pm.fill(Qt::transparent);
        QPainter painter(&pm);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 2.0));
        painter.drawLine(3, 3, 13, 13);
        painter.drawLine(13, 3, 3, 13);
        closeIcon = QIcon(pm);
    }

    if (minimizeIcon.isNull())
    {
        QPixmap pm(16, 16);
        pm.fill(Qt::transparent);
        QPainter painter(&pm);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 2.0));
        painter.drawLine(3, 8, 13, 8);
        minimizeIcon = QIcon(pm);
    }

    if (maximizeIcon.isNull())
    {
        QPixmap pm(16, 16);
        pm.fill(Qt::transparent);
        QPainter painter(&pm);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 1.5));
        painter.drawRect(3, 3, 10, 10);
        maximizeIcon = QIcon(pm);
    }

    if (restoreIcon.isNull())
    {
        QPixmap pm(16, 16);
        pm.fill(Qt::transparent);
        QPainter painter(&pm);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setPen(QPen(Qt::black, 1.5));
        painter.drawRect(3, 3, 8, 8);
        painter.drawRect(5, 5, 8, 8);
        restoreIcon = QIcon(pm);
    }
}

// ---------------------------------------------------------------------------
// Отрисовка
// ---------------------------------------------------------------------------

void MdiOfficeStyle::drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                                   QPainter *painter, const QWidget *widget) const
{
    if (element == PE_FrameWindow && widget && qobject_cast<const QMdiSubWindow *>(widget))
    {
        drawMdiSubWindowFrame(option, painter, widget);
        return;
    }

    QProxyStyle::drawPrimitive(element, option, painter, widget);
}

void MdiOfficeStyle::drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                                        QPainter *painter, const QWidget *widget) const
{
    if (control == CC_TitleBar && widget && qobject_cast<const QMdiSubWindow *>(widget))
    {
        drawMdiSubWindowTitleBar(option, painter, widget);

        if (const QStyleOptionTitleBar *titleBarOption = qstyleoption_cast<const QStyleOptionTitleBar *>(option))
            drawTitleBarButtons(titleBarOption, painter, widget);

        return;
    }

    QProxyStyle::drawComplexControl(control, option, painter, widget);
}

void MdiOfficeStyle::drawMdiSubWindowFrame(const QStyleOption *option,
                                           QPainter *painter, const QWidget *widget) const
{
    if (!painter || !widget)
        return;

    painter->save();

    const QMdiSubWindow *subWindow = qobject_cast<const QMdiSubWindow *>(widget);
    const bool isActive = isActiveSubWindow(subWindow);

    const QColor frameColor = isActive ? activeFrameColor() : inactiveFrameColor();
    const int frameWidth = pixelMetric(PM_MdiSubWindowFrameWidth, option, widget);

    // Рамка заливкой полос ровно frameWidth пикселей (перо центрируется
    // на границе, из-за чего толщина "плывёт" и залезает на содержимое)
    const QRect r = widget->rect();
    painter->fillRect(QRect(r.left(), r.top(), frameWidth, r.height()), frameColor);
    painter->fillRect(QRect(r.right() - frameWidth + 1, r.top(), frameWidth, r.height()), frameColor);
    painter->fillRect(QRect(r.left(), r.top(), r.width(), frameWidth), frameColor);
    painter->fillRect(QRect(r.left(), r.bottom() - frameWidth + 1, r.width(), frameWidth), frameColor);

    painter->restore();
}

void MdiOfficeStyle::drawMdiSubWindowTitleBar(const QStyleOptionComplex *option,
                                              QPainter *painter, const QWidget *widget) const
{
    if (!painter || !widget || !option)
        return;

    painter->save();

    const QMdiSubWindow *subWindow = qobject_cast<const QMdiSubWindow *>(widget);
    const bool isActive = isActiveSubWindow(subWindow);

    const QColor titleBarColor = isActive ? titleBarActive() : titleBarInactive();
    const QColor titleTextColor = isActive ? titleTextActive() : titleTextInactive();
    const QColor borderColorValue = isActive ? activeFrameColor() : borderColor();

    // Фон заголовка
    const QRect titleRect = option->rect;
    painter->fillRect(titleRect, titleBarColor);

    // Контур заголовка внутри его rect (верх + бока, низ не рисуем)
    QPen borderPen(borderColorValue, 2);
    painter->setPen(borderPen);
    const int borderTop = titleRect.top() + 1;
    const int borderLeft = titleRect.left() + 1;
    const int borderRight = titleRect.right(); // перо 2px ляжет по right-1..right
    painter->drawLine(QPoint(borderLeft, borderTop), QPoint(borderRight, borderTop));
    painter->drawLine(QPoint(borderLeft, borderTop), QPoint(borderLeft, titleRect.bottom()));
    painter->drawLine(QPoint(borderRight, borderTop), QPoint(borderRight, titleRect.bottom()));

    const int titleBarHeight = pixelMetric(PM_TitleBarHeight, option, widget);
    const int buttonSize = titleBarHeight;
    const int frameWidth = pixelMetric(PM_MdiSubWindowFrameWidth, option, widget);

    // Количество кнопок
    int buttonCount = 0;
    const QStyleOptionTitleBar *titleBarOption = qstyleoption_cast<const QStyleOptionTitleBar *>(option);
    if (titleBarOption)
    {
        if (titleBarOption->titleBarFlags & Qt::WindowCloseButtonHint) buttonCount++;
        if (titleBarOption->titleBarFlags & Qt::WindowMaximizeButtonHint) buttonCount++;
        if (titleBarOption->titleBarFlags & Qt::WindowMinimizeButtonHint) buttonCount++;
    }

    const int buttonsWidth = buttonCount * buttonSize;

    // Иконка окна
    int iconOffset = 0;
    if (titleBarOption && !titleBarOption->icon.isNull())
    {
        const int iconSize = qMin(titleBarHeight - 8, 16);
        const int iconX = titleRect.left() + frameWidth + 4;
        const int iconY = titleRect.top() + (titleRect.height() - iconSize) / 2;
        const QRect iconRect(iconX, iconY, iconSize, iconSize);

        QIcon::Mode mode = isActive ? QIcon::Active : QIcon::Normal;
        titleBarOption->icon.paint(painter, iconRect, Qt::AlignCenter, mode, QIcon::On);

        iconOffset = iconSize + 8;
    }

    // Текст заголовка
    painter->setPen(titleTextColor);
    QFont font = painter->font();
    font.setFamily(QStringLiteral("Segoe UI"));
    font.setPointSize(9);
    font.setBold(isActive);
    painter->setFont(font);

    const int textLeft = titleRect.left() + frameWidth + 8 + iconOffset;
    const int textRight = titleRect.right() - buttonsWidth;
    const QRect textRect(textLeft, titleRect.top(),
                         textRight - textLeft, titleRect.height());

    const QString title = titleBarOption ? titleBarOption->text : widget->windowTitle();

    if (textRect.width() > 20)
    {
        painter->drawText(textRect, Qt::AlignLeft | Qt::AlignVCenter,
                          painter->fontMetrics().elidedText(title, Qt::ElideRight, textRect.width()));
    }

    painter->restore();
}

void MdiOfficeStyle::drawTitleBarButtons(const QStyleOptionTitleBar *option,
                                         QPainter *painter, const QWidget *widget) const
{
    if (!painter || !option || !widget)
        return;

    const QMdiSubWindow *subWindow = qobject_cast<const QMdiSubWindow *>(widget);
    if (!subWindow)
        return;

    const bool isMinimized = subWindow->windowState().testFlag(Qt::WindowMinimized);
    const bool isMaximized = subWindow->windowState().testFlag(Qt::WindowMaximized);

    if (option->titleBarFlags & Qt::WindowCloseButtonHint)
        drawTitleBarButton(option, painter, widget, SC_TitleBarCloseButton, closeIcon);

    if (option->titleBarFlags & Qt::WindowMaximizeButtonHint)
        drawTitleBarButton(option, painter, widget, SC_TitleBarMaxButton,
                           isMaximized ? restoreIcon : maximizeIcon);

    if (option->titleBarFlags & Qt::WindowMinimizeButtonHint)
    {
        if (isMinimized)
            drawTitleBarButton(option, painter, widget, SC_TitleBarNormalButton, restoreIcon);
        else
            drawTitleBarButton(option, painter, widget, SC_TitleBarMinButton, minimizeIcon);
    }
}

void MdiOfficeStyle::drawTitleBarButton(const QStyleOptionTitleBar *option,
                                        QPainter *painter, const QWidget *widget,
                                        SubControl control, const QIcon &icon) const
{
    if (!(option->subControls & control))
        return;

    const QRect buttonRect = subControlRect(CC_TitleBar, option, control, widget);
    if (!buttonRect.isValid())
        return;

    const bool isHovered = (option->activeSubControls & control) &&
                           (option->state & State_MouseOver);
    const bool isPressed = isHovered && (option->state & State_Sunken);

    QColor bgColor = Qt::transparent;
    if (isPressed)
        bgColor = (control == SC_TitleBarCloseButton) ? closeButtonPress() : buttonPressColor();
    else if (isHovered)
        bgColor = (control == SC_TitleBarCloseButton) ? closeButtonHover() : buttonHoverColor();

    if (bgColor != Qt::transparent)
    {
        QRect rc = buttonRect.adjusted(1, 1, -1, -1);
        rc.moveTop(2);
        painter->fillRect(rc, bgColor);
    }

    QIcon::Mode mode = isHovered ? QIcon::Active : QIcon::Normal;
    QIcon::State state = isPressed ? QIcon::On : QIcon::Off;

    const int titleBarHeight = pixelMetric(PM_TitleBarHeight, option, widget);
    const int iconPadding = titleBarHeight / 4;
    const QRect iconRect = buttonRect.adjusted(iconPadding, iconPadding, -iconPadding, -iconPadding);

    icon.paint(painter, iconRect, Qt::AlignCenter, mode, state);
}

// ---------------------------------------------------------------------------
// Геометрия
// ---------------------------------------------------------------------------

QSize MdiOfficeStyle::sizeFromContents(ContentsType type, const QStyleOption *option,
                                       const QSize &size, const QWidget *widget) const
{
    QSize s = QProxyStyle::sizeFromContents(type, option, size, widget);

    // место под рамку MDI-окна
    if (widget && qobject_cast<const QMdiSubWindow *>(widget))
    {
        s.rwidth() += 6;
        s.rheight() += 6;
    }

    return s;
}

int MdiOfficeStyle::pixelMetric(PixelMetric metric, const QStyleOption *option,
                                const QWidget *widget) const
{
    if (metric == PM_MdiSubWindowFrameWidth && widget && qobject_cast<const QMdiSubWindow *>(widget))
        return 2;

    if (metric == PM_MdiSubWindowMinimizedWidth && widget && qobject_cast<const QMdiSubWindow *>(widget))
        return 300;

    if (metric == PM_TitleBarHeight && widget && qobject_cast<const QMdiSubWindow *>(widget))
        return 34;

    if (metric == PM_TitleBarButtonIconSize)
        return 16;

    if (metric == PM_TitleBarButtonSize && widget && qobject_cast<const QMdiSubWindow *>(widget))
        return QProxyStyle::pixelMetric(PM_TitleBarHeight, option, widget);

    if (metric == PM_ButtonShiftHorizontal || metric == PM_ButtonShiftVertical)
        return 1;

    return QProxyStyle::pixelMetric(metric, option, widget);
}

QRect MdiOfficeStyle::subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                                     SubControl sc, const QWidget *widget) const
{
    if (cc == CC_TitleBar && widget && qobject_cast<const QMdiSubWindow *>(widget))
    {
        if (const QStyleOptionTitleBar *titleBarOpt = qstyleoption_cast<const QStyleOptionTitleBar *>(opt))
        {
            const int titleBarHeight = pixelMetric(PM_TitleBarHeight, titleBarOpt, widget);
            const int buttonSize = titleBarHeight;
            const int frameWidth = pixelMetric(PM_MdiSubWindowFrameWidth, titleBarOpt, widget);

            const QMdiSubWindow *window = qobject_cast<const QMdiSubWindow *>(widget);
            const bool isMinimized = window && window->windowState().testFlag(Qt::WindowMinimized);

            // Кнопки вплотную к правой границе, порядок справа налево:
            // Закрыть -> Во весь экран -> Свернуть/Восстановить
            int x = opt->rect.right();
            const int y = opt->rect.top();

            if (titleBarOpt->titleBarFlags & Qt::WindowCloseButtonHint)
            {
                const QRect closeRect(x - buttonSize, y, buttonSize, buttonSize);
                if (sc == SC_TitleBarCloseButton)
                    return closeRect;
                x -= buttonSize;
            }

            if (titleBarOpt->titleBarFlags & Qt::WindowMaximizeButtonHint)
            {
                const QRect maxRect(x - buttonSize, y, buttonSize, buttonSize);
                if (sc == SC_TitleBarMaxButton)
                    return maxRect;
                x -= buttonSize;
            }

            if (titleBarOpt->titleBarFlags & Qt::WindowMinimizeButtonHint)
            {
                const QRect minRect(x - buttonSize, y, buttonSize, buttonSize);
                if (isMinimized)
                {
                    if (sc == SC_TitleBarNormalButton)
                        return minRect;
                }
                else
                {
                    if (sc == SC_TitleBarMinButton)
                        return minRect;
                }
            }

            if (sc == SC_TitleBarLabel)
            {
                int buttonCount = 0;
                if (titleBarOpt->titleBarFlags & Qt::WindowCloseButtonHint) buttonCount++;
                if (titleBarOpt->titleBarFlags & Qt::WindowMaximizeButtonHint) buttonCount++;
                if (titleBarOpt->titleBarFlags & Qt::WindowMinimizeButtonHint) buttonCount++;

                return opt->rect.adjusted(frameWidth + 4, 0, -buttonCount * buttonSize, 0);
            }
        }
    }

    return QProxyStyle::subControlRect(cc, opt, sc, widget);
}

QStyle::SubControl MdiOfficeStyle::hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                                         const QPoint &pos, const QWidget *widget) const
{
    const QMdiSubWindow *window = qobject_cast<const QMdiSubWindow *>(widget);
    if (cc == CC_TitleBar && widget && window)
    {
        const QRect closeRect = subControlRect(cc, opt, SC_TitleBarCloseButton, widget);
        if (closeRect.isValid() && closeRect.contains(pos))
            return SC_TitleBarCloseButton;

        const QRect maxRect = subControlRect(cc, opt, SC_TitleBarMaxButton, widget);
        if (maxRect.isValid() && maxRect.contains(pos))
            return SC_TitleBarMaxButton;

        if (window->windowState().testFlag(Qt::WindowMinimized))
        {
            const QRect normalRect = subControlRect(cc, opt, SC_TitleBarNormalButton, widget);
            if (normalRect.isValid() && normalRect.contains(pos))
                return SC_TitleBarNormalButton;
        }
        else
        {
            const QRect minRect = subControlRect(cc, opt, SC_TitleBarMinButton, widget);
            if (minRect.isValid() && minRect.contains(pos))
                return SC_TitleBarMinButton;
        }

        return SC_TitleBarLabel;
    }

    return QProxyStyle::hitTestComplexControl(cc, opt, pos, widget);
}

int MdiOfficeStyle::styleHint(QStyle::StyleHint hint, const QStyleOption *option,
                              const QWidget *widget, QStyleHintReturn *returnData) const
{
    if (hint == QStyle::SH_WindowFrame_Mask)
    {
        const int result = QProxyStyle::styleHint(hint, option, widget, returnData);

        if (returnData)
        {
            QStyleHintReturnMask *frameMask = static_cast<QStyleHintReturnMask *>(returnData);
            frameMask->region = QRegion();
        }

        return result;
    }

    if (hint == QStyle::SH_UnderlineShortcut)
        return true;

    return QProxyStyle::styleHint(hint, option, widget, returnData);
}

// ---------------------------------------------------------------------------
// Polish: hover + перерисовка рамок при смене активного окна
// ---------------------------------------------------------------------------

void MdiOfficeStyle::polish(QWidget *widget)
{
    QProxyStyle::polish(widget);

    if (qobject_cast<QMdiSubWindow *>(widget))
        widget->setAttribute(Qt::WA_Hover, true);

    if (QMdiArea *mdiArea = qobject_cast<QMdiArea *>(widget))
    {
        mdiArea->setAttribute(Qt::WA_Hover, true);

        // PE_FrameWindow не перерисовывается сам при смене активного окна —
        // перерисовываем все субокна принудительно
        if (!m_mdiConnections.contains(mdiArea))
        {
            QMetaObject::Connection conn = connect(mdiArea, &QMdiArea::subWindowActivated,
                this, [mdiArea](QMdiSubWindow *)
            {
                const auto subWindows = mdiArea->subWindowList(QMdiArea::CreationOrder);
                for (QMdiSubWindow *subWindow : subWindows)
                    subWindow->update();
            });
            m_mdiConnections.insert(mdiArea, conn);
        }
    }
}

void MdiOfficeStyle::unpolish(QWidget *widget)
{
    if (QMdiArea *mdiArea = qobject_cast<QMdiArea *>(widget))
    {
        auto it = m_mdiConnections.find(mdiArea);
        if (it != m_mdiConnections.end())
        {
            disconnect(it.value());
            m_mdiConnections.erase(it);
        }
    }

    QProxyStyle::unpolish(widget);
}

bool MdiOfficeStyle::isActiveSubWindow(const QMdiSubWindow *subWindow) const
{
    if (!subWindow)
        return false;

    QMdiArea *area = subWindow->mdiArea();
    return area && area->activeSubWindow() == subWindow;
}

// ---------------------------------------------------------------------------
// Офисная палитра (синий порт MDIProxyStyle::standardPalette из FmtLib,
// там основной цвет — зелёный #217346). QApplication::setStyle() забирает
// её при applyToApplication(), поэтому палитру получают и всплывающие
// меню ленты, и прочие top-level попапы
// ---------------------------------------------------------------------------

QPalette MdiOfficeStyle::standardPalette() const
{
    QPalette pal = QProxyStyle::standardPalette(); // начинаем с палитры базового стиля

    const QColor mainColor = mainAppColor();                 // #0072C6
    const QColor lightColor = mainColor.lighter(240);        // светлый акцент
    const QColor lighterColor = mainColor.lighter(280);      // ещё светлее
    const QColor midColor = mainColor.darker(120);           // средний тон
    const QColor darkColor = mainColor.darker(180);          // тёмный тон
    const QColor highlightColor = mainColor;                 // фон выделения
    const QColor highlightedTextColor = Qt::white;           // текст на выделении
    const QColor windowBgColor = QColor(0xF8, 0xF8, 0xF8);   // фон окна
    const QColor windowTextColor = QColor(0x66, 0x66, 0x66); // текст окна
    const QColor buttonBgColor = QColor(0xFF, 0xFF, 0xFF);   // фон кнопки
    const QColor buttonTextColor = QColor(0x33, 0x33, 0x33); // текст кнопки
    const QColor disabledTextColor = QColor(0xB2, 0xB2, 0xB2);

    // --- Основные цвета окна ---
    pal.setColor(QPalette::Window, windowBgColor);
    pal.setColor(QPalette::WindowText, windowTextColor);

    // --- Цвета текста ---
    pal.setColor(QPalette::Text, windowTextColor);
    pal.setColor(QPalette::ButtonText, buttonTextColor);
    pal.setColor(QPalette::PlaceholderText, disabledTextColor);

    // --- Цвета фона ---
    pal.setColor(QPalette::Base, Qt::white);
    pal.setColor(QPalette::AlternateBase, windowBgColor.lighter(110));

    // --- Цвета акцента (Highlight) — в т.ч. фон выделенного пункта меню ---
    pal.setColor(QPalette::Active, QPalette::Highlight, highlightColor);
    pal.setColor(QPalette::Inactive, QPalette::Highlight, inactiveFrameColor());
    pal.setColor(QPalette::Active, QPalette::HighlightedText, highlightedTextColor);
    pal.setColor(QPalette::Inactive, QPalette::HighlightedText, Qt::black);

    // --- Цвета кнопок ---
    pal.setColor(QPalette::Button, buttonBgColor);
    pal.setColor(QPalette::Light, lightColor);
    pal.setColor(QPalette::Mid, midColor);
    pal.setColor(QPalette::Dark, darkColor);

    // --- Неактивное состояние ---
    pal.setColor(QPalette::Inactive, QPalette::Window, windowBgColor);
    pal.setColor(QPalette::Inactive, QPalette::WindowText, windowTextColor.darker(120));
    pal.setColor(QPalette::Inactive, QPalette::Text, windowTextColor.darker(120));
    pal.setColor(QPalette::Inactive, QPalette::ButtonText, buttonTextColor.darker(120));

    // --- Отключённые элементы ---
    pal.setColor(QPalette::Disabled, QPalette::WindowText, disabledTextColor);
    pal.setColor(QPalette::Disabled, QPalette::Text, disabledTextColor);
    pal.setColor(QPalette::Disabled, QPalette::ButtonText, disabledTextColor);
    pal.setColor(QPalette::Disabled, QPalette::Highlight, pal.color(QPalette::Base));
    pal.setColor(QPalette::Disabled, QPalette::HighlightedText, Qt::black);

    // --- Рамки и детали ---
    pal.setColor(QPalette::Midlight, lighterColor);

    return pal;
}
