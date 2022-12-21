#ifndef TOOLBOXTREEVIEW_H
#define TOOLBOXTREEVIEW_H

#include <QTreeView>

class ToolBoxTreeView : public QTreeView
{
    Q_OBJECT
public:
    ToolBoxTreeView(QWidget *parent = nullptr);
    virtual ~ToolBoxTreeView();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
};

#endif // TOOLBOXTREEVIEW_H
