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

    Q_INVOKABLE bool open(const QString &filename) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool create(const QString &filename) Q_DECL_OVERRIDE;

    QAbstractItemModel *list() Q_DECL_OVERRIDE;

    void getResource(const QString &name, const int &type, ResBuffer **buffer) Q_DECL_OVERRIDE;
    bool deleteResource(const QString &name, const int &type) Q_DECL_OVERRIDE;
    bool isResExists(const QString &name, const int &type) Q_DECL_OVERRIDE;

private:
    Q_DECLARE_PRIVATE(LbrObject);
};

RSRESLBRFN_EXPORT void CreateLbrObject(LbrObjectInterface **obj, QObject *parent = nullptr);

#endif // LBROBJECT_H
