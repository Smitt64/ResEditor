#include "labeltextpropertyitem.h"
#include "widgets/labeltexteditdlg.h"
#include <QLineEdit>
#include <QIcon>
#include <QAction>

LabelTextPropertyItem::LabelTextPropertyItem(QObject *rectItem, QObject *parent)
    : PropertyTreeItem{rectItem, parent}
{

}

LabelTextPropertyItem::~LabelTextPropertyItem()
{

}

QWidget *LabelTextPropertyItem::createEditor(QWidget *parent, const QStyleOptionViewItem &option, const QModelIndex &index) const
{
    QLineEdit *pEditor = new QLineEdit(parent);
    QAction *myAction = pEditor->addAction(QIcon(":/img/CustomControl_16x.png"), QLineEdit::TrailingPosition);

    connect(myAction, &QAction::triggered, [pEditor]()
    {
        LabelTextEditDlg dlg(pEditor);
        dlg.editor()->setText(pEditor->text());

        if (dlg.exec() == QDialog::Accepted)
            pEditor->setText(dlg.editor()->text());
    });
    return pEditor;
}

bool LabelTextPropertyItem::setEditorData(QWidget *editor, const QModelIndex &index) const
{
    QLineEdit *pEditor = qobject_cast<QLineEdit*>(editor);
    pEditor->setProperty("text", object()->property(propertyName().toLocal8Bit()));
    return true;
}

bool LabelTextPropertyItem::setModelData(QWidget *editor, QAbstractItemModel *model, const QModelIndex &index) const
{
    QLineEdit *pEditor = qobject_cast<QLineEdit*>(editor);
    object()->setProperty(propertyName().toLocal8Bit(), pEditor->text());

    return true;
}
