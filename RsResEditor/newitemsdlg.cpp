#include "newitemsdlg.h"
#include "ui_newitemsdlg.h"
#include "rsrescore.h"
#include <QFileDialog>
#include <QListWidget>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonValue>
#include <QJsonParseError>

enum
{
    RoleDescription = Qt::UserRole + 1,
    RoleAction,
    RoleGroup,
    RoleNeedName,
    RoleNeedLbr,
    RoleNeedPath,
    RoleNameLen
};

#define isSeted(item,role) item->data(role).toBool()

NewItemsDlg::NewItemsDlg(LbrObjectInterface *lbr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewItemsDlg),
    m_pLbrObj(lbr)
{
    m_pSelectedItem = nullptr;

    ui->setupUi(this);
    ui->treeWidget->header()->setVisible(false);

    updateAcceptButton();

    connect(ui->pathButton, &QPushButton::clicked, this, &NewItemsDlg::pathButton);
    connect(ui->nameEdit, &QLineEdit::textChanged, this, &NewItemsDlg::updateAcceptButton);
}

NewItemsDlg::~NewItemsDlg()
{
    delete ui;
}

void NewItemsDlg::itemUpdated(QListWidgetItem *item)
{
    QString label = QString("<b>Имя: </b> %3<br><b>Группа: </b> %1<br><br>%2")
            .arg(item->data(RoleGroup).toString())
            .arg(item->data(RoleDescription).toString())
            .arg(item->text());

    ui->descriptionLabel->setText(label);
    ui->nameEdit->setEnabled(item->data(RoleNeedName).toBool());
    ui->pathEdit->setEnabled(item->data(RoleNeedPath).toBool());
    ui->pathButton->setEnabled(item->data(RoleNeedPath).toBool());
    ui->nameEdit->setMaxLength(item->data(RoleNameLen).toInt());

    QListWidget *list = qobject_cast<QListWidget*>(sender());
    if (list)
    {
        QHashIterator<QString, QTreeWidgetItem*> iter(m_Groups);
        while(iter.hasNext())
        {
            iter.next();
            QListWidget *itemLst = qobject_cast<QListWidget*>(ui->treeWidget->itemWidget(iter.value(), 0));
            if (itemLst != list)
                itemLst->selectionModel()->clearSelection();
        }
    }

    m_pSelectedItem = item;
    updateAcceptButton();
}

QListWidget *NewItemsDlg::CreateSubList()
{
    QListWidget *list = new QListWidget(this);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(32, 32));
    list->setFrameShape(QFrame::NoFrame);
    list->setSortingEnabled(true);
    list->setWordWrap(true);
    list->setResizeMode(QListView::Fixed);
    list->setMovement(QListView::Static);
    list->setUniformItemSizes(true);
    list->setSpacing(2);
    /*list->setGridSize(QSize(68, 68));
    list->setResizeMode(QListView::Fixed);*/

    list->updateGeometry();

    connect(list, &QListWidget::itemClicked, this, &NewItemsDlg::itemUpdated);
    connect(list, &QListWidget::itemDoubleClicked, this, &NewItemsDlg::itemDoubleClicked);

    return list;
}

QListWidget *NewItemsDlg::getGroup(const QString &name, bool addIfNotExists)
{
    if (m_Groups.contains(name))
    {
        QListWidget *list = qobject_cast<QListWidget*>(ui->treeWidget->itemWidget(m_Groups[name], 0));
        return list;
    }

    if (addIfNotExists)
    {
        QTreeWidgetItem *groupItem = new QTreeWidgetItem();
        groupItem->setText(0, name);
        groupItem->setFlags(groupItem->flags() & (~Qt::ItemIsSelectable));
        ui->treeWidget->addTopLevelItem(groupItem);

        QTreeWidgetItem *groupItemView = new QTreeWidgetItem();
        groupItem->addChild(groupItemView);

        QListWidget *groupItemList = CreateSubList();
        ui->treeWidget->setItemWidget(groupItemView, 0, groupItemList);

        m_Groups[name] = groupItemView;
        return groupItemList;
    }

    return nullptr;
}

