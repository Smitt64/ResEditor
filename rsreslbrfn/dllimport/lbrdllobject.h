#ifndef LBRDLLOBJECT_H
#define LBRDLLOBJECT_H

#include <lbrobjectinterface.h>
#include <QObject>

class LbrDllObjectPrivate;
class LbrDllObject : public LbrObjectInterface
{
public:
    LbrDllObject(QObject *parent = nullptr);
    virtual ~LbrDllObject();

    Q_INVOKABLE bool open(const QString &filename) Q_DECL_OVERRIDE;
    Q_INVOKABLE bool create(const QString &filename) Q_DECL_OVERRIDE;

    QAbstractItemModel *list() Q_DECL_OVERRIDE;
    void getResource(const QString &name, const int &type, ResBuffer **buffer) Q_DECL_OVERRIDE;
    bool deleteResource(const QString &name, const int &type) Q_DECL_OVERRIDE;
    bool isResExists(const QString &name, const int &type) Q_DECL_OVERRIDE;

    bool beginSaveRes(const QString &name, const int &type, ResBuffer **buffer) Q_DECL_OVERRIDE;
    bool endSaveRes(ResBuffer **buffer) Q_DECL_OVERRIDE;

    QString fileName() const Q_DECL_OVERRIDE;
private:
    Q_DECLARE_PRIVATE(LbrDllObject);
};

#endif // LBRDLLOBJECT_H
