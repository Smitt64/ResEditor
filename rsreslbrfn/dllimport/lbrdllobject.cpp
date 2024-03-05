#include "lbrdllobject.h"
#include "lbrdllobjectprivate.h"
#include <QDebug>
#include <lbrreslistmodel.h>

LbrDllObject::LbrDllObject(QObject *parent)
    : LbrObjectInterface(new LbrDllObjectPrivate(this), parent)
{

}

LbrDllObject::~LbrDllObject()
{

}

bool LbrDllObject::open(const QString &filename)
{
    Q_D(LbrDllObject);
    bool hr = d->loadLib();

    if (hr)
        hr = d->open(filename);

    if (hr)
        d->m_FileName = filename;

    return hr;
}

bool LbrDllObject::create(const QString &filename)
{
    return false;
}

QAbstractItemModel *LbrDllObject::list()
{
    Q_D(LbrDllObject);
    return d->m_pDirModel.data();
}

void LbrDllObject::getResource(const QString &name, const int &type, ResBuffer **buffer)
{
    Q_D(LbrDllObject);
    d->getResource(name, type, buffer);
}

bool LbrDllObject::deleteResource(const QString &name, const int &type)
{
    Q_D(LbrDllObject);
    return d->deleteResource(name, type);
}

bool LbrDllObject::isResExists(const QString &name, const int &type)
{
    Q_D(LbrDllObject);
    return d->isResExists(name, type);
}

bool LbrDllObject::beginSaveRes(const QString &name, const int &type, ResBuffer **buffer)
{
    Q_D(LbrDllObject);
    return d->beginSaveRes(name, type, buffer);
}

bool LbrDllObject::endSaveRes(ResBuffer **buffer)
{
    Q_D(LbrDllObject);
    return d->endSaveRes(buffer);
}

QString LbrDllObject::fileName() const
{
    Q_D(const LbrDllObject);
    return d->m_FileName;
}