void NewItemsDlg::buildStandartNewItems()
{
    QStringList metalist = RsResCore::inst()->newItemsMetaList();

    for (const QString &meta : qAsConst(metalist))
        addFromMetaDataList(meta);

    for (int i = ui->treeWidget->topLevelItemCount() - 1; i >= 0; i--)
    {
        QTreeWidgetItem *item = ui->treeWidget->topLevelItem(i);
        QListWidget *list = qobject_cast<QListWidget*>(ui->treeWidget->itemWidget(item->child(0), 0));

        if (list->count() <= 0)
        {
            item = ui->treeWidget->takeTopLevelItem(i);
            m_Groups.remove(item->text(0));
            ui->treeWidget->removeItemWidget(item->child(0), 0);
            delete item;
        }
    }

    ui->treeWidget->expandAll();
}

void NewItemsDlg::addFromMetaDataList(const QString &metadata)
{
    QJsonParseError errors;

    QByteArray tmp = metadata.toStdString().c_str();
    QJsonDocument doc = QJsonDocument::fromJson(tmp, &errors);

    if (errors.error != QJsonParseError::NoError)
    {
        qDebug() << errors.errorString();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray items = root["items"].toArray();

    for (const auto &value : qAsConst(items))
    {
        QJsonObject obj = value.toObject();
        addFromMetaData(obj);
    }
}

void NewItemsDlg::addFromMetaData(const QJsonObject &metadata)
{
    QListWidget *group = getGroup(metadata["group"].toString(), true);
    addItemToGroupList(group, metadata);
}

void NewItemsDlg::addItemToGroupList(QListWidget *list, const QJsonObject &metadata)
{
    bool needlbr = metadata["needlbr"].toBool(true);

    if (needlbr && !m_pLbrObj)
        return;

    QListWidgetItem *item = new QListWidgetItem();
    item->setIcon(QIcon(metadata["icon"].toString()));
    item->setText(metadata["title"].toString());
    item->setData(RoleDescription, metadata["description"].toString());
    item->setData(RoleGroup, metadata["group"].toString());
    item->setData(RoleAction, metadata["action"].toString());
    item->setData(RoleNeedName, metadata["needname"].toBool());
    item->setData(RoleNeedPath, metadata["needpath"].toBool());
    item->setData(RoleNeedLbr, needlbr);
    item->setData(RoleNameLen, metadata["namelen"].toInt(255));

    list->addItem(item);
    updateListSize(list);
}

void NewItemsDlg::updateListSize(QListWidget *list)
{
    int count = list->count();

    QSize sz;
    for (int i = 0; i < count; i++)
    {
        QListWidgetItem *item = list->item(i);
        QRect rect = list->visualItemRect(item);

        sz.setHeight(rect.top() + rect.height());
    }

    list->setFixedHeight(sz.height() + list->spacing() * 5);
    list->updateGeometry();
}

void NewItemsDlg::pathButton()
{
    QString dir = QFileDialog::getExistingDirectory(this);

    if (dir.isEmpty())
        return;

    ui->pathEdit->setText(dir);
    updateAcceptButton();
}

void NewItemsDlg::updateAcceptButton()
{
    bool result = m_pSelectedItem != nullptr;
    if (result && isSeted(m_pSelectedItem,RoleNeedPath) && ui->pathEdit->text().isEmpty())
        result = false;

    if (result && isSeted(m_pSelectedItem,RoleNeedName) && ui->nameEdit->text().isEmpty())
        result = false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(result);
}

QString NewItemsDlg::action() const
{
    return m_pSelectedItem->data(RoleAction).toString();
}

QString NewItemsDlg::name() const
{
    return ui->nameEdit->text();
}

QString NewItemsDlg::path() const
{
    return ui->pathEdit->text();
}

void NewItemsDlg::itemDoubleClicked()
{
    updateAcceptButton();

    if (ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled())
        emit ui->buttonBox->accepted();
}
