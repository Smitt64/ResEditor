#include "resxmlloaderchain.h"
#include "errorsmodel.h"
#include <QObject>
#include <QXmlStreamReader>

ResXmlLoaderChain::ResXmlLoaderChain(ErrorsModel *pErrorsModel) :
    m_current(nullptr),
    m_pErrorsModel(pErrorsModel)
{
}

void ResXmlLoaderChain::addImporter(ResXmlReader *importer)
{
    if (importer)
        m_importers.append(importer);
}

ResXmlReader *ResXmlLoaderChain::importerForTag(const QString &tag) const
{
    for (ResXmlReader *importer : m_importers)
    {
        if (importer->xmlTags().contains(tag))
            return importer;
    }

    return nullptr;
}

void ResXmlLoaderChain::OnResRead(QXmlStreamReader *reader)
{
    ResXmlReader *importer = importerForTag(reader->name().toString());

    if (importer)
        importer->OnResRead(reader);
    else
        reader->skipCurrentElement();
}

bool ResXmlLoaderChain::BeginLoadXml(QXmlStreamReader *reader, QString &name, qint16 &type)
{
    m_current = importerForTag(reader->name().toString());

    if (!m_current)
    {
        if (m_pErrorsModel)
            m_pErrorsModel->appendError(
                QObject::tr("Тип ресурса <%1> не поддерживается, ресурс пропущен")
                    .arg(reader->name().toString()),
                ErrorsModel::TypeWarning);

        reader->skipCurrentElement();
        return false;
    }

    return m_current->BeginLoadXml(reader, name, type);
}

bool ResXmlLoaderChain::SaveXmlToBuffer(ResBuffer *resBuffer)
{
    if (!m_current)
        return false;

    return m_current->SaveXmlToBuffer(resBuffer);
}

void ResXmlLoaderChain::EndLoadXml()
{
    if (m_current)
        m_current->EndLoadXml();

    m_current = nullptr;
}
