// ToolboxMenu.cpp
#include "ToolboxMenu.h"
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDebug>
#include <QFont>
#include <QFileInfo>

ToolboxCircularMenu::ToolboxCircularMenu(QWidget *parent)
    : CircularMenu(parent)
{
    setTitle("Toolbox");
    applyOffice2013Style();
    connect(this, &ToolboxCircularMenu::triggered,
            this, &ToolboxCircularMenu::onActionTriggered);
}

ToolboxCircularMenu::~ToolboxCircularMenu()
{
}

void ToolboxCircularMenu::applyOffice2013Style()
{
    setupOffice2013Colors();

    // Настройка шрифтов в стиле Office 2013
    QFont textFont("Segoe UI", 9);
    textFont.setHintingPreference(QFont::PreferDefaultHinting);
    setTextFont(textFont);

    QFont centerFont("Segoe UI", 10, QFont::Bold);
    centerFont.setHintingPreference(QFont::PreferDefaultHinting);
    setCenterFont(centerFont);

    // Геометрические настройки
    setCenterRadius(35);
    setIconSize(24);
    setIconDistance(0.42);
    setTextDistance(0.78);
    setRadius(180);

    // Обновляем заголовок
    update();
}

void ToolboxCircularMenu::setupOffice2013Colors()
{
    // Основные цвета Office 2013
    setBackgroundColor(QColor(255, 255, 255, 248));     // Белый фон с прозрачностью
    setHoverColor(QColor(206, 232, 252, 200));          // Голубой при наведении (#cee8fc)
    setBorderColor(QColor(0, 114, 198, 200));           // Синяя рамка (#0072C6)

    // Цвета текста
    setTextColor(QColor(51, 51, 51));                   // Темно-серый (#333333)
    setTextHoverColor(QColor(0, 0, 0));                 // Черный при наведении

    // Центральная кнопка
    setCenterBgColor(QColor(0, 114, 198));              // Синий фон (#0072C6)
    setCenterBorderColor(QColor(0, 114, 198));          // Синяя рамка
    setCenterTextColor(QColor(255, 255, 255));          // Белый текст

    // Разделители и тени
    setSeparatorColor(QColor(197, 210, 224, 180));      // Светло-серый (#c5d2e0)
    setShadowColor(QColor(0, 0, 0, 40));                // Легкая тень
}

bool ToolboxCircularMenu::loadFromJson(const QString &jsonFilePath)
{
    QFile file(jsonFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qDebug() << "Failed to open JSON file:" << jsonFilePath;
        return false;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    return loadFromJson(jsonData);
}

bool ToolboxCircularMenu::loadFromJson(const QByteArray &jsonData)
{
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonData, &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qDebug() << "JSON parse error:" << parseError.errorString();
        return false;
    }

    return loadFromJson(doc.object());
}

bool ToolboxCircularMenu::loadFromJson(const QJsonObject &jsonObject)
{
    if (!jsonObject.contains("templates") || !jsonObject["templates"].isArray())
    {
        qDebug() << "Invalid JSON structure: missing 'templates' array";
        return false;
    }

    clearItems();

    QJsonArray templatesArray = jsonObject["templates"].toArray();

    for (const QJsonValue &value : templatesArray)
    {
        if (!value.isObject())
            continue;

        QJsonObject templateObj = value.toObject();

        ToolboxItemData item;
        item.alias = templateObj["alias"].toString();
        item.tooltip = templateObj["tooltip"].toString();
        item.mimetype = templateObj["mimetype"].toString();

        // Иконка
        QString iconPath = templateObj["icon"].toString();
        if (!iconPath.isEmpty())
        {
            item.icon = QIcon(iconPath);
            if (item.icon.isNull())
            {
                // Пробуем загрузить из ресурсов с другим префиксом
                item.icon = QIcon(":/img/" + QFileInfo(iconPath).fileName());
            }
        }

        // MimeData - сериализуем в JSON
        if (templateObj.contains("mimedata") && templateObj["mimedata"].isObject())
        {
            QJsonObject mimedataObj = templateObj["mimedata"].toObject();
            QJsonDocument mimedataDoc(mimedataObj);
            item.mimeData = mimedataDoc.toJson();
        }

        m_items.append(item);
    }

    rebuildMenu();

    qDebug() << "Loaded" << m_items.size() << "items from JSON";
    return true;
}

