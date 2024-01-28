#include "lbrobjectinterface.h"
#include "lbrobjectinterfaceprivate.h"
#include "resbuffer.h"

LbrObjectInterface::LbrObjectInterface(QObject *parent)
    : QObject(parent),
    d_ptr(new LbrObjectInterfacePrivate(this))
{

}

LbrObjectInterface::LbrObjectInterface(LbrObjectInterfacePrivate *dd, QObject *parent)
    : QObject(parent),
    d_ptr(dd)
{

}

LbrObjectInterface::~LbrObjectInterface()
{

}

void LbrObjectInterface::createResBuffer(const QString &name, const int &type, QByteArray *data, ResBuffer **buffer)
{
    *buffer = new ResBuffer(this, data, name, type);
}

void LbrObjectInterface::createResBuffer(const QString &name, const int &type, ResBuffer **buffer)
{
    QByteArray data;
    *buffer = new ResBuffer(this, &data, name, type);
}

QString LbrObjectInterface::getResTypeName(const int &type)
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
