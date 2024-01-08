#ifndef LBROBJECT_H
#define LBROBJECT_H

#include "rsreslbrfn_global.h"
#include <QObject>

class ResBuffer;
class LbrObjectPrivate;
class QAbstractItemModel;
class RSRESLBRFN_EXPORT LbrObject : public QObject
{
    Q_OBJECT
public:
    enum ResTypes
    {
        RES_MENU = 1, // Меню
        RES_STAT = 2, // Строка статюса
        RES_DIALOG = 3, // Окно диалог
        RES_HIST = 4, // Строки подсказок
        RES_REPORT = 5, // Отчет для печати
        RES_BFSTRUCT = 6, // Структура Btrieve файла
        RES_DBLINK = 7, // Связь между экранной формой и файлом данных
        RES_PANEL = 8,
        RES_SCROL = 9,
        RES_REP = 10,
        RES_BS = 11,
        RES_LS = 12,
        RES_ACCEL = 13,
        RES_STRTABLE = 14,
        RES_MENU2 = 15,
    };

    LbrObject(QObject *parent = nullptr);
    virtual ~LbrObject();

    Q_INVOKABLE bool open(const QString &filename);
    Q_INVOKABLE bool create(const QString &filename);

    static QString getResTypeName(const int &type);
    QAbstractItemModel *list();

    void getResource(const QString &name, const int &type, ResBuffer **buffer);
    bool deleteResource(const QString &name, const int &type);
    bool isResExists(const QString &name, const int &type);

private:
    LbrObjectPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(LbrObject);
};

RSRESLBRFN_EXPORT void CreateLbrObject(LbrObject **obj, QObject *parent = nullptr);

#endif // LBROBJECT_H
