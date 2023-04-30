#ifndef CONTROTABORDER_H
#define CONTROTABORDER_H

#include <QObject>
#include <QDebug>

class ControTabOrder : public QObject
{
    Q_OBJECT
    Q_PROPERTY(quint8 thisid READ thisid WRITE setThisId NOTIFY thisidChanged)
    Q_PROPERTY(quint8 up READ up WRITE setUp NOTIFY upChanged)
    Q_PROPERTY(quint8 bottom READ up WRITE setBottom NOTIFY bottomChanged)
    Q_PROPERTY(quint8 previous READ previous WRITE setPrevious NOTIFY previousChanged)
    Q_PROPERTY(quint8 next READ next WRITE setNext NOTIFY nextChanged)
public:
    Q_INVOKABLE ControTabOrder(QObject *parent = nullptr);
    ControTabOrder(const ControTabOrder &other);
    virtual ~ControTabOrder();

    ControTabOrder& operator=(const ControTabOrder& other);
    bool operator ==(const ControTabOrder& other) const;

    static QJsonObject toJsonObject(const ControTabOrder &order);
    static ControTabOrder fromJsonObject(const QJsonObject &obj);

    void fromOther(const ControTabOrder &other);

    const quint8 &thisid() const;
    void setThisId(const quint8 &val);

    const quint8 &up() const;
    void setUp(const quint8 &val);

    const quint8 &bottom() const;
    void setBottom(const quint8 &val);

    const quint8 &previous() const;
    void setPrevious(const quint8 &val);

    const quint8 &next() const;
    void setNext(const quint8 &val);

signals:
    void thisidChanged();
    void upChanged();
    void bottomChanged();
    void previousChanged();
    void nextChanged();

private:
    quint8 m_Up, m_Bottom, m_This;
    quint8 m_Previous, m_Next;
};

QDebug operator<< (QDebug d, const ControTabOrder &model);

template<class T> bool operator ==(const T& a1, const ControTabOrder& a2)
{
    if (std::is_same_v<T, ControTabOrder>)
        return a2.operator ==(a1);

    return false;
}

template<class T>bool operator <(const T& a1, const ControTabOrder& a2)
{
    Q_UNUSED(a1)
    Q_UNUSED(a2)
    return false;
}

Q_DECLARE_METATYPE(ControTabOrder)

#endif // CONTROTABORDER_H
