#ifndef PROPERTYTREEDELEGATE_H
#define PROPERTYTREEDELEGATE_H

#include <QStyledItemDelegate>

class PropertyTreeDelegate : public QStyledItemDelegate
{
public:
    PropertyTreeDelegate(QObject *parent = nullptr);
    virtual ~PropertyTreeDelegate();

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual QWidget *createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setEditorData(QWidget *editor, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const Q_DECL_OVERRIDE;
};

#endif // PROPERTYTREEDELEGATE_H
