#include "reskeymap.h"

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>

bool ResKeyMap::m_loaded = false;
QVector<ResKeyMap::Entry> ResKeyMap::m_entries;
QHash<int, int> ResKeyMap::m_seqToCode;
QHash<int, int> ResKeyMap::m_codeToSeq;

void ResKeyMap::ensureLoaded()
{
    if (m_loaded)
        return;
    m_loaded = true;

    QFile f(QStringLiteral(":/json/KeyCodes.json"));
    if (!f.open(QIODevice::ReadOnly))
        return;

    const QJsonDocument doc = QJsonDocument::fromJson(f.readAll());
    if (!doc.isObject())
        return;

    const QJsonArray keys = doc.object().value(QStringLiteral("keys")).toArray();
    m_entries.reserve(keys.size());

    for (const QJsonValue &v : keys)
    {
        const QJsonObject o = v.toObject();
        Entry e;
        e.code = o.value(QStringLiteral("code")).toInt();
        e.seq  = o.value(QStringLiteral("seq")).toString();
        e.name = o.value(QStringLiteral("name")).toString();

        if (e.code <= 0)
            continue;

        // Прямое отображение: первая seq-запись для кода — каноническая
        if (!e.seq.isEmpty() && !m_codeToSeq.contains(e.code))
            m_codeToSeq.insert(e.code, m_entries.size());

        // Обратное отображение: все seq-записи
        if (!e.seq.isEmpty())
        {
            const QKeySequence ks = QKeySequence::fromString(e.seq, QKeySequence::PortableText);
            if (ks.count() == 1 && !m_seqToCode.contains(ks[0]))
                m_seqToCode.insert(ks[0], e.code);
        }

        m_entries.append(e);
    }
}

const QVector<ResKeyMap::Entry> &ResKeyMap::entries()
{
    ensureLoaded();
    return m_entries;
}

int ResKeyMap::codeFromKeySequence(const QKeySequence &seq)
{
    ensureLoaded();

    if (seq.isEmpty() || seq.count() != 1)
        return -1;

    return m_seqToCode.value(seq[0], -1);
}

QKeySequence ResKeyMap::keySequenceFromCode(int code)
{
    ensureLoaded();

    const int idx = m_codeToSeq.value(code, -1);
    if (idx < 0)
        return QKeySequence();

    return QKeySequence::fromString(m_entries.at(idx).seq, QKeySequence::PortableText);
}

QString ResKeyMap::nameFromCode(int code)
{
    ensureLoaded();

    for (const Entry &e : qAsConst(m_entries))
    {
        if (e.code == code && !e.name.isEmpty())
            return e.name;
    }

    return QString();
}

QString ResKeyMap::textFromCode(int code)
{
    const QKeySequence seq = keySequenceFromCode(code);
    if (!seq.isEmpty())
        return seq.toString(QKeySequence::NativeText);

    return nameFromCode(code);
}
