// ToolboxMenu.h
#ifndef TOOLBOXMENU_H
#define TOOLBOXMENU_H

#include "CircularMenu.h"
#include <QMap>
#include <QMimeData>

class ToolboxCircularMenu : public CircularMenu
{
    Q_OBJECT

public:
    explicit ToolboxCircularMenu(QWidget *parent = nullptr);
    ~ToolboxCircularMenu();

    // Загрузка данных из JSON
    bool loadFromJson(const QString &jsonFilePath);
    bool loadFromJson(const QJsonObject &jsonObject);
    bool loadFromJson(const QByteArray &jsonData);

    // Добавление элемента вручную
    void addToolboxItem(const QString &alias,
                        const QIcon &icon,
                        const QString &mimetype,
                        const QByteArray &mimeData,
                        const QString &tooltip = QString());

    // Очистка всех элементов
    void clearItems();

    // Получение MimeData для выбранного элемента
    QMimeData* getSelectedMimeData() const;
    QString getSelectedMimetype() const;
    QByteArray getSelectedMimeDataBytes() const;
    QString getSelectedAlias() const;

    // Блокирующий exec с возвратом MimeData
    QMimeData* execForMimeData(const QPoint &globalPos);

    // Статический метод для быстрого показа меню
    static QMimeData* showToolbox(const QPoint &globalPos,
                                  const QString &jsonPath,
                                  QWidget *parent = nullptr);

    // Применение Office 2013 стиля
    void applyOffice2013Style();

signals:
    void toolboxItemSelected(const QString &alias, const QMimeData *mimeData);

private slots:
    void onActionTriggered(QAction *action);

private:
    struct ToolboxItemData
    {
        QString alias;
        QString mimetype;
        QByteArray mimeData;
        QIcon icon;
        QString tooltip;
    };

    void rebuildMenu();
    QAction* createActionForItem(const ToolboxItemData &item);
    void setupOffice2013Colors();

    QList<ToolboxItemData> m_items;
    QMap<QAction*, ToolboxItemData> m_actionToItem;
    ToolboxItemData m_currentSelected;
};

#endif // TOOLBOXMENU_H