void ToolboxCircularMenu::addToolboxItem(const QString &alias,
                                         const QIcon &icon,
                                         const QString &mimetype,
                                         const QByteArray &mimeData,
                                         const QString &tooltip)
{
    ToolboxItemData item;
    item.alias = alias;
    item.icon = icon;
    item.mimetype = mimetype;
    item.mimeData = mimeData;
    item.tooltip = tooltip;

    m_items.append(item);
    rebuildMenu();
}

void ToolboxCircularMenu::clearItems()
{
    m_items.clear();
    m_actionToItem.clear();
    clear();
}

void ToolboxCircularMenu::rebuildMenu()
{
    clear();
    m_actionToItem.clear();

    if (m_items.isEmpty())
    {
        QAction *emptyAction = new QAction("No items", this);
        emptyAction->setEnabled(false);

        // Стилизуем пустой пункт
        QFont emptyFont("Segoe UI", 9);
        emptyFont.setItalic(true);
        emptyAction->setFont(emptyFont);

        addAction(emptyAction);
        setTitle("Toolbox");
        return;
    }

    // Сортируем элементы по алфавиту
    QList<ToolboxItemData> sortedItems = m_items;
    /*std::sort(sortedItems.begin(), sortedItems.end(),
              [](const ToolboxItemData &a, const ToolboxItemData &b) {
                  return a.alias < b.alias;
              });*/

    for (const ToolboxItemData &item : sortedItems)
    {
        QAction *action = createActionForItem(item);
        addAction(action);
        m_actionToItem[action] = item;
    }

    setTitle(QString("Toolbox (%1)").arg(m_items.size()));
}

QAction* ToolboxCircularMenu::createActionForItem(const ToolboxItemData &item)
{
    QAction *action = new QAction(item.icon, item.alias, this);

    if (!item.tooltip.isEmpty())
        action->setToolTip(item.tooltip);

    action->setData(item.alias);
    action->setIconVisibleInMenu(true);

    // Стилизуем действие
    QFont actionFont("Segoe UI", 9);
    action->setFont(actionFont);

    return action;
}

QMimeData* ToolboxCircularMenu::getSelectedMimeData() const
{
    if (m_currentSelected.mimeData.isEmpty())
        return nullptr;

    QMimeData *mimeData = new QMimeData();
    mimeData->setData(m_currentSelected.mimetype, m_currentSelected.mimeData);
    mimeData->setText(m_currentSelected.alias);

    return mimeData;
}

QString ToolboxCircularMenu::getSelectedMimetype() const
{
    return m_currentSelected.mimetype;
}

QByteArray ToolboxCircularMenu::getSelectedMimeDataBytes() const
{
    return m_currentSelected.mimeData;
}

QString ToolboxCircularMenu::getSelectedAlias() const
{
    return m_currentSelected.alias;
}

QMimeData* ToolboxCircularMenu::execForMimeData(const QPoint &globalPos)
{
    // Защита от повторного вызова
    static bool isExecuting = false;
    if (isExecuting)
        return nullptr;

    isExecuting = true;

    QAction *selected = exec(globalPos);

    QMimeData *result = nullptr;
    if (selected && m_actionToItem.contains(selected))
    {
        m_currentSelected = m_actionToItem[selected];
        result = getSelectedMimeData();

        // Логирование выбора
        qDebug() << "Toolbox selected:" << m_currentSelected.alias
                 << "Mimetype:" << m_currentSelected.mimetype;
    }

    isExecuting = false;
    return result;
}

QMimeData* ToolboxCircularMenu::showToolbox(const QPoint &globalPos,
                                            const QString &jsonPath,
                                            QWidget *parent)
{
    ToolboxCircularMenu menu(parent);
    menu.applyOffice2013Style();

    if (!menu.loadFromJson(jsonPath))
    {
        qDebug() << "Failed to load toolbox from:" << jsonPath;
        return nullptr;
    }

    return menu.execForMimeData(globalPos);
}

void ToolboxCircularMenu::onActionTriggered(QAction *action)
{
    if (m_actionToItem.contains(action))
    {
        m_currentSelected = m_actionToItem[action];

        // Эмиттим сигнал с выбранным элементом
        QMimeData *mimeData = getSelectedMimeData();
        emit toolboxItemSelected(m_currentSelected.alias, mimeData);

        // Очищаем mimeData, так как он будет удален в слоте получателя
        // Не удаляем здесь!
    }
}