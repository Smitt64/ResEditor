#ifndef LBROBJECTINTERFACE_H
#define LBROBJECTINTERFACE_H

#include "rsreslbrfn_global.h"
#include <QObject>

class ResBuffer;
class QAbstractItemModel;
class LbrObjectInterfacePrivate;
class RSRESLBRFN_EXPORT LbrObjectInterface : public QObject
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

    LbrObjectInterface(QObject *parent = nullptr);
    LbrObjectInterface(LbrObjectInterfacePrivate *dd, QObject *parent = nullptr);
    virtual ~LbrObjectInterface();

    Q_INVOKABLE virtual bool open(const QString &filename) = 0;
    Q_INVOKABLE virtual bool create(const QString &filename) = 0;

    virtual QString fileName() const;

    static QString getResTypeName(const int &type);
    virtual QAbstractItemModel *list() = 0;

    virtual void getResource(const QString &name, const int &type, ResBuffer **buffer) = 0;
    virtual bool deleteResource(const QString &name, const int &type) = 0;
    virtual bool isResExists(const QString &name, const int &type) = 0;

    void createResBuffer(const QString &name, const int &type, QByteArray *data, ResBuffer **buffer);
    void createResBuffer(const QString &name, const int &type, ResBuffer **buffer);

    virtual bool beginSaveRes(const QString &name, const int &type, ResBuffer **buffer) = 0;
    virtual bool endSaveRes(ResBuffer **buffer) = 0;
signals:

protected:
    LbrObjectInterfacePrivate * const d_ptr;

private:
    Q_DECLARE_PRIVATE(LbrObjectInterface);
};

#endif // LBROBJECTINTERFACE_H
