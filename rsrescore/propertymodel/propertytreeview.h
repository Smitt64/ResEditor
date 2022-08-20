#ifndef PROPERTYTREEVIEW_H
#define PROPERTYTREEVIEW_H

#include <QTreeView>

class PropertyTreeDelegate;
class PropertyTreeView : public QTreeView
{
public:
    PropertyTreeView(QWidget *parent = nullptr);
    virtual ~PropertyTreeView();

protected:
    virtual void drawRow(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_OVERRIDE;
    virtual void drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const Q_DECL_OVERRIDE;

private:
    PropertyTreeDelegate *m_pDelegate;
};

#endif // PROPERTYTREEVIEW_H
