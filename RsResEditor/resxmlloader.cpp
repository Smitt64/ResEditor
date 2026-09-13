#include "resxmlloader.h"
#include "lbrobjectinterface.h"
#include "respanel.h"
#include "errorsmodel.h"
#include <QObject>
#include <QApplication>
#include <QXmlStreamReader>

// Корневые теги ресурсов, которые читает этот загрузчик
static const QStringList &panelRootTags()
{
    static const QStringList tags =
    {
        QStringLiteral("panel"),
        QStringLiteral("bscrol"),
        QStringLiteral("scrol"),
        QStringLiteral("lscrol")
    };
    return tags;
}

ResXmlLoader::ResXmlLoader(LbrObjectInterface *pLbrObj, ErrorsModel *pErrorsModel) :
    m_pLbrObj(pLbrObj),
    m_pErrorsModel(pErrorsModel),
    m_BeginOk(false)
{

}

QStringList ResXmlLoader::xmlTags() const
{
    return panelRootTags();
}

void ResXmlLoader::OnResRead(QXmlStreamReader *reader)
{
    if (panelRootTags().contains(reader->name().toString()))
    {
        QScopedPointer<ResPanel> _currentPanel(new ResPanel());
        _currentPanel->loadXmlStream(*reader);
    }
}

bool ResXmlLoader::BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type)
{
    // loadXml/цепочка дёргают нас на каждом стартовом элементе внутри
    // <reslib> — пропускаем всё, что не является панельным ресурсом
    if (!panelRootTags().contains(reader->name().toString()))
    {
        reader->skipCurrentElement();
        return false;
    }

    currentPanel.reset(new ResPanel());
    // Элемент наш, поэтому true даже при ошибке разбора — об ошибке
    // отрапортует SaveXmlToBuffer (иначе ресурс молча пропадёт из импорта)
    m_BeginOk = !currentPanel->loadXmlStream(*reader);

    name = currentPanel->name();
    type = currentPanel->type();

    return true;
}

bool ResXmlLoader::SaveXmlToBuffer(ResBuffer *resBuffer)
{
    if (!m_BeginOk || !currentPanel)
        return false;

    return !currentPanel->save(resBuffer);
}

void ResXmlLoader::EndLoadXml()
{
    currentPanel.reset();
    m_BeginOk = false;
}
