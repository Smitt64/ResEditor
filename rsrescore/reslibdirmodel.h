#ifndef RESLIBDIRMODEL_H
#define RESLIBDIRMODEL_H

#include "rsrescore_global.h"
#include <QAbstractTableModel>
#include <QSet>

typedef struct
{
    quint16 Type;
    QString name, comment;
}tResLibDirElement;
typedef QSet<quint16> ResTypesSet;

class RSRESCORE_EXPORT ResLibDirModel : public QAbstractTableModel
{
    Q_OBJECT
    enum
    {
        fldName,
        fldType,
        fldComment
    };
public:
    explicit ResLibDirModel(QObject *parent = nullptr);
    virtual ~ResLibDirModel();

    virtual int columnCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    virtual QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
    void addElement(const quint16 &Type, const QString &name, const QString &comment);

    const tResLibDirElement &element(const QModelIndex &index) const;
    const tResLibDirElement &element(const quint16 &index) const;

    const ResTypesSet &resTypes() const;
private:
    QVector<tResLibDirElement> m_Elements;
    ResTypesSet m_ResTypes;
};

#endif // RESLIBDIRMODEL_H
