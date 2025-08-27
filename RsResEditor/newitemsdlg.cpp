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

#define isSeted(item,role) item->data(role).toBool()

NewItemsDlg::NewItemsDlg(LbrObjectInterface *lbr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewItemsDlg),
    m_pLbrObj(lbr)
{
    m_pSelectedItem = nullptr;

    ui->setupUi(this);
    ui->treeWidget->header()->setVisible(false);

    QString dialogStyle = R"(
    /* Стиль для QTreeWidget - белый фон */
    QTreeWidget {
        background: white;
        border: 1px solid #c0c0c0;
        border-radius: 4px;
        outline: none;
    }

    /* Элементы TreeView - белые без подсветки */
    QTreeWidget::item {
        background: white;
        border: none;
        padding: 4px;
        color: black;
    }

    /* Заголовок TreeView - скрываем */
    QHeaderView::section {
        background: white;
        border: none;
        height: 0px;
        padding: 0px;
        margin: 0px;
    }

    /* Стиль для QListWidget */
    QListWidget {
        background: white;
        outline: none;
    }

    QListWidget::item {
        background: white;
        border: 1px solid #d0d0d0;
        border-radius: 4px;
        padding: 8px;
        margin: 2px;
        color: black;
        text-align: center;
    }

    QListWidget::item:selected {
        background: #0078d7;
        border: 2px solid #005fa3;
    }

    QListWidget::item:hover {
        background: #e3f2fd;
        border: 1px solid #0078d7;
        color: black;
    }
    )";

    setStyleSheet(dialogStyle);

    setWindowIcon(QIcon::fromTheme("NewFile"));
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

    /*QListWidget *list = new QListWidget(this);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(32, 32)); // Явно устанавливаем размер иконок
    list->setFrameShape(QFrame::NoFrame);
    list->setSortingEnabled(true);
    list->setWordWrap(true);
    list->setResizeMode(QListView::Adjust); // Меняем на Adjust вместо Fixed
    list->setMovement(QListView::Static);
    list->setUniformItemSizes(false); // Меняем на false для переменного размера
    list->setSpacing(10); // Увеличиваем отступы

    // Устанавливаем размер сетки (опционально)
    list->setGridSize(QSize(80, 80)); // Ширина, высота

    // Включаем выравнивание по сетке
    list->setViewMode(QListView::IconMode);

    list->updateGeometry();

    connect(list, &QListWidget::itemClicked, this, &NewItemsDlg::itemUpdated);
    connect(list, &QListWidget::itemDoubleClicked, this, &NewItemsDlg::itemDoubleClicked);

    return list;*/

    /*QListWidget *list = new QListWidget(this);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(32, 32));
    list->setFrameShape(QFrame::NoFrame);
    list->setSortingEnabled(true);
    list->setWordWrap(true);
    list->setResizeMode(QListView::Adjust);  // Меняем на Adjust для лучшего отображения
    list->setMovement(QListView::Static);
    list->setUniformItemSizes(false);        // Меняем на false
    list->setSpacing(8);                     // Увеличиваем отступы

    // Устанавливаем размер сетки для лучшего выравнивания
    list->setGridSize(QSize(100, 90));

    list->updateGeometry();

    connect(list, &QListWidget::itemClicked, this, &NewItemsDlg::itemUpdated);
    connect(list, &QListWidget::itemDoubleClicked, this, &NewItemsDlg::itemDoubleClicked);

    return list;*/
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
    QJsonObject ribbon = root["ribbon"].toObject();

    for (const auto &value : qAsConst(items))
    {
        QJsonObject obj = value.toObject();
        addFromMetaData(obj);
    }

    QJsonArray panels = ribbon["panels"].toArray();
    for (const auto &value : qAsConst(panels))
        m_RibbonPannels.append(value.toString());

    QJsonArray scrols = ribbon["scrols"].toArray();
    for (const auto &value : qAsConst(scrols))
        m_RibbonScrols.append(value.toString());
}

void NewItemsDlg::addFromMetaData(const QJsonObject &metadata)
{
    QListWidget *group = getGroup(metadata["group"].toString(), true);
    addItemToGroupList(group, metadata);
}

