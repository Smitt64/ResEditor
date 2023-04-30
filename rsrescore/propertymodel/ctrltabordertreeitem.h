#ifndef CTRLTABORDERTREEITEM_H
#define CTRLTABORDERTREEITEM_H

#include "propertytreeitem.h"
#include <QObject>

class ControTabOrder;
class CtrlTabOrderTreeItem : public PropertyTreeItem
{
    Q_OBJECT
public:
    Q_INVOKABLE CtrlTabOrderTreeItem(QObject *rectItem, QObject *parent = nullptr);
    virtual ~CtrlTabOrderTreeItem();

    //virtual Qt::ItemFlags flags() const Q_DECL_OVERRIDE;
    virtual QVariant data(const int &role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual bool setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QSize sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;

public slots:
    virtual void setPropertyName(const QString &name) Q_DECL_OVERRIDE;

private slots:
    void itemTabOrderChanged();

private:
    void propertyChanged();

private:
    ControTabOrder *m_pTabOrder;
};

Q_DECLARE_OPAQUE_POINTER(CtrlTabOrderTreeItem)

#endif // CTRLTABORDERTREEITEM_H
