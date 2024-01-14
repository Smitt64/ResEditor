#include "lbrobjectinterfaceprivate.h"
#include <QTextCodec>

LbrObjectInterfacePrivate::LbrObjectInterfacePrivate(LbrObjectInterface *obj)
    :q_ptr(obj)
{
    m_p866 = QTextCodec::codecForName("IBM 866");
}

LbrObjectInterfacePrivate::~LbrObjectInterfacePrivate()
{

}
