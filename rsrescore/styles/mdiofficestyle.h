#ifndef MDIOFFICESTYLE_H
#define MDIOFFICESTYLE_H

#include "../rsrescore_global.h"

#include <QProxyStyle>
#include <QIcon>
#include <QMetaObject>

class QMdiArea;
class QMdiSubWindow;
class QStyleOptionButton;
class QStyleOptionTitleBar;

// Стилизация дочерних MDI-окон под "голубой офис" — синий порт
// MDIProxyStyle из FmtLib (основной цвет #0072C6).
//
// Два сценария применения:
//  * applyToApplication() — на всё приложение (qApp->setStyle), тогда
//    офисную палитру (standardPalette) получают и всплывающие меню ленты:
//    попапы — top-level окна, stylesheet главного окна их не достаёт
//    (аналог FmtRibbonMainWindow::ApplyRibbonProxy из FmtLib);
//  * inst() + setStyle() на конкретные QMdiArea/QMdiSubWindow (так делает
//    редактор меню для своего внутреннего MDI).
class RSRESCORE_EXPORT MdiOfficeStyle : public QProxyStyle
{
    Q_OBJECT
public:
    explicit MdiOfficeStyle(QStyle *style = nullptr);

    // Общий экземпляр: прокси не должен удаляться, пока установлен
    // на виджетах, поэтому живёт всё время работы приложения
    static MdiOfficeStyle *inst();

    // Установить офисный стиль на всё приложение. Вызывать после создания
    // QApplication и главного окна; экземпляр утекает сознательно —
    // он должен жить до конца работы приложения
    static void applyToApplication();

    void drawPrimitive(PrimitiveElement element, const QStyleOption *option,
                       QPainter *painter, const QWidget *widget = nullptr) const override;
    void drawComplexControl(ComplexControl control, const QStyleOptionComplex *option,
                            QPainter *painter, const QWidget *widget = nullptr) const override;

    QSize sizeFromContents(ContentsType type, const QStyleOption *option,
                           const QSize &size, const QWidget *widget = nullptr) const override;
    int pixelMetric(PixelMetric metric, const QStyleOption *option,
                    const QWidget *widget = nullptr) const override;
    QRect subControlRect(ComplexControl cc, const QStyleOptionComplex *opt,
                         SubControl sc, const QWidget *widget) const override;
    SubControl hitTestComplexControl(ComplexControl cc, const QStyleOptionComplex *opt,
                                     const QPoint &pos, const QWidget *widget) const override;
    int styleHint(QStyle::StyleHint hint, const QStyleOption *option = nullptr,
                  const QWidget *widget = nullptr, QStyleHintReturn *returnData = nullptr) const override;

    void polish(QWidget *widget) override;
    void unpolish(QWidget *widget) override;

    // Офисная палитра — подхватывается QApplication::setStyle() при
    // applyToApplication() и раскрашивает все попапы/меню приложения
    QPalette standardPalette() const override;

    QColor mainAppColor() const { return QColor(0x00, 0x72, 0xC6); }       // #0072C6
    QColor activeFrameColor() const { return mainAppColor(); }             // #0072C6
    QColor inactiveFrameColor() const { return QColor(0xC5, 0xD2, 0xE0); } // #c5d2e0
    QColor titleBarActive() const { return QColor(0xFF, 0xFF, 0xFF); }     // белый
    QColor titleBarInactive() const { return QColor(0xF8, 0xF8, 0xF8); }   // #f8f8f8
    QColor titleTextActive() const { return QColor(0x00, 0x72, 0xC6); }    // #0072C6
    QColor titleTextInactive() const { return QColor(0x66, 0x66, 0x66); }  // #666666
    QColor borderColor() const { return QColor(0xC5, 0xD2, 0xE0); }        // #c5d2e0
    QColor buttonHoverColor() const { return QColor(0xDE, 0xEB, 0xF7); }   // #DEEBF7
    QColor buttonPressColor() const { return QColor(0xBD, 0xD7, 0xEE); }   // #BDD7EE
    QColor closeButtonHover() const { return QColor(0xE8, 0x11, 0x23); }   // #e81123
    QColor closeButtonPress() const { return QColor(0xC8, 0x00, 0x14); }   // #c80014

private:
    void loadIcons();

    void drawMdiSubWindowFrame(const QStyleOption *option,
                               QPainter *painter, const QWidget *widget) const;
    void drawMdiSubWindowTitleBar(const QStyleOptionComplex *option,
                                  QPainter *painter, const QWidget *widget) const;
    void drawTitleBarButtons(const QStyleOptionTitleBar *option,
                             QPainter *painter, const QWidget *widget) const;
    void drawTitleBarButton(const QStyleOptionTitleBar *option,
                            QPainter *painter, const QWidget *widget,
                            SubControl control, const QIcon &icon) const;

    bool isActiveSubWindow(const QMdiSubWindow *subWindow) const;

    QIcon closeIcon;
    QIcon minimizeIcon;
    QIcon maximizeIcon;
    QIcon restoreIcon;

    QHash<QMdiArea *, QMetaObject::Connection> m_mdiConnections;
};

#endif // MDIOFFICESTYLE_H
