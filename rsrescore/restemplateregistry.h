#ifndef RESTEMPLATEREGISTRY_H
#define RESTEMPLATEREGISTRY_H

#include "rsrescore_global.h"
#include <QJsonObject>
#include <QList>
#include <QString>
#include <QStringList>

// Реестр пользовательских шаблонов ресурсов. Плагин указывает свои
// каталоги (addSearchDir) — реестр сканирует их на *.json-дескрипторы
// вида:
//   {
//       "title": "Панель ввода счёта",
//       "icon": "icon.png",            // рядом с json или "theme:Dialog"
//       "group": "Панели",
//       "description": "...",
//       "template": "panel.xml",       // путь относительно json
//       "needname": true, "needlbr": true, "namelen": 8,
//       "validator": "StdPanNameValidator",
//       "action": "{...}",             // опционально: явный guid
//       "ribbon": ["panels"]           // опционально: секции ленты
//   }
//
// guid по умолчанию — стабильный хэш пути к json (переживает перезапуски,
// ссылки из ribbon-секций не ломаются). Принадлежность шаблона плагину
// определяется расположением: плагин сканирует только свои каталоги.
class RSRESCORE_EXPORT ResTemplateRegistry
{
public:
    ResTemplateRegistry();

    // Стандартные каталоги шаблонов для плагина с идентификатором id
    // ("panels", "menu", ...): <appdir>/reseditor/templates/<id>,
    // <cwd>/reseditor/templates/<id> и %APPDATA%/<app>/templates/<id>
    static QString programTemplatesDir(const QString &id);
    static QString userTemplatesDir(const QString &id);
    static QString workingTemplatesDir(const QString &id);

    // Стандартные каталоги пользовательских элементов ToolBox:
    // та же тройка каталогов, но с подкаталогом toolbox/<id>
    static QString programToolBoxDir(const QString &id);
    static QString userToolBoxDir(const QString &id);
    static QString workingToolBoxDir(const QString &id);

    // Порядок добавления = приоритет: при конфликте guid побеждает
    // шаблон из каталога, добавленного раньше
    void addSearchDir(const QString &dir);

    // Перечитать каталоги (дескрипторы без title/template или с
    // несуществующим xml пропускаются с предупреждением в лог)
    void rescan();

    // Дописывает найденные шаблоны к металисту плагина:
    // в массив "items" и в секции "ribbon" (по списку из дескриптора)
    QString appendToMetaList(const QString &baseJson) const;

    bool contains(const QString &guid) const;
    // Абсолютный путь к xml шаблона (пусто, если guid неизвестен)
    QString templatePath(const QString &guid) const;

private:
    struct TemplateInfo
    {
        QString guid;
        QJsonObject meta;    // готовый item для NewItemsDlg (пути абсолютные)
        QStringList ribbon;  // секции ленты из дескриптора
        QString xmlPath;     // абсолютный путь к xml шаблона
    };

    // Сборка пути стандартного каталога: "<base>/<subdir>/<id>"
    static QString dirFor(const QString &base, const QString &subdir, const QString &id);

    bool addFromDescriptor(const QString &jsonPath);

    QStringList m_dirs;
    QList<TemplateInfo> m_templates;
};

#endif // RESTEMPLATEREGISTRY_H
