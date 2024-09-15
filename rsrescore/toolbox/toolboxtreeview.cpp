#include "toolboxtreeview.h"
#include "qmimedata.h"
#include "toolbox/toolboxmodel.h"
#include <QMouseEvent>
#include <QDrag>
#include <QGuiApplication>
#include <QIcon>
#include <QDebug>
#include <QStyledItemDelegate>
#include <QApplication>
#include <QStyleOptionToolBox>

class ToolBoxDelegate : public QStyledItemDelegate
{
public:
    ToolBoxDelegate(QObject *parent = nullptr) :
        QStyledItemDelegate(parent)
    {
    }

    virtual ~ToolBoxDelegate()
    {

    }

    virtual void paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const Q_DECL_FINAL
    {
        if (!index.parent().isValid())
        {
            QStyle *m_pStyle = QApplication::style();

            QStyleOptionToolBox opt;
            (*((QStyleOption*)&opt)) = option;
            opt.text = index.data().toString();

            m_pStyle->drawControl(QStyle::CE_ToolBoxTab, &opt, painter);
            return;
        }
        QStyledItemDelegate::paint(painter, option, index);
    }
};

ToolBoxTreeView::ToolBoxTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setIconSize(QSize(24, 24));

    m_pDelegate = new ToolBoxDelegate(this);
    setItemDelegate(m_pDelegate);
    setItemsExpandable(false);
    setRootIsDecorated(false);
}

ToolBoxTreeView::~ToolBoxTreeView()
{

}

void ToolBoxTreeView::mousePressEvent(QMouseEvent *event)
{
    if (!model())
    {
        QTreeView::mousePressEvent(event);
        return;
    }

    QModelIndex index = indexAt(event->pos());
    if (!index.isValid())
    {
        QTreeView::mousePressEvent(event);
        return;
    }

    QModelIndex parentIndex = model()->parent(index);
    if (!parentIndex.isValid())
    {
        QTreeView::mousePressEvent(event);
        return;
    }

    if (event->button() == Qt::LeftButton)
    {
        QIcon icon = index.data(Qt::DecorationRole).value<QIcon>();

        QDrag *drag = new QDrag(this);
        QMimeData *mimeData = new QMimeData();

        QString mimetype = model()->data(index, ToolBoxModel::MimeTypeRole).toString();
        QByteArray mimidata = model()->data(index, ToolBoxModel::MimeDataRole).toByteArray();
        mimeData->setData(mimetype, mimidata);

        if (!icon.isNull())
            drag->setPixmap(icon.pixmap(QSize(24, 24)));

        drag->setMimeData(mimeData);
        //qDebug() << mimetype << mimidata;
        /*Qt::DropAction dropAction = */drag->exec();
    }
}
