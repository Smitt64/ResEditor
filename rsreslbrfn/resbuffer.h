#ifndef RESBUFFER_H
#define RESBUFFER_H

#include <QBuffer>
#include "rsreslbrfn_global.h"

class LbrObject;
class ResBufferPrivate;
class RSRESLBRFN_EXPORT ResBuffer : public QBuffer
{
    friend class LbrObject;
public:
    virtual ~ResBuffer();

    int headerVersion() const;
    int version() const;
    void setResVersion(const int &version);

    QString decodeString(const char *str) const;
    void encodeString(const QString &str, char *encStr, int len) const;
    const QString &name() const;
    const QString &comment() const;
    const qint16 &type() const;

    void setComment(const QString &comment);

    void debugSaveToFile(const QString &filename);

private:
    ResBufferPrivate * const d_ptr;
    Q_DECLARE_PRIVATE(ResBuffer);

    ResBuffer(LbrObject *pLbrObj, QByteArray *data, const QString &name, const qint16 &type, const QString &comment = QString());
    void setResHeader(void *ptr);
};

#endif // RESBUFFER_H
