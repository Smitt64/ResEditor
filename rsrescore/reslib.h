#ifndef RESLIB_H
#define RESLIB_H

#include "rsrescore_global.h"
#include <QObject>
#include <QLibrary>

// Types of resources
#define  RES_MENU         1    // Меню
#define  RES_STAT         2    // Строка статюса
#define  RES_DIALOG       3    // Окно диалог
#define  RES_HIST         4    // Строки подсказок
#define  RES_REPORT       5    // Отчет для печати
#define  RES_BFSTRUCT     6    // Структура Btrieve файла
#define  RES_DBLINK       7    // Связь между экранной формой и файлом данных

#define  RES_PANEL        8
#define  RES_SCROL        9
#define  RES_REP         10
#define  RES_BS          11
#define  RES_LS          12
#define  RES_ACCEL       13
#define  RES_STRTABLE    14

#define  RES_MENU2       15

#define  RES_LAST       RES_MENU2

class ResLibDirModel;
class RsResInterface;
class QAbstractItemModel;
class RSRESCORE_EXPORT ResLib : public QObject
{
    Q_OBJECT
public:
    explicit ResLib(QObject *parent = nullptr);
    virtual ~ResLib();
    bool loadResLibDll();
    bool open(const QString &resFile);

    QString resError(const int &stat);
    QString decodeString(const char *str);

    ResLibDirModel *model();

    void resGetCurHeader(void **resh);
    int resHeaderVersion();
    int readBytes(void *ptr, quint64 len);
    int fileHandle() const;

    int loadResource(const QString &name, const qint16 &type, RsResInterface *pCallback);

private:
    void fillDir(void *resLbr, ResLibDirModel *model);
    QLibrary m_ResLib;
    QFunctionPointer pResOpen, pResError, pBuildDirectory, pFreeDirectory, pResGetObject, pResClose;

    ResLibDirModel *m_pModel;
    qintptr m_ResPtr;
};

#endif // RESLIB_H
