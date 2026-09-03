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
#include <QPainter>
#include <QPen>
#include <QTimer>

#define isSeted(item,role) item->data(role).toBool()

StdPanNameValidator::StdPanNameValidator(QObject* parent) :
    QValidator(parent)
{

}

QValidator::State StdPanNameValidator::validate(QString &input, int &pos) const
{
    QString filtered;
    for (int i = 0; i < input.length(); ++i)
    {
        QChar ch = input[i];
        if (ch.isLetterOrNumber())
            filtered.append(ch.toUpper());
    }

    if (filtered != input)
    {
        input = filtered;
        return QValidator::Intermediate;
    }

    return QValidator::Acceptable;
}

// -----------------------------------------------------------------------------

NewItemsDlg::NewItemsDlg(LbrObjectInterface *lbr, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::NewItemsDlg),
    m_pLbrObj(lbr)
{
    m_pSelectedItem = nullptr;
    m_pValidator = nullptr;

    ui->setupUi(this);
    ui->treeWidget->header()->setVisible(false);
    // колонка на всю ширину — иначе списки шаблонов сжимаются
    // до sizeHintForColumn (2 иконки в строке)
    ui->treeWidget->header()->setSectionResizeMode(0, QHeaderView::Stretch);

    int typeId = QMetaType::type("StdPanNameValidator*");
    if (typeId == QMetaType::UnknownType)
        qRegisterMetaType<StdPanNameValidator*>();

    QString dialogStyle = R"(
    /* Стиль для QTreeWidget - белый фон */
    QTreeWidget {
        background: white;
        border: 1px solid #c0c0c0;
        border-radius: 4px;
        outline: none;
        show-decoration-selected: 0;
    }

    /* Элементы TreeView - белые без подсветки */
    QTreeWidget::item {
        background: white;
        border: none;
        padding: 0px;  /* Убираем отступы */
        margin: 0px;   /* Убираем margins */
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
        color: white;
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
    connect(ui->searchEdit, &QLineEdit::textChanged, this, &NewItemsDlg::applyFilter);
    connect(ui->nameEdit, &QLineEdit::returnPressed, this, [this]()
    {
        if (ui->buttonBox->button(QDialogButtonBox::Ok)->isEnabled())
            accept();
    });
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

    bool NeedName = item->data(RoleNeedName).toBool();
    bool NeedPath = item->data(RoleNeedPath).toBool();

    ui->descriptionLabel->setText(label);
    ui->nameEdit->setEnabled(NeedName);
    ui->pathEdit->setEnabled(NeedPath);
    ui->pathButton->setEnabled(NeedPath);
    ui->nameEdit->setMaxLength(item->data(RoleNameLen).toInt());

    if (m_pValidator)
    {
        delete m_pValidator;
        m_pValidator = nullptr;
    }

    QString ValidatorName = item->data(RoleValidator).toString();
    if (!ValidatorName.isEmpty())
    {
        m_pValidator = createValidator(ValidatorName);
        ui->nameEdit->setValidator(m_pValidator);
    }

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

// -----------------------------------------------------------------------------

void BadgeItemDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QStyledItemDelegate::paint(painter, option, index);

    if (!index.data(RoleUserTemplate).toBool())
        return;

    const QString badgeName = index.data(RoleBadge).toString();
    if (badgeName.isEmpty())
        return;

    QIcon badge = m_BadgeCache.value(badgeName);
    if (badge.isNull())
    {
        badge = QIcon::fromTheme(badgeName);
        m_BadgeCache.insert(badgeName, badge);
    }

    if (badge.isNull())
        return;

    const int bs = 18;
    const QRect r(option.rect.right() - bs - 3, option.rect.top() + 3, bs, bs);

    // контрастная подложка, чтобы бейдж читался поверх иконки/текста
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setPen(QPen(QColor(0xc0, 0xc0, 0xc0)));
    painter->setBrush(Qt::white);
    painter->drawEllipse(r.adjusted(-1, -1, 1, 1));
    painter->restore();

    badge.paint(painter, r);
}

// -----------------------------------------------------------------------------

QListWidget *NewItemsDlg::CreateSubList()
{
    QListWidget *list = new QListWidget(this);
    list->setViewMode(QListView::IconMode);
    list->setIconSize(QSize(32, 32));
    // бейдж пользовательских шаблонов рисует делегат
    list->setItemDelegate(new BadgeItemDelegate(list));
    list->setFrameShape(QFrame::NoFrame);
    list->setSortingEnabled(true);
    list->setWordWrap(true);
    // Adjust, а не Fixed: Fixed раскладывает итемы один раз (при узкой
    // начальной ширине) и не перестраивает ряды при ресайзе — получалось
    // 2 иконки в строке до первого фильтра
    list->setResizeMode(QListView::Adjust);
    list->setMovement(QListView::Static);
    list->setUniformItemSizes(true);
    //list->setSpacing(4);
    //list->setGridSize(QSize(68, 68));

    list->updateGeometry();

    // пересчёт высоты при изменении ширины списка (eventFilter)
    list->setProperty("newItemsList", true);
    list->installEventFilter(this);

    // currentItemChanged, а не itemClicked: реагируем и на клавиатурную
    // навигацию (стрелки), иначе описание/кнопка OK не обновлялись
    connect(list, &QListWidget::currentItemChanged, this,
            [this](QListWidgetItem *current, QListWidgetItem *)
    {
        if (current && !current->isHidden())
            itemUpdated(current);
    });
    connect(list, &QListWidget::itemActivated, this, [this](QListWidgetItem *)
    {
        itemDoubleClicked();
    });
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

    // visualItemRect валиден только после раскладки представления —
    // пересчитываем высоты отложенно
    QTimer::singleShot(0, this, &NewItemsDlg::updateAllListSizes);
}

void NewItemsDlg::addFromMetaDataList(const QString &metadata)
{
    QJsonParseError errors;

    QJsonDocument doc = QJsonDocument::fromJson(metadata.toUtf8(), &errors);

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

    // Все секции ribbon (panels/scrols/menus/...) — generic, чтобы плагины
    // могли добавлять свои кнопки создания без правки диалога
    for (auto it = ribbon.begin(); it != ribbon.end(); ++it)
    {
        const QJsonArray guids = it.value().toArray();
        for (const auto &value : guids)
            m_RibbonSections[it.key()].append(value.toString());
    }
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
    infoMap[RoleValidator] = item->data(RoleValidator);
    infoMap[RoleUserTemplate] = item->data(RoleUserTemplate);
    infoMap[RoleBadge] = item->data(RoleBadge);

    return infoMap;
}

GroupInfoMap NewItemsDlg::getInfoForItem(const QString &guid)
{
    return m_Templates.value(guid);
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

    // бейдж пользовательского шаблона рисует BadgeItemDelegate
    // поверх элемента списка, иконку не трогаем
    const bool userTemplate = metadata["usertemplate"].toBool();
    const QString badgeName = metadata["badge"].toString();

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
    item->setData(RoleValidator, metadata["validator"].toString());
    item->setData(RoleUserTemplate, userTemplate);
    item->setData(RoleBadge, badgeName);
    item->setSizeHint(QSize(90, 120));

    GroupInfoMap infoMap = fillGroupInfoFromListItem(item);
    m_Templates.insert(item->data(RoleAction).toString(), infoMap);

    list->addItem(item);
    updateListSize(list);
}

void NewItemsDlg::updateListSize(QListWidget *list)
{
    // Высота по нижней кромке самого нижнего ВИДИМОГО элемента
    // (скрытые фильтром не учитываем)
    int maxBottom = 0;
    for (int i = 0; i < list->count(); ++i)
    {
        QListWidgetItem *item = list->item(i);
        if (item->isHidden())
            continue;

        const QRect rect = list->visualItemRect(item);
        if (rect.isValid())
            maxBottom = qMax(maxBottom, rect.bottom() + 1);
    }

    if (maxBottom <= 0)
        maxBottom = list->sizeHintForRow(0);

    const int height = maxBottom + list->spacing() + 4;
    list->setFixedHeight(height);

    // Явно сообщаем строке дерева высоту содержимого: QTreeView подгоняет
    // строку под item-widget только при собственной перекладке
    for (auto it = m_Groups.begin(); it != m_Groups.end(); ++it)
    {
        if (ui->treeWidget->itemWidget(it.value(), 0) == list)
        {
            it.value()->setSizeHint(0, QSize(-1, height));
            break;
        }
    }

    // форсируем перекладку дерева, иначе строка остаётся высокой
    // и под списком висит пустая область
    ui->treeWidget->doItemsLayout();

    list->updateGeometry();
}

void NewItemsDlg::updateAllListSizes()
{
    for (auto it = m_Groups.constBegin(); it != m_Groups.constEnd(); ++it)
    {
        QListWidget *lst = qobject_cast<QListWidget*>(ui->treeWidget->itemWidget(it.value(), 0));
        if (lst)
            updateListSize(lst);
    }
}

bool NewItemsDlg::eventFilter(QObject *watched, QEvent *event)
{
    // Список шаблонов изменил ширину (первый показ, ресайз диалога,
    // растяжение колонки) — пересчитать высоту. Отложенно, чтобы
    // QListView успел перестроить ряды (Adjust) и visualItemRect стал валиден
    if (event->type() == QEvent::Resize && watched->property("newItemsList").toBool())
    {
        QListWidget *list = static_cast<QListWidget*>(watched);
        QTimer::singleShot(0, list, [this, list]()
        {
            updateListSize(list);
        });
    }

    return QDialog::eventFilter(watched, event);
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

    if (result && isSeted(m_pSelectedItem,RoleNeedName) &&
            (ui->nameEdit->text().isEmpty() || !ui->nameEdit->hasAcceptableInput()))
        result = false;

    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(result);
}

QString NewItemsDlg::action() const
{
    return m_pSelectedItem ? m_pSelectedItem->data(RoleAction).toString() : QString();
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
        accept();
}

QStringList NewItemsDlg::getGroups() const
{
    return m_Groups.keys();
}

const QStringList &NewItemsDlg::ribbonScrols() const
{
    return ribbonSection(QStringLiteral("scrols"));
}

const QStringList &NewItemsDlg::ribbonPannels() const
{
    return ribbonSection(QStringLiteral("panels"));
}

const QStringList &NewItemsDlg::ribbonSection(const QString &key) const
{
    static const QStringList empty;
    auto it = m_RibbonSections.constFind(key);

    return it != m_RibbonSections.constEnd() ? it.value() : empty;
}

QStringList NewItemsDlg::ribbonSectionKeys() const
{
    return m_RibbonSections.keys();
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
        AddInfoToElement(RoleValidator);
        AddInfoToElement(RoleUserTemplate);
        AddInfoToElement(RoleBadge);

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

    // после скрытия элементов пересчитать высоты списков
    QTimer::singleShot(0, this, &NewItemsDlg::updateAllListSizes);
}

void NewItemsDlg::applyFilter(const QString &text)
{
    const QString needle = text.trimmed();

    for (int i = 0; i < ui->treeWidget->topLevelItemCount(); ++i)
    {
        QTreeWidgetItem *groupItem = ui->treeWidget->topLevelItem(i);
        if (groupItem->childCount() <= 0)
            continue;

        QListWidget *list = qobject_cast<QListWidget*>(ui->treeWidget->itemWidget(groupItem->child(0), 0));
        if (!list)
            continue;

        bool anyVisible = false;
        for (int j = 0; j < list->count(); ++j)
        {
            QListWidgetItem *item = list->item(j);
            const bool matches = needle.isEmpty()
                || item->text().contains(needle, Qt::CaseInsensitive)
                || item->data(RoleDescription).toString().contains(needle, Qt::CaseInsensitive);

            item->setHidden(!matches);
            anyVisible |= matches;
        }

        groupItem->setHidden(!anyVisible);
    }

    QTimer::singleShot(0, this, &NewItemsDlg::updateAllListSizes);
}

void NewItemsDlg::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);

    // ширина колонки зависит от ширины диалога — ряды иконок перестраиваются,
    // пересчитываем высоты списков
    QTimer::singleShot(0, this, &NewItemsDlg::updateAllListSizes);
}

