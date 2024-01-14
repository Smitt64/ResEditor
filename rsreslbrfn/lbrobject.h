#ifndef LBROBJECT_H
#define LBROBJECT_H

#include "lbrobjectinterface.h"
#include <QObject>

class ResBuffer;
class LbrObjectPrivate;
class QAbstractItemModel;

class RSRESLBRFN_EXPORT LbrObject : public LbrObjectInterface
{
    Q_OBJECT
public:
    LbrObject(QObject *parent = nullptr);
    virtual ~LbrObject();

    Q_INVOKABLE bool open(const QString &filename);
    Q_INVOKABLE bool create(const QString &filename);

    static QString getResTypeName(const int &type);
    QAbstractItemModel *list();

    void getResource(const QString &name, const int &type, ResBuffer **buffer);
    bool deleteResource(const QString &name, const int &type);
    bool isResExists(const QString &name, const int &type);

private:
    Q_DECLARE_PRIVATE(LbrObject);
};

RSRESLBRFN_EXPORT void CreateLbrObject(LbrObjectInterface **obj, QObject *parent = nullptr);

#endif // LBROBJECT_H
