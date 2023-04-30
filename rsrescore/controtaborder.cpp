#include "controtaborder.h"
#include "qjsonobject.h"

ControTabOrder::ControTabOrder(QObject *parent)
    : QObject{parent},
      m_Up(0),
      m_Bottom(0),
      m_This(0),
      m_Previous(0),
      m_Next(0)
{
}

ControTabOrder::ControTabOrder(const ControTabOrder &other):
    QObject()
{
    fromOther(other);
}

ControTabOrder::~ControTabOrder()
{

}

void ControTabOrder::fromOther(const ControTabOrder &other)
{
    m_Up = other.m_Up;
    m_Bottom = other.m_Bottom;
    m_This = other.m_This;
    m_Previous = other.m_Previous;
    m_Next = other.m_Next;
}

bool ControTabOrder::operator ==(const ControTabOrder& other) const
{
    return m_Up == other.m_Up && m_Bottom == other.m_Bottom &&
            m_This == other.m_This && m_Previous == other.m_Previous &&
            m_Next == other.m_Next;
}

ControTabOrder& ControTabOrder::operator=(const ControTabOrder& other)
{
    fromOther(other);
    return *this;
}

QJsonObject ControTabOrder::toJsonObject(const ControTabOrder &order)
{
    QJsonObject obj;
    obj.insert("up", (qint16)order.m_Up);
    obj.insert("bottom", (qint16)order.m_Bottom);
    obj.insert("this", (qint16)order.m_This);
    obj.insert("previous", (qint16)order.m_Previous);
    obj.insert("next", (qint16)order.m_Next);
    return obj;
}

ControTabOrder ControTabOrder::fromJsonObject(const QJsonObject &obj)
{
    ControTabOrder order;
    using T = decltype(m_Up);

    order.m_Up = static_cast<T>(obj["up"].toInt());
    order.m_Bottom = static_cast<T>(obj["bottom"].toInt());
    order.m_This = static_cast<T>(obj["this"].toInt());
    order.m_Previous = static_cast<T>(obj["previous"].toInt());
    order.m_Next = static_cast<T>(obj["next"].toInt());

    return order;
}

const quint8 &ControTabOrder::thisid() const
{
    return m_This;
}

void ControTabOrder::setThisId(const quint8 &val)
{
    m_This = val;
    emit thisidChanged();
}

const quint8 &ControTabOrder::up() const
{
    return m_Up;
}

void ControTabOrder::setUp(const quint8 &val)
{
    m_Up = val;
    emit upChanged();
}

const quint8 &ControTabOrder::bottom() const
{
    return m_Bottom;
}

void ControTabOrder::setBottom(const quint8 &val)
{
    m_Bottom = val;
    emit bottomChanged();
}

const quint8 &ControTabOrder::previous() const
{
    return m_Previous;
}

void ControTabOrder::setPrevious(const quint8 &val)
{
    m_Previous = val;
    emit previousChanged();
}

const quint8 &ControTabOrder::next() const
{
    return m_Next;
}

void ControTabOrder::setNext(const quint8 &val)
{
    m_Next = val;
    emit nextChanged();
}

QDebug operator<< (QDebug d, const ControTabOrder &model)
{
    d << QString("ControTabOrder(this: %1, up: %2, left: %3, right: %4, bottom: %5]")
         .arg(model.thisid())
         .arg(model.up())
         .arg(model.previous())
         .arg(model.next())
         .arg(model.bottom());

    return d;
}
