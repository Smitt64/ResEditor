#include "restemplateregistry.h"
#include <QCoreApplication>
#include <QCryptographicHash>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonParseError>
#include <QStandardPaths>
#include <QUuid>

ResTemplateRegistry::ResTemplateRegistry()
{

}

// base + "/" + subdir + "/" + id
QString ResTemplateRegistry::dirFor(const QString &base, const QString &subdir, const QString &id)
{
    return base + QLatin1Char('/') + subdir + QLatin1Char('/') + id;
}

QString ResTemplateRegistry::programTemplatesDir(const QString &id)
{
    return dirFor(QCoreApplication::applicationDirPath() + QLatin1String("/reseditor"),
                  QStringLiteral("templates"), id);
}

QString ResTemplateRegistry::userTemplatesDir(const QString &id)
{
    return dirFor(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),
                  QStringLiteral("templates"), id);
}

QString ResTemplateRegistry::workingTemplatesDir(const QString &id)
{
    // Повторяем структуру programTemplatesDir от рабочего каталога:
    // debug-сборка запускается с cwd = каталогу деплоя (bin), где
    // шаблоны лежат в <cwd>/reseditor/templates/<id>
    return dirFor(QDir::currentPath() + QLatin1String("/reseditor"),
                  QStringLiteral("templates"), id);
}

QString ResTemplateRegistry::programToolBoxDir(const QString &id)
{
    return dirFor(QCoreApplication::applicationDirPath() + QLatin1String("/reseditor"),
                  QStringLiteral("toolbox"), id);
}

QString ResTemplateRegistry::userToolBoxDir(const QString &id)
{
    return dirFor(QStandardPaths::writableLocation(QStandardPaths::AppDataLocation),
                  QStringLiteral("toolbox"), id);
}

QString ResTemplateRegistry::workingToolBoxDir(const QString &id)
{
    return dirFor(QDir::currentPath() + QLatin1String("/reseditor"),
                  QStringLiteral("toolbox"), id);
}

void ResTemplateRegistry::addSearchDir(const QString &dir)
{
    m_dirs.append(dir);
}

void ResTemplateRegistry::rescan()
{
    m_templates.clear();

    for (const QString &dirPath : qAsConst(m_dirs))
    {
        QDir dir(dirPath);

        if (!dir.exists())
            continue;

        const QFileInfoList files = dir.entryInfoList(QStringList() << QStringLiteral("*.json"),
                                                      QDir::Files | QDir::Readable, QDir::Name);
        for (const QFileInfo &fi : files)
            addFromDescriptor(fi.absoluteFilePath());
    }
}

