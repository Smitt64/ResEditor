#include "resmenu.h"
#include <QIcon>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QStandardPaths>
#include <QPixmap>
#include <QImage>
#include <QDebug>

ResMenu *ResMenu::m_pInstance = nullptr;

namespace {

// ---------------------------------------------------------------------------
// Парсинг PE и дерева ресурсов (без Win32 API — у этих DLL ресурсная секция
// не читается стандартным EnumResource*)
// ---------------------------------------------------------------------------

typedef struct PeReader
{
    const QByteArray &f;
    explicit PeReader(const QByteArray &file) :
        f(file)
    {

    }

    quint8 byteAt(qint64 off) const { return quint8(f.at(int(off))); }
    quint16 le16(qint64 off) const { return quint16(byteAt(off) | (byteAt(off + 1) << 8)); }
    quint32 le32(qint64 off) const { return quint32(byteAt(off)) | (quint32(byteAt(off + 1)) << 8) | (quint32(byteAt(off + 2)) << 16) | (quint32(byteAt(off + 3)) << 24); }
} PeReader;

typedef struct ResEntry
{
    quint32 type;   // тип ресурса (2 = RT_BITMAP)
    quint32 id;     // числовой id (== ICON_<id>)
    quint32 rva;
    quint32 size;
} ResEntry;

// находит .rsrc и возвращает все data-записи дерева ресурсов
QList<ResEntry> readResourceEntries(const QByteArray &file)
{
    QList<ResEntry> out;
    PeReader pe(file);
    if (file.size() < 0x40 || file[0] != 'M' || file[1] != 'Z')
        return out;

    const quint32 peOff = pe.le32(0x3C);
    const quint16 nsec = pe.le16(peOff + 6);
    const quint16 optSize = pe.le16(peOff + 20);
    const qint64 secOff = peOff + 24 + optSize;

    quint32 rsrcVa = 0, rsrcRaw = 0, rsrcSize = 0;
    for (int i = 0; i < nsec; ++i)
    {
        const qint64 o = secOff + i * 40;

        if (file.mid(o, 5) == ".rsrc")
        {
            rsrcVa = pe.le32(o + 12);
            rsrcSize = pe.le32(o + 16);
            rsrcRaw = pe.le32(o + 20);
            break;
        }
    }

    if (!rsrcRaw || !rsrcSize)
        return out;

    const quint32 NONE = 0xFFFFFFFF;

    // рекурсивный обход: тип -> id -> язык -> data entry
    std::function<void(quint32, quint32, quint32)> walk = [&](quint32 dirOff, quint32 type, quint32 id)
    {
        const qint64 base = rsrcRaw + dirOff;
        if (base + 16 > file.size())
            return;

        const int total = pe.le16(base + 12) + pe.le16(base + 14);
        for (int i = 0; i < total; ++i)
        {
            const qint64 eo = base + 16 + i * 8;

            if (eo + 8 > file.size())
                return;

            const quint32 name = pe.le32(eo);
            const quint32 tgt = pe.le32(eo + 4);
            const quint32 nid = name & 0xFFFF;

            if (tgt & 0x80000000)
            {
                // подкаталог
                const quint32 sub = tgt & 0x7FFFFFFF;
                if (type == NONE)
                    walk(sub, nid, NONE);
                else if (id == NONE)
                    walk(sub, type, nid);
                else
                    walk(sub, type, id); // языковой уровень
            }
            else
            {
                // IMAGE_RESOURCE_DATA_ENTRY
                if (type == NONE || id == NONE)
                    continue;

                ResEntry e;
                e.type = type;
                e.id = id;
                e.rva = pe.le32(rsrcRaw + tgt);
                e.size = pe.le32(rsrcRaw + tgt + 4);
                out.append(e);
            }
        }
    };

    walk(0, NONE, NONE);
    Q_UNUSED(rsrcVa);

    return out;
}

// ---------------------------------------------------------------------------
// DIB -> QPixmap (с поддержкой "иконочного" формата: удвоенная высота
// = XOR-картинка + AND-маска прозрачности)
// ---------------------------------------------------------------------------

void appendLe16(QByteArray &b, quint16 v)
{
    b.append(char(v & 0xFF));
    b.append(char(v >> 8));
}

void appendLe32(QByteArray &b, quint32 v)
{
    b.append(char(v & 0xFF));
    b.append(char((v >> 8) & 0xFF));
    b.append(char((v >> 16) & 0xFF));
    b.append(char((v >> 24) & 0xFF));
}

QPixmap dibToPixmap(const QByteArray &dib)
{
    if (dib.size() < 40)
        return {};

    PeReader r(dib);
    const quint32 headerSize = r.le32(0);
    const qint32 width = qint32(r.le32(4));
    qint32 height = qint32(r.le32(8));
    const quint16 bitCount = r.le16(14);
    const quint32 compression = r.le32(16);
    const quint32 clrUsed = r.le32(32);

    if (width <= 0 || height == 0 || compression != 0)
        return {};

    const bool topDown = height < 0;
    if (topDown)
        height = -height;

    // иконка? (высота удвоена: картинка + AND-маска)
    const bool hasMask = height == 2 * width && bitCount <= 8;
    const qint32 realH = hasMask ? height / 2 : height;

    const quint32 colors = clrUsed ? clrUsed : (bitCount <= 8 ? (1u << bitCount) : 0);
    const qint64 headBytes = headerSize + colors * 4;
    const qint64 rowSize = ((qint64(width) * bitCount + 31) / 32) * 4;
    const qint64 xorBytes = rowSize * realH;

    if (headBytes + xorBytes > dib.size())
        return {};

    // собираем корректный BMP-файл: BITMAPFILEHEADER + DIB без маски
    QByteArray img = dib.left(int(headBytes + xorBytes));
    // правим высоту в копии заголовка
    img[8] = char(realH & 0xFF);
    img[9] = char((realH >> 8) & 0xFF);
    img[10] = char((realH >> 16) & 0xFF);
    img[11] = char((realH >> 24) & 0xFF);

    QByteArray bmp;
    bmp.reserve(img.size() + 14);
    bmp.append("BM", 2);
    appendLe32(bmp, quint32(14 + img.size()));
    appendLe16(bmp, 0);
    appendLe16(bmp, 0);
    appendLe32(bmp, quint32(14 + headBytes));
    bmp.append(img);

    QImage qimg = QImage::fromData(bmp, "BMP");
    if (qimg.isNull())
        return {};

    // AND-маска -> альфа-канал
    if (hasMask)
    {
        const QByteArray mask = dib.mid(int(headBytes + xorBytes));
        const qint64 maskRow = ((qint64(width) + 31) / 32) * 4;
        qimg = qimg.convertToFormat(QImage::Format_ARGB32);

        for (int y = 0; y < realH && (y + 1) * maskRow <= mask.size(); ++y)
        {
            const auto *row = reinterpret_cast<const uchar *>(mask.constData()) + y * maskRow;
            const int imgY = topDown ? y : realH - 1 - y; // маска в том же порядке, что и картинка

            for (int x = 0; x < width; ++x)
            {
                if (row[x / 8] & (0x80 >> (x % 8)))
                    qimg.setPixelColor(x, imgY, QColor(0, 0, 0, 0));
            }
        }
    }
    else
    {
        // цвет прозрачности — FF00FF (color key)
        qimg = qimg.convertToFormat(QImage::Format_ARGB32);
        const QRgb key = qRgb(0xFF, 0x00, 0xFF);

        for (int y = 0; y < qimg.height(); ++y)
        {
            auto *row = reinterpret_cast<QRgb *>(qimg.scanLine(y));
            for (int x = 0; x < qimg.width(); ++x)
            {
                if (qRgb(qRed(row[x]), qGreen(row[x]), qBlue(row[x])) == key)
                    row[x] = qRgba(0, 0, 0, 0);
            }
        }
    }

    return QPixmap::fromImage(qimg);
}
} // namespace

