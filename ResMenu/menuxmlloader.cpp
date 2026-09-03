#include "menuxmlloader.h"
#include "editormenubar.h"
#include <lbrobject.h>
#include <resbuffer.h>
#include <QXmlStreamReader>

MenuXmlLoader::MenuXmlLoader()
{
}

MenuXmlLoader::~MenuXmlLoader()
{
}

QStringList MenuXmlLoader::xmlTags() const
{
    return QStringList() << QStringLiteral("menu");
}

void MenuXmlLoader::OnResRead(QXmlStreamReader *reader)
{
    // readXml — перебор ресурсов без загрузки в библиотеку:
    // просто разбираем <menu>, как ResXmlLoader разбирает панели
    if (reader->name() == QLatin1String("menu"))
    {
        EditorMenuBar bar;
        bar.setMenuFromXml(*reader, nullptr);
    }
}

bool MenuXmlLoader::BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type)
{
    // loadXml вызывает нас на каждом стартовом элементе внутри <reslib> —
    // пропускаем всё, что не является меню
    if (reader->name() != QLatin1String("menu"))
    {
        reader->skipCurrentElement();
        return false;
    }

    const QXmlStreamAttributes attrs = reader->attributes();
    name      = attrs.value(QStringLiteral("name")).toString();
    m_comment = attrs.value(QStringLiteral("comment")).toString();
    type      = LbrObject::RES_MENU2; // тело всегда пишется в версии 2

    m_menuBar.reset(new EditorMenuBar());

    // Элемент наш, поэтому true даже при ошибке разбора — об ошибке
    // отрапортует SaveXmlToBuffer (иначе ресурс молча пропадёт из импорта)
    QString error;
    if (!m_menuBar->setMenuFromXml(*reader, &error))
        m_menuBar.reset();

    return true;
}

bool MenuXmlLoader::SaveXmlToBuffer(ResBuffer *resBuffer)
{
    if (!m_menuBar)
        return false;

    // Как MenuEditorWindow::save — версия 2 и комментарий в заголовке ресурса
    resBuffer->setResVersion(2);
    resBuffer->setComment(m_comment);

    QString error;
    return m_menuBar->writeMenu(resBuffer, m_comment, &error);
}

void MenuXmlLoader::EndLoadXml()
{
    m_menuBar.reset();
    m_comment.clear();
}
