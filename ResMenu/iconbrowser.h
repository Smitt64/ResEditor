#ifndef ICONBROWSER_H
#define ICONBROWSER_H

#include "ResMenu_global.h"

#include <QWidget>

class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QComboBox;

// Просмотрщик иконок из ресурсной DLL, загруженных ResMenu.
// Иконки — RT_BITMAP (тип 2), id ресурса совпадает с enum ICON_<id>;
// вариант 24px хранится под id + 2000 и показывается предпочтительно.
// Теги/названия для фильтрации берутся из ResMenu (:/json/IconTags.json).
class RESMENU_EXPORT IconBrowser : public QWidget
{
    Q_OBJECT
public:
    explicit IconBrowser(QWidget *parent = nullptr);

    // id выделенной иконки (0 — ничего не выбрано)
    quint32 selectedIconId() const;
    void setCurrentIconId(const quint32 &id);

signals:
    void iconActivated(quint32 id);      // двойной клик / Enter по иконке
    void currentIconChanged(quint32 id); // смена выделения

private slots:
    void applyFilter(const QString &text);
    void applyZoom(int index);

private:
    void loadIcons();
    quint32 itemIconId(QListWidgetItem *item) const;

    QLabel      *m_info;
    QLineEdit   *m_filter;
    QComboBox   *m_zoom;
    QListWidget *m_list;
};

#endif // ICONBROWSER_H