bool ResTemplateRegistry::addFromDescriptor(const QString &jsonPath)
{
    QFile f(jsonPath);
    if (!f.open(QIODevice::ReadOnly))
        return false;

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(f.readAll(), &parseError);

    if (parseError.error != QJsonParseError::NoError)
    {
        qWarning("ResTemplateRegistry: %s: %s", qPrintable(jsonPath), qPrintable(parseError.errorString()));
        return false;
    }

    const QJsonObject descr = doc.object();
    const QDir dir = QFileInfo(jsonPath).absoluteDir();

    const QString title = descr[QStringLiteral("title")].toString();
    const QString templ = descr[QStringLiteral("template")].toString();

    if (title.isEmpty() || templ.isEmpty())
    {
        qWarning("ResTemplateRegistry: %s: нет title или template, шаблон пропущен", qPrintable(jsonPath));
        return false;
    }

    const QString xmlPath = dir.absoluteFilePath(templ);
    if (!QFileInfo::exists(xmlPath))
    {
        qWarning("ResTemplateRegistry: %s: не найден файл шаблона %s", qPrintable(jsonPath), qPrintable(xmlPath));
        return false;
    }

    // guid: явный "action" из дескриптора или стабильный хэш пути к json
    QString guid = descr[QStringLiteral("action")].toString();
    if (guid.isEmpty())
    {
        const QByteArray hash = QCryptographicHash::hash(QDir::toNativeSeparators(jsonPath).toUtf8(),
                                                         QCryptographicHash::Md5);
        guid = QUuid::fromRfc4122(hash).toString();
    }

    // конфликт guid — побеждает каталог, добавленный раньше
    if (contains(guid))
    {
        qWarning("ResTemplateRegistry: %s: guid %s уже занят, шаблон пропущен",
                 qPrintable(jsonPath), qPrintable(guid));
        return false;
    }

    // иконка: "theme:Имя" оставляем как есть, файл резолвим относительно json
    QString icon = descr[QStringLiteral("icon")].toString();
    if (!icon.isEmpty() && !icon.startsWith(QLatin1String("theme:")) && !QDir::isAbsolutePath(icon))
        icon = QDir::fromNativeSeparators(dir.absoluteFilePath(icon));

    QJsonObject meta;
    meta[QStringLiteral("action")] = guid;
    meta[QStringLiteral("title")] = title;
    meta[QStringLiteral("icon")] = icon;
    meta[QStringLiteral("group")] = descr[QStringLiteral("group")].toString(QStringLiteral("Шаблоны"));
    meta[QStringLiteral("description")] = descr[QStringLiteral("description")].toString();
    meta[QStringLiteral("needname")] = descr[QStringLiteral("needname")].toBool(true);
    meta[QStringLiteral("needpath")] = false; // шаблоны не создают файлов
    meta[QStringLiteral("needlbr")] = descr[QStringLiteral("needlbr")].toBool(true);
    meta[QStringLiteral("namelen")] = descr[QStringLiteral("namelen")].toInt(255);
    meta[QStringLiteral("validator")] = descr[QStringLiteral("validator")].toString();

    // Пользовательский шаблон: диалог создания помечает его бейджем
    // на иконке (имя иконки темы можно переопределить полем "badge")
    meta[QStringLiteral("usertemplate")] = true;
    meta[QStringLiteral("badge")] = descr[QStringLiteral("badge")].toString(QStringLiteral("UserPurple"));

    QStringList ribbon;
    const QJsonArray ribbonArr = descr[QStringLiteral("ribbon")].toArray();
    for (const QJsonValue &v : ribbonArr)
        ribbon.append(v.toString());

    m_templates.append({guid, meta, ribbon, xmlPath});
    return true;
}

QString ResTemplateRegistry::appendToMetaList(const QString &baseJson) const
{
    if (m_templates.isEmpty())
        return baseJson;

    QJsonDocument doc = QJsonDocument::fromJson(baseJson.toUtf8());
    QJsonObject root = doc.object();

    QJsonArray items = root[QStringLiteral("items")].toArray();
    QJsonObject ribbon = root[QStringLiteral("ribbon")].toObject();

    for (const TemplateInfo &tpl : m_templates)
    {
        items.append(tpl.meta);

        for (const QString &section : tpl.ribbon)
        {
            QJsonArray sectionArr = ribbon[section].toArray();
            sectionArr.append(tpl.guid);
            ribbon[section] = sectionArr;
        }
    }

    root[QStringLiteral("items")] = items;
    root[QStringLiteral("ribbon")] = ribbon;

    doc.setObject(root);
    return QString::fromUtf8(doc.toJson(QJsonDocument::Compact));
}

bool ResTemplateRegistry::contains(const QString &guid) const
{
    for (const TemplateInfo &tpl : m_templates)
    {
        if (!tpl.guid.compare(guid, Qt::CaseInsensitive))
            return true;
    }

    return false;
}

QString ResTemplateRegistry::templatePath(const QString &guid) const
{
    for (const TemplateInfo &tpl : m_templates)
    {
        if (!tpl.guid.compare(guid, Qt::CaseInsensitive))
            return tpl.xmlPath;
    }

    return QString();
}