void NewItemsDlg::showEvent(QShowEvent* event)
{
    QDialog::showEvent(event);

    if (ui->nameEdit->isEnabled())
        ui->nameEdit->setFocus();
    else
        ui->searchEdit->setFocus();
}

QIcon NewItemsDlg::badgedIcon(const QIcon &base, const QString &badgeName)
{
    const QIcon badge = QIcon::fromTheme(badgeName);

    if (badge.isNull() || base.isNull())
        return base;

    QIcon result;
    for (int size : {16, 24, 32, 48})
    {
        QPixmap pm = base.pixmap(QSize(size, size));
        const int bs = size / 2; // бейдж — половина иконки в правом нижнем углу

        QPainter p(&pm);
        badge.paint(&p, QRect(size - bs, size - bs, bs, bs));
        p.end();

        result.addPixmap(pm);
    }

    return result;
}

QValidator *NewItemsDlg::createValidator(const QString &className, QObject* parent)
{
    int typeId = QMetaType::type(className.toUtf8() + "*");
    if (typeId == QMetaType::UnknownType)
    {
        qWarning() << "Unknown validator type:" << className;
        return nullptr;
    }

    const QMetaObject* metaObject = QMetaType::metaObjectForType(typeId);
    if (!metaObject)
    {
        qWarning() << "No metaobject for type:" << className;
        return nullptr;
    }

    QObject* object = metaObject->newInstance(Q_ARG(QObject*, parent));
    QValidator* validator = qobject_cast<QValidator*>(object);

    if (!validator)
    {
        qWarning() << "Failed to create validator:" << className;
        delete object;
        return nullptr;
    }

    return validator;
}
