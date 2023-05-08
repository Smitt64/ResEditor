#include "lbrobject.h"
#include "lbrobjectprivate.h"
#include "lbrreslistmodel.h"
#include "resbuffer.h"
#include <QDebug>

void CreateLbrObject(LbrObject **obj, QObject *parent)
{
    *obj = new LbrObject(parent);
}

LbrObject::LbrObject(QObject *parent)
    : QObject{parent},
      d_ptr(new LbrObjectPrivate())
{
    Q_D(LbrObject);
    d->q_ptr = this;
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

bool LbrObject::isResExists(const QString &name, const int &type)
{
    Q_D(LbrObject);
    return d->GetResource(name, type) != nullptr;
}

QString LbrObject::getResTypeName(const int &type)
{
    QString val;
    switch(type)
    {
    case RES_MENU:
        val = "MENU";
        break;
    case RES_STAT:
        val = "STAT";
        break;
    case RES_DIALOG:
        val = "DIALOG";
        break;
    case RES_HIST:
        val = "HIST";
        break;
    case RES_REPORT:
        val = "REPORT";
        break;
    case RES_BFSTRUCT:
        val = "BFSTRUCT";
        break;
    case RES_DBLINK:
        val = "DBLINK";
        break;
    case RES_PANEL:
        val = "PANEL";
        break;
    case RES_SCROL:
        val = "SCROL";
        break;
    case RES_REP:
        val = "REP";
        break;
    case RES_BS:
        val = "BSCROL";
        break;
    case RES_LS:
        val = "LS";
        break;
    case RES_ACCEL:
        val = "ACCEL";
        break;
    case RES_STRTABLE:
        val = "STRTABLE";
        break;
    case RES_MENU2:
        val = "MENU2";
        break;
    }
    return val;
}