GroupInfoMap NewItemsDlg::fillGroupInfoFromListItem(QListWidgetItem* item)
{
    GroupInfoMap infoMap;

    if (!item)
        return infoMap;

    infoMap[RoleDescription] = item->data(RoleDescription);
    infoMap[RoleGroup] = item->data(RoleGroup);
    infoMap[RoleAction] = item->data(RoleAction);
    infoMap[RoleNeedName] = item->data(RoleNeedName);
    infoMap[RoleNeedPath] = item->data(RoleNeedPath);
    infoMap[RoleNeedLbr] = item->data(RoleNeedLbr);
    infoMap[RoleNameLen] = item->data(RoleNameLen);
    infoMap[RoleIconName] = item->data(RoleIconName);
    infoMap[RoleTitle] = item->data(RoleTitle);

    return infoMap;
}

GroupInfoMap NewItemsDlg::getInfoForItem(const QString &guid)
{
    return m_Templates[guid];
}

void NewItemsDlg::addItemToGroupList(QListWidget *list, const QJsonObject &metadata)
{
    bool needlbr = metadata["needlbr"].toBool(true);

    if (needlbr && !m_pLbrObj)
        return;

    QIcon icon;
    QListWidgetItem *item = new QListWidgetItem();
    QString iconname = metadata["icon"].toString();

    if (iconname.startsWith("theme:"))
    {
        iconname = iconname.remove("theme:");
        icon = QIcon::fromTheme(iconname);
    }
    else
        icon = QIcon(iconname);

    item->setIcon(icon);
    item->setText(metadata["title"].toString());
    item->setData(RoleDescription, metadata["description"].toString());
    item->setData(RoleGroup, metadata["group"].toString());
    item->setData(RoleAction, metadata["action"].toString());
    item->setData(RoleNeedName, metadata["needname"].toBool());
    item->setData(RoleNeedPath, metadata["needpath"].toBool());
    item->setData(RoleNeedLbr, needlbr);
    item->setData(RoleNameLen, metadata["namelen"].toInt(255));
    item->setData(RoleIconName, metadata["icon"].toString());
    item->setData(RoleTitle, metadata["title"].toString());

    GroupInfoMap infoMap = fillGroupInfoFromListItem(item);
    m_Templates.insert(item->data(RoleAction).toString(), infoMap);

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

QStringList NewItemsDlg::getGroups() const
{
    return m_Groups.keys();
}

const QStringList &NewItemsDlg::ribbonScrols() const
{
    return m_RibbonScrols;
}

const QStringList &NewItemsDlg::ribbonPannels() const
{
    return m_RibbonPannels;
}

#define AddInfoToElement(key) element[key] = item->data(key)
QList<GroupInfoMap> NewItemsDlg::groupInfo(const QString &name)
{
    if (!m_Groups.contains(name))
        return QList<GroupInfoMap>();

    QList<GroupInfoMap> lst;
    QListWidget *listwidget = getGroup(name, false);

    for (int i = 0; i < listwidget->count(); i++)
    {
        GroupInfoMap element;
        QListWidgetItem *item = listwidget->item(i);

        AddInfoToElement(RoleDescription);
        AddInfoToElement(RoleGroup);
        AddInfoToElement(RoleAction);
        AddInfoToElement(RoleNeedName);
        AddInfoToElement(RoleNeedPath);
        AddInfoToElement(RoleNeedLbr);
        AddInfoToElement(RoleNameLen);
        AddInfoToElement(RoleIconName);
        AddInfoToElement(RoleTitle);

        lst.append(element);
    }

    return lst;
}

void NewItemsDlg::filterByAction(const QString& action)
{
    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem* groupItem = ui->treeWidget->topLevelItem(i);
        groupItem->setHidden(true);

        if (groupItem->childCount() > 0)
        {
            QListWidget* list = qobject_cast<QListWidget*>(ui->treeWidget->itemWidget(groupItem->child(0), 0));

            if (list)
            {
                for (int j = 0; j < list->count(); ++j)
                {
                    QListWidgetItem* item = list->item(j);
                    bool matches = item->data(RoleAction).toString() == action;
                    item->setHidden(!matches);

                    if (matches)
                    {
                        groupItem->setHidden(false);

                        list->setCurrentItem(item);
                        itemUpdated(item);
                    }
                }
            }
        }
    }

    ui->treeWidget->expandAll();
}

void NewItemsDlg::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    if (ui->nameEdit->isEnabled())
        ui->nameEdit->setFocus();
    else
    {
        QWidget* nextFocus = ui->nameEdit->nextInFocusChain();
        if (nextFocus && nextFocus->isEnabled())
            nextFocus->setFocus();
    }
}
