#ifndef RESMENU_H
#define RESMENU_H

#include "ResMenu_global.h"
#include <QMap>
#include <QString>
#include <QStringList>
#include <QScopedPointer>

class QIcon;
class QPixmap;
class ResMenuPrivate;
class RESMENU_EXPORT ResMenu
{
    Q_DECLARE_PRIVATE(ResMenu)
public:
    ResMenu(const ResMenu&) = delete;
    ResMenu& operator=(const ResMenu&) = delete;

    static ResMenu *inst();

    QPixmap getResPixMap(const qint32 &id);
    QIcon getResIcon(const qint32 &id);

    // Все загруженные из ресурсной DLL иконки (id -> pixmap,
    // 24px-варианты хранятся под id + 2000)
    QMap<quint32, QPixmap> resPixMaps() const;
    QString resDllPath() const;

    // Теги и названия пунктов из :/json/IconTags.json
    QStringList resIconTags(const qint32 &id) const;
    QStringList resIconTitles(const qint32 &id) const;
    int resIconTagsCount() const; // 0 — теги не загружены

private:
    ResMenu();
    ~ResMenu();

    static ResMenu *m_pInstance;
    QScopedPointer<ResMenuPrivate> d_ptr;
};

#endif // RESMENU_H