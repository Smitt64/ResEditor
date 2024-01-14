#ifndef LBROBJECTINTERFACEPRIVATE_H
#define LBROBJECTINTERFACEPRIVATE_H

#include "lbrobjectinterface.h"

class LbrObjectInterfacePrivate
{
    Q_DECLARE_PUBLIC(LbrObjectInterface)
public:
    LbrObjectInterfacePrivate(LbrObjectInterface *obj);
    virtual ~LbrObjectInterfacePrivate();

    LbrObjectInterface *q_ptr;
    QTextCodec *m_p866;
};

#endif // LBROBJECTINTERFACEPRIVATE_H
