#include "lbrobject.h"
#include "lbrobjectprivate.h"
#include "lbrreslistmodel.h"
#include "resbuffer.h"
#include <QDebug>

void CreateLbrObject(LbrObjectInterface **obj, QObject *parent)
{
    *obj = new LbrObject(parent);
}

LbrObject::LbrObject(QObject *parent)
    : LbrObjectInterface(new LbrObjectPrivate(this), parent)
{
}

LbrObject::~LbrObject()
{

}

bool LbrObject::open(const QString &filename)
{
    Q_D(LbrObject);
    return d->Open(filename);
}

bool LbrObject::create(const QString &filename)
{
    Q_D(LbrObject);
    return d->Create(filename);
}

QAbstractItemModel *LbrObject::list()
{
    Q_D(LbrObject);
    return d->m_pDirModel.data();
}

void LbrObject::getResource(const QString &name, const int &type, ResBuffer **buffer)
{
    Q_D(LbrObject);
    QString comment;
    ResHeader header;
    QByteArray data = d->GetResourceData(name, type, &header, &comment);
    *buffer = new ResBuffer(this, &data, name, type);
    (*buffer)->setResHeader(&header);
    (*buffer)->setComment(comment);
    (*buffer)->open(QIODevice::ReadOnly);

    //qDebug() << "getResource" << (*buffer)->buffer().size();
}

bool LbrObject::deleteResource(const QString &name, const int &type)
{
    Q_D(LbrObject);

    bool hr = true;
    LibElem  rc;
    strncpy_s(rc.name, name.toLocal8Bit().data(), RESNAMELEN - 1);
    rc.name[RESNAMELEN - 1] = '\0';
    _strupr_s(rc.name);
    rc.type = type;

    hr = !d->ResDelObject(&rc.rc);

    return hr;
}

bool LbrObject::isResExists(const QString &name, const int &type)
{
    Q_D(LbrObject);
    return d->GetResource(name, type) != nullptr;
}