// ---------------------------------------------------------------------------------------------------

class ResMenuPrivate
{
    Q_DECLARE_PUBLIC(ResMenu)  // Объявляет q_ptr и q_func()

public:
    ResMenuPrivate(ResMenu *q) :
        q_ptr(q)
    {
        loadIcons();
        loadIconTags();
    }

    ~ResMenuPrivate()
    {

    }

    QString findResourceDll() const
    {
        const QString dllName = "RsBankLibRes.dll";
        QStringList searchPaths;

        // 1. Текущий рабочий каталог
        searchPaths << QDir::currentPath();

        // 2. Каталог приложения
        searchPaths << QCoreApplication::applicationDirPath();

        // 3. Каталог, где лежит сам exe (то же самое, что и пункт 2, но через absolutePath)
        searchPaths << QFileInfo(QCoreApplication::applicationFilePath()).absolutePath();

// 4. Системные пути (для Windows)
#ifdef Q_OS_WIN
        searchPaths << QStandardPaths::writableLocation(QStandardPaths::GenericDataLocation);
        searchPaths << QStandardPaths::writableLocation(QStandardPaths::ApplicationsLocation);
#endif

        // 5. Путь из переменной окружения (если задана)
        if (qEnvironmentVariableIsSet("RSTOOL_LIB_PATH"))
            searchPaths << qgetenv("RSTOOL_LIB_PATH");

        // Ищем в каждом пути
        for (const QString& path : qAsConst(searchPaths))
        {
            QString fullPath = QDir(path).absoluteFilePath(dllName);

            if (QFileInfo::exists(fullPath))
            {
                QString absolutePath = QFileInfo(fullPath).absoluteFilePath();
                return absolutePath;
            }
        }

        return QString();
    }

