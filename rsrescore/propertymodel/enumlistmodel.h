#ifndef ENUMLISTMODEL_H
#define ENUMLISTMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QJsonArray>
#include <QMetaEnum>

class EnumListModel : public QAbstractListModel
{
    typedef struct EnumlistElement
    {
        QString key, alias;
    }EnumlistElement;
public:
    enum CustomRoles
    {
        RoleKey = Qt::UserRole + 1,
    };
    explicit EnumListModel(QObject *parent = nullptr);
    virtual ~EnumListModel();

    void loadFromJsonArray(const QJsonArray &array);
    void loadFromJsonFile(const QString &filename);

    QString alias(const QString &key) const;
    QString key(const QString &alias) const;

    void setMetaEnum(const QMetaEnum &menum);
    void setMetaEnum(QObject *obj, const QString &property);

    int keyToValue(const QString &key) const;
    QString valueToAlias(const int &value) const;
    QString valueToKey(const int &value) const;

    int indexFromValue(const int &value) const;
    int valueFromIndex(const int &index) const;

    virtual int rowCount(const QModelIndex &parent = QModelIndex()) const Q_DECL_OVERRIDE;
    virtual QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;
private:
    QList<EnumlistElement> m_EnumKey;
    QMetaEnum m_MetaEnum;
};

#endif // ENUMLISTMODEL_H
