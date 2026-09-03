#ifndef ICONSELECTDIALOG_H
#define ICONSELECTDIALOG_H

#include "ResMenu_global.h"

#include <QDialog>

class IconBrowser;

// Диалог выбора иконки: показывает IconBrowser, по Ok возвращает
// id выбранной иконки (selectedIconId()). Двойной клик по иконке
// равнозначен нажатию Ok.
class RESMENU_EXPORT IconSelectDialog : public QDialog
{
    Q_OBJECT
public:
    explicit IconSelectDialog(QWidget *parent = nullptr);

    quint32 selectedIconId() const;
    void setCurrentIconId(const quint32 &id);

    // exec-обёртка: возвращает id выбранной иконки, при отмене — current
    static quint32 getIconId(QWidget *parent = nullptr,
                             const quint32 &current = 0,
                             bool *ok = nullptr);

private:
    IconBrowser *m_browser;
};

#endif // ICONSELECTDIALOG_H