    void loadIcons()
    {
        m_ResDllPath = findResourceDll();

        QFile f(m_ResDllPath);
        if (!f.open(QIODevice::ReadOnly))
        {
            dllMissing = true;
            return;
        }

        const QByteArray file = f.readAll();
        const QList<ResEntry> entries = readResourceEntries(file);
        QMap<quint32, ResEntry> bitmaps; // id -> запись (из языков берём первую)

        for (const ResEntry &e : entries)
        {
            if (e.type == 2 && !bitmaps.contains(e.id))
                bitmaps.insert(e.id, e);
        }

        PeReader pe(file);
        quint32 rsrcVa = 0, rsrcRaw = 0;
        const quint32 peOff = pe.le32(0x3C);
        const quint16 nsec = pe.le16(peOff + 6);
        const qint64 secOff = peOff + 24 + pe.le16(peOff + 20);
        for (int i = 0; i < nsec; ++i)
        {
            const qint64 o = secOff + i * 40;
            if (file.mid(o, 5) == ".rsrc")
            {
                rsrcVa = pe.le32(o + 12);
                rsrcRaw = pe.le32(o + 20);
                break;
            }
        }

        for (auto it = bitmaps.constBegin(); it != bitmaps.constEnd(); ++it)
        {
            const ResEntry &e = it.value();
            const qint64 foff = rsrcRaw + (qint64(e.rva) - rsrcVa);
            if (foff < 0 || foff + e.size > file.size())
                continue;

            QPixmap pm = dibToPixmap(file.mid(int(foff), int(e.size)));
            if (pm.isNull())
                continue;

            m_pPixMaps.insert(e.id, pm);
        }

        //qDebug() << m_pPixMaps;
    }

    // Теги/названия пунктов из :/json/IconTags.json
    // (массив "icons": icon = ICON_<id>, tags, titles)
    void loadIconTags()
    {
        QFile f(QStringLiteral(":/json/IconTags.json"));
        if (!f.open(QIODevice::ReadOnly))
            return;

        const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
        if (!doc.isObject())
            return;

        const QJsonArray icons = doc.object().value(QStringLiteral("icons")).toArray();
        for (const QJsonValue &v : icons)
        {
            const QJsonObject o = v.toObject();
            const QString icon = o.value(QStringLiteral("icon")).toString();

            if (!icon.startsWith(QStringLiteral("ICON_")))
                continue;

            bool ok = false;
            const quint32 id = icon.mid(5).toUInt(&ok);
            if (!ok)
                continue;

            QStringList tags, titles;
            for (const QJsonValue &t : o.value(QStringLiteral("tags")).toArray())
                tags << t.toString();
            for (const QJsonValue &t : o.value(QStringLiteral("titles")).toArray())
                titles << t.toString();

            if (!tags.isEmpty())
                m_IconTags.insert(id, tags);
            if (!titles.isEmpty())
                m_IconTitles.insert(id, titles);
        }
    }

private:
    ResMenu * const q_ptr;  // Указатель на публичный класс

    bool dllMissing = false;
    QMap<quint32, QPixmap> m_pPixMaps;
    QMap<quint32, QStringList> m_IconTags;   // id -> теги
    QMap<quint32, QStringList> m_IconTitles; // id -> названия пунктов
    QString m_ResDllPath;
};

// ---------------------------------------------------------------------------------------------------

ResMenu::ResMenu() :
    d_ptr(new ResMenuPrivate(this))
{
}

ResMenu::~ResMenu()
{
}

ResMenu *ResMenu::inst()
{
    if (!m_pInstance)
        m_pInstance = new ResMenu();

    return m_pInstance;
}

QPixmap ResMenu::getResPixMap(const qint32 &id)
{
    Q_D(ResMenu);

    // предпочитаем 24px-вариант (id + 2000), иначе обычный
    auto it = d->m_pPixMaps.constFind(quint32(id) + 2000);
    if (it != d->m_pPixMaps.constEnd())
        return it.value();

    return d->m_pPixMaps.value(quint32(id));
}

QIcon ResMenu::getResIcon(const qint32 &id)
{
    QPixmap pm = getResPixMap(id);
    return QIcon(pm);
}

QMap<quint32, QPixmap> ResMenu::resPixMaps() const
{
    Q_D(const ResMenu);
    return d->m_pPixMaps;
}

QString ResMenu::resDllPath() const
{
    Q_D(const ResMenu);
    return d->m_ResDllPath;
}

QStringList ResMenu::resIconTags(const qint32 &id) const
{
    Q_D(const ResMenu);
    return d->m_IconTags.value(quint32(id));
}

QStringList ResMenu::resIconTitles(const qint32 &id) const
{
    Q_D(const ResMenu);
    return d->m_IconTitles.value(quint32(id));
}

int ResMenu::resIconTagsCount() const
{
    Q_D(const ResMenu);
    return d->m_IconTags.size();
}