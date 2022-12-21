#include "toolboxtreeview.h"
#include "qmimedata.h"
#include "toolbox/toolboxmodel.h"
#include <QMouseEvent>
#include <QDrag>
#include <QGuiApplication>
#include <QIcon>
#include <QDebug>

ToolBoxTreeView::ToolBoxTreeView(QWidget *parent) :
    QTreeView(parent)
{
    setIconSize(QSize(24, 24));
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
