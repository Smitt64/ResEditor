#ifndef TOOLBOXTREEVIEW_H
#define TOOLBOXTREEVIEW_H

#include <QTreeView>

class ToolBoxDelegate;
class ToolBoxTreeView : public QTreeView
{
    Q_OBJECT
public:
    ToolBoxTreeView(QWidget *parent = nullptr);
    virtual ~ToolBoxTreeView();

protected:
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;

private:
    ToolBoxDelegate *m_pDelegate;
};

#endif // TOOLBOXTREEVIEW_H
