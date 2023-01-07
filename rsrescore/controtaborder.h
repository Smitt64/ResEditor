#ifndef CONTROTABORDER_H
#define CONTROTABORDER_H

#include <QObject>

class ControTabOrder : public QObject
{
    Q_OBJECT
public:
    Q_INVOKABLE ControTabOrder(QObject *parent = nullptr);
    ControTabOrder(const ControTabOrder &other);
    virtual ~ControTabOrder();

    ControTabOrder& operator=(const ControTabOrder& other);
    bool operator ==(const ControTabOrder& other) const;

    static QJsonObject toJsonObject(const ControTabOrder &order);
    static ControTabOrder fromJsonObject(const QJsonObject &obj);

signals:

private:
    void fromOther(const ControTabOrder &other);
    quint8 m_Up, m_Bottom, m_This;
    quint8 m_Previous, m_Next;
};

template<class T> bool operator ==(const T& a1, const ControTabOrder& a2)
{
    if (std::is_same_v<T, ControTabOrder>)
        return a2.operator ==(a1);

    return false;
}

template<class T>bool operator <(const T& a1, const ControTabOrder& a2)
{
    return false;
}

Q_DECLARE_METATYPE(ControTabOrder)

#endif // CONTROTABORDER_H
