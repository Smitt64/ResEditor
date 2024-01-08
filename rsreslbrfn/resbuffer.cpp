#include "resbuffer.h"
#include "qtextcodec.h"
#include "LbrObjectHeaders.h"
#include <QFile>
#include <QByteArray>

class ResBufferPrivate
{
    Q_DECLARE_PUBLIC(ResBuffer);
public:
    ResBufferPrivate()
    {
        memset(&m_ResHeader, 0, sizeof(ResHeader));
    }

    ResBuffer * q_ptr;
    LbrObject *m_pObject;
    QByteArray m_Buffer;

    ResHeader m_ResHeader;

    QString m_Name, m_Comment;
    qint16 m_Type;
};

ResBuffer::ResBuffer(LbrObject *pLbrObj, QByteArray *data, const QString &name, const qint16 &type, const QString &comment) :
    QBuffer(),
    d_ptr(new ResBufferPrivate())
{
    Q_D(ResBuffer);
    d->q_ptr = this;
    d->m_pObject = pLbrObj;
    d->m_Buffer = *data;
    d->m_Name = name;
    d->m_Type = type;
    d->m_Comment = comment;
    setBuffer(&d->m_Buffer);
}

ResBuffer::~ResBuffer()
{

}

void ResBuffer::setResHeader(void *ptr)
{
    Q_D(ResBuffer);
    memcpy(&d->m_ResHeader, ptr, sizeof(ResHeader));
}

int ResBuffer::headerVersion() const
{
    Q_D(const ResBuffer);
    return d->m_ResHeader.ver;
}

int ResBuffer::version() const
{
    Q_D(const ResBuffer);
    return d->m_ResHeader.ver;
}

void ResBuffer::setResVersion(const int &version)
{
    Q_D(ResBuffer);
    d->m_ResHeader.ver = version;
}

QString ResBuffer::decodeString(const char *str) const
{
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    return codec->toUnicode(str);
}

void ResBuffer::encodeString(const QString &str, char *encStr, int len) const
{
    QTextCodec *codec = QTextCodec::codecForName("IBM 866");
    QByteArray tmp = codec->fromUnicode(str);
    strcpy_s(encStr, len, tmp.data());
}

const QString &ResBuffer::name() const
{
    Q_D(const ResBuffer);
    return d->m_Name;
}

const QString &ResBuffer::comment() const
{
    Q_D(const ResBuffer);
    return d->m_Comment;
}

const qint16 &ResBuffer::type() const
{
    Q_D(const ResBuffer);
    return d->m_Type;
}

void ResBuffer::setComment(const QString &comment)
{
    Q_D(ResBuffer);
    d->m_Comment = comment;
}

void ResBuffer::debugSaveToFile(const QString &filename)
{
    QFile f(filename);

    if (f.open(QIODevice::WriteOnly))
        f.write(data());
    f.close();
}
