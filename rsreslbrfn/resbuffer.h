#ifndef RESBUFFER_H
#define RESBUFFER_H

#include <QBuffer>
#include "rsreslbrfn_global.h"

class LbrObjectInterface;
class ResBufferPrivate;
class RSRESLBRFN_EXPORT ResBuffer : public QBuffer
{
    friend class LbrObject;
    friend class LbrObjectInterface;
public:
    virtual ~ResBuffer();

    int headerVersion() const;
    int version() const;
    void setResVersion(const int &version);

    QString decodeString(const char *str) const;
    void encodeString(const QString &str, char *encStr, int len) const;

    bool writeString(const QString &str);

    const QString &name() const;
    const QString &comment() const;
    const qint16 &type() const;

    void setComment(const QString &comment);
    void setResHeader(void *ptr);
    QDateTime getResDateTime() const;

    void setResStream(Qt::HANDLE handle);
    Qt::HANDLE getResStream();

    void debugSaveToFile(const QString &filename);
    static void debugSaveToFile(const QString &filename, char *mem, int len);

private:
    ResBufferPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ResBuffer);

    ResBuffer(LbrObjectInterface *pLbrObj, QByteArray *data, const QString &name, const qint16 &type, const QString &comment = QString());
};

#endif // RESBUFFER_H
