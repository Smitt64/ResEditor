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
    // Плашка под курсором при перетаскивании: иконка элемента (если есть)
    // и его название
    QPixmap dragPixmap(const QModelIndex &index) const;

    ToolBoxDelegate *m_pDelegate;
};

#endif // TOOLBOXTREEVIEW_H